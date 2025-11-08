/*
 * WSA Event and Async Functions Implementation
 * Implements WSACreateEvent, WSAEventSelect, WSAAsyncSelect, and related functions
 */

#ifdef __linux__

#include "winsock2.h"
#include "ws2tcpip.h"
#include <pthread.h>
#include <sys/eventfd.h>
#include <sys/epoll.h>
#include <sys/time.h>
#include <signal.h>
#include <time.h>

extern __thread int g_wsa_last_error;

/* Event structure */
typedef struct WSAEventStruct {
    int eventfd;
    int manual_reset;
    int signaled;
    pthread_mutex_t mutex;
} WSAEventStruct;

/* Socket event mapping structure */
typedef struct SocketEventMap {
    SOCKET sock;
    WSAEVENT event;
    long network_events;
    int epoll_fd;
    pthread_t thread;
    int running;
    pthread_mutex_t mutex;
    struct SocketEventMap* next;
} SocketEventMap;

static SocketEventMap* g_socket_event_map = NULL;
static pthread_mutex_t g_map_mutex = PTHREAD_MUTEX_INITIALIZER;

/* Async request structure */
typedef struct AsyncRequest {
    HANDLE hWnd;
    unsigned int wMsg;
    void* buffer;
    int buflen;
    int type; /* 0=host by name, 1=host by addr, 2=serv by name, etc */
    char* name;
    char* addr;
    int len;
    int addr_type;
    int port;
    char* proto;
    int number;
    pthread_t thread;
    int cancelled;
    struct AsyncRequest* next;
} AsyncRequest;

static AsyncRequest* g_async_requests = NULL;
static pthread_mutex_t g_async_mutex = PTHREAD_MUTEX_INITIALIZER;
static int g_async_handle_counter = 1;

/* ============================================================================
 * Event Functions
 * ============================================================================ */

WSAEVENT WSAAPI WSACreateEvent(void)
{
    WSAEventStruct* event;
    int efd;

    efd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (efd < 0) {
        g_wsa_last_error = WSA_NOT_ENOUGH_MEMORY;
        return NULL;
    }

    event = (WSAEventStruct*)malloc(sizeof(WSAEventStruct));
    if (event == NULL) {
        close(efd);
        g_wsa_last_error = WSA_NOT_ENOUGH_MEMORY;
        return NULL;
    }

    event->eventfd = efd;
    event->manual_reset = 1;
    event->signaled = 0;
    pthread_mutex_init(&event->mutex, NULL);

    g_wsa_last_error = 0;
    return (WSAEVENT)event;
}

BOOL WSAAPI WSACloseEvent(WSAEVENT hEvent)
{
    WSAEventStruct* event;

    if (hEvent == NULL) {
        g_wsa_last_error = WSA_INVALID_HANDLE;
        return FALSE;
    }

    event = (WSAEventStruct*)hEvent;

    close(event->eventfd);
    pthread_mutex_destroy(&event->mutex);
    free(event);

    g_wsa_last_error = 0;
    return TRUE;
}

BOOL WSAAPI WSASetEvent(WSAEVENT hEvent)
{
    WSAEventStruct* event;
    uint64_t val;

    if (hEvent == NULL) {
        g_wsa_last_error = WSA_INVALID_HANDLE;
        return FALSE;
    }

    event = (WSAEventStruct*)hEvent;

    pthread_mutex_lock(&event->mutex);

    if (!event->signaled) {
        val = 1;
        if (write(event->eventfd, &val, sizeof(val)) != sizeof(val)) {
            pthread_mutex_unlock(&event->mutex);
            g_wsa_last_error = WSAENETDOWN;
            return FALSE;
        }
        event->signaled = 1;
    }

    pthread_mutex_unlock(&event->mutex);

    g_wsa_last_error = 0;
    return TRUE;
}

BOOL WSAAPI WSAResetEvent(WSAEVENT hEvent)
{
    WSAEventStruct* event;
    uint64_t val;

    if (hEvent == NULL) {
        g_wsa_last_error = WSA_INVALID_HANDLE;
        return FALSE;
    }

    event = (WSAEventStruct*)hEvent;

    pthread_mutex_lock(&event->mutex);

    if (event->signaled) {
        while (read(event->eventfd, &val, sizeof(val)) == sizeof(val)) {
            /* Drain the eventfd */
        }
        event->signaled = 0;
    }

    pthread_mutex_unlock(&event->mutex);

    g_wsa_last_error = 0;
    return TRUE;
}

DWORD WSAAPI WSAWaitForMultipleEvents(DWORD cEvents, const WSAEVENT* lphEvents,
                                      BOOL fWaitAll, DWORD dwTimeout,
                                      BOOL fAlertable)
{
    fd_set readfds;
    struct timeval tv;
    struct timeval* ptv;
    int max_fd;
    DWORD i;
    int result;
    WSAEventStruct* event;

    (void)fWaitAll; /* Simplified implementation */
    (void)fAlertable;

    if (cEvents == 0 || cEvents > WSA_MAXIMUM_WAIT_EVENTS || lphEvents == NULL) {
        g_wsa_last_error = WSA_INVALID_PARAMETER;
        return WSA_WAIT_FAILED;
    }

    FD_ZERO(&readfds);
    max_fd = -1;

    for (i = 0; i < cEvents; i++) {
        if (lphEvents[i] == NULL) {
            g_wsa_last_error = WSA_INVALID_HANDLE;
            return WSA_WAIT_FAILED;
        }
        event = (WSAEventStruct*)lphEvents[i];
        FD_SET(event->eventfd, &readfds);
        if (event->eventfd > max_fd) {
            max_fd = event->eventfd;
        }
    }

    if (dwTimeout == WSA_INFINITE) {
        ptv = NULL;
    } else {
        tv.tv_sec = dwTimeout / 1000;
        tv.tv_usec = (dwTimeout % 1000) * 1000;
        ptv = &tv;
    }

    result = select(max_fd + 1, &readfds, NULL, NULL, ptv);

    if (result < 0) {
        g_wsa_last_error = WSAENETDOWN;
        return WSA_WAIT_FAILED;
    }

    if (result == 0) {
        g_wsa_last_error = 0;
        return WSA_WAIT_TIMEOUT;
    }

    /* Find which event was signaled */
    for (i = 0; i < cEvents; i++) {
        event = (WSAEventStruct*)lphEvents[i];
        if (FD_ISSET(event->eventfd, &readfds)) {
            g_wsa_last_error = 0;
            return WSA_WAIT_EVENT_0 + i;
        }
    }

    g_wsa_last_error = 0;
    return WSA_WAIT_TIMEOUT;
}

/* ============================================================================
 * WSAEventSelect Implementation
 * ============================================================================ */

/* Thread function for monitoring socket events */
static void* event_monitor_thread(void* arg)
{
    SocketEventMap* map;
    struct epoll_event ev;
    struct epoll_event events[10];
    int nfds;
    int i;
    WSAEventStruct* event_obj;
    uint32_t epoll_events;

    map = (SocketEventMap*)arg;

    while (1) {
        pthread_mutex_lock(&map->mutex);
        if (!map->running) {
            pthread_mutex_unlock(&map->mutex);
            break;
        }
        pthread_mutex_unlock(&map->mutex);

        nfds = epoll_wait(map->epoll_fd, events, 10, 100);

        if (nfds < 0) {
            if (errno == EINTR) {
                continue;
            }
            break;
        }

        for (i = 0; i < nfds; i++) {
            event_obj = (WSAEventStruct*)map->event;
            if (event_obj != NULL) {
                WSASetEvent(map->event);
            }
        }
    }

    return NULL;
}

int WSAAPI WSAEventSelect(SOCKET s, WSAEVENT hEventObject, long lNetworkEvents)
{
    SocketEventMap* map;
    SocketEventMap* existing;
    struct epoll_event ev;
    int epoll_fd;
    uint32_t epoll_events;

    /* Find existing mapping */
    pthread_mutex_lock(&g_map_mutex);

    existing = g_socket_event_map;
    while (existing != NULL) {
        if (existing->sock == s) {
            break;
        }
        existing = existing->next;
    }

    if (existing != NULL) {
        /* Update existing mapping */
        existing->event = hEventObject;
        existing->network_events = lNetworkEvents;
        pthread_mutex_unlock(&g_map_mutex);
        g_wsa_last_error = 0;
        return 0;
    }

    /* Create new mapping */
    map = (SocketEventMap*)malloc(sizeof(SocketEventMap));
    if (map == NULL) {
        pthread_mutex_unlock(&g_map_mutex);
        g_wsa_last_error = WSA_NOT_ENOUGH_MEMORY;
        return SOCKET_ERROR;
    }

    epoll_fd = epoll_create1(EPOLL_CLOEXEC);
    if (epoll_fd < 0) {
        free(map);
        pthread_mutex_unlock(&g_map_mutex);
        g_wsa_last_error = WSAENETDOWN;
        return SOCKET_ERROR;
    }

    /* Setup epoll events */
    epoll_events = 0;
    if (lNetworkEvents & (FD_READ | FD_ACCEPT)) {
        epoll_events |= EPOLLIN;
    }
    if (lNetworkEvents & (FD_WRITE | FD_CONNECT)) {
        epoll_events |= EPOLLOUT;
    }
    if (lNetworkEvents & FD_OOB) {
        epoll_events |= EPOLLPRI;
    }
    if (lNetworkEvents & FD_CLOSE) {
        epoll_events |= EPOLLRDHUP;
    }

    ev.events = epoll_events;
    ev.data.fd = (int)s;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, (int)s, &ev) < 0) {
        close(epoll_fd);
        free(map);
        pthread_mutex_unlock(&g_map_mutex);
        g_wsa_last_error = WSAENETDOWN;
        return SOCKET_ERROR;
    }

    map->sock = s;
    map->event = hEventObject;
    map->network_events = lNetworkEvents;
    map->epoll_fd = epoll_fd;
    map->running = 1;
    pthread_mutex_init(&map->mutex, NULL);
    map->next = g_socket_event_map;
    g_socket_event_map = map;

    /* Create monitoring thread */
    if (pthread_create(&map->thread, NULL, event_monitor_thread, map) != 0) {
        g_socket_event_map = map->next;
        close(epoll_fd);
        pthread_mutex_destroy(&map->mutex);
        free(map);
        pthread_mutex_unlock(&g_map_mutex);
        g_wsa_last_error = WSAENETDOWN;
        return SOCKET_ERROR;
    }

    pthread_detach(map->thread);

    pthread_mutex_unlock(&g_map_mutex);

    /* Set socket to non-blocking */
    {
        int flags;
        flags = fcntl((int)s, F_GETFL, 0);
        if (flags >= 0) {
            fcntl((int)s, F_SETFL, flags | O_NONBLOCK);
        }
    }

    g_wsa_last_error = 0;
    return 0;
}

int WSAAPI WSAEnumNetworkEvents(SOCKET s, WSAEVENT hEventObject,
                                LPWSANETWORKEVENTS lpNetworkEvents)
{
    SocketEventMap* map;
    int error;
    socklen_t errlen;

    if (lpNetworkEvents == NULL) {
        g_wsa_last_error = WSAEFAULT;
        return SOCKET_ERROR;
    }

    memset(lpNetworkEvents, 0, sizeof(WSANETWORKEVENTS));

    /* Find socket mapping */
    pthread_mutex_lock(&g_map_mutex);

    map = g_socket_event_map;
    while (map != NULL) {
        if (map->sock == s) {
            break;
        }
        map = map->next;
    }

    pthread_mutex_unlock(&g_map_mutex);

    if (map == NULL) {
        g_wsa_last_error = WSAEINVAL;
        return SOCKET_ERROR;
    }

    /* Get socket error if any */
    errlen = sizeof(error);
    if (getsockopt((int)s, SOL_SOCKET, SO_ERROR, &error, &errlen) == 0) {
        if (error != 0) {
            lpNetworkEvents->lNetworkEvents |= FD_CONNECT;
            lpNetworkEvents->iErrorCode[FD_CONNECT_BIT] = error;
        }
    }

    /* Reset event if provided */
    if (hEventObject != NULL) {
        WSAResetEvent(hEventObject);
    }

    g_wsa_last_error = 0;
    return 0;
}

/* ============================================================================
 * WSAAsyncSelect Implementation
 * ============================================================================ */

int WSAAPI WSAAsyncSelect(SOCKET s, HANDLE hWnd, unsigned int wMsg, long lEvent)
{
    (void)s;
    (void)hWnd;
    (void)wMsg;
    (void)lEvent;

    /* WSAAsyncSelect requires Windows message queue */
    /* Not fully supported on Linux, but we accept the call */

    g_wsa_last_error = 0;
    return 0;
}

/* ============================================================================
 * Async Name Resolution Functions
 * ============================================================================ */

/* Thread function for async gethostbyname */
static void* async_gethostbyname_thread(void* arg)
{
    AsyncRequest* req;
    struct hostent* result;

    req = (AsyncRequest*)arg;

    if (req->cancelled) {
        return NULL;
    }

    result = gethostbyname(req->name);

    if (!req->cancelled && result != NULL) {
        /* Copy result to buffer */
        memcpy(req->buffer, result, sizeof(struct hostent));
    }

    return NULL;
}

HANDLE WSAAPI WSAAsyncGetHostByName(HANDLE hWnd, unsigned int wMsg,
                                    const char* name, char* buf, int buflen)
{
    AsyncRequest* req;
    HANDLE handle;

    req = (AsyncRequest*)malloc(sizeof(AsyncRequest));
    if (req == NULL) {
        g_wsa_last_error = WSA_NOT_ENOUGH_MEMORY;
        return NULL;
    }

    req->hWnd = hWnd;
    req->wMsg = wMsg;
    req->buffer = buf;
    req->buflen = buflen;
    req->type = 0;
    req->name = strdup(name);
    req->cancelled = 0;

    pthread_mutex_lock(&g_async_mutex);
    handle = (HANDLE)(intptr_t)g_async_handle_counter++;
    req->next = g_async_requests;
    g_async_requests = req;
    pthread_mutex_unlock(&g_async_mutex);

    if (pthread_create(&req->thread, NULL, async_gethostbyname_thread, req) != 0) {
        pthread_mutex_lock(&g_async_mutex);
        g_async_requests = req->next;
        pthread_mutex_unlock(&g_async_mutex);
        free(req->name);
        free(req);
        g_wsa_last_error = WSAENETDOWN;
        return NULL;
    }

    pthread_detach(req->thread);

    g_wsa_last_error = 0;
    return handle;
}

/* Thread function for async gethostbyaddr */
static void* async_gethostbyaddr_thread(void* arg)
{
    AsyncRequest* req;
    struct hostent* result;

    req = (AsyncRequest*)arg;

    if (req->cancelled) {
        return NULL;
    }

    result = gethostbyaddr(req->addr, req->len, req->addr_type);

    if (!req->cancelled && result != NULL) {
        memcpy(req->buffer, result, sizeof(struct hostent));
    }

    return NULL;
}

HANDLE WSAAPI WSAAsyncGetHostByAddr(HANDLE hWnd, unsigned int wMsg,
                                    const char* addr, int len, int type,
                                    char* buf, int buflen)
{
    AsyncRequest* req;
    HANDLE handle;

    req = (AsyncRequest*)malloc(sizeof(AsyncRequest));
    if (req == NULL) {
        g_wsa_last_error = WSA_NOT_ENOUGH_MEMORY;
        return NULL;
    }

    req->hWnd = hWnd;
    req->wMsg = wMsg;
    req->buffer = buf;
    req->buflen = buflen;
    req->type = 1;
    req->addr = (char*)malloc(len);
    memcpy(req->addr, addr, len);
    req->len = len;
    req->addr_type = type;
    req->cancelled = 0;

    pthread_mutex_lock(&g_async_mutex);
    handle = (HANDLE)(intptr_t)g_async_handle_counter++;
    req->next = g_async_requests;
    g_async_requests = req;
    pthread_mutex_unlock(&g_async_mutex);

    if (pthread_create(&req->thread, NULL, async_gethostbyaddr_thread, req) != 0) {
        pthread_mutex_lock(&g_async_mutex);
        g_async_requests = req->next;
        pthread_mutex_unlock(&g_async_mutex);
        free(req->addr);
        free(req);
        g_wsa_last_error = WSAENETDOWN;
        return NULL;
    }

    pthread_detach(req->thread);

    g_wsa_last_error = 0;
    return handle;
}

/* Stubs for other async functions */
HANDLE WSAAPI WSAAsyncGetServByName(HANDLE hWnd, unsigned int wMsg,
                                    const char* name, const char* proto,
                                    char* buf, int buflen)
{
    (void)hWnd; (void)wMsg; (void)name; (void)proto; (void)buf; (void)buflen;
    g_wsa_last_error = 0;
    return (HANDLE)(intptr_t)g_async_handle_counter++;
}

HANDLE WSAAPI WSAAsyncGetServByPort(HANDLE hWnd, unsigned int wMsg,
                                    int port, const char* proto,
                                    char* buf, int buflen)
{
    (void)hWnd; (void)wMsg; (void)port; (void)proto; (void)buf; (void)buflen;
    g_wsa_last_error = 0;
    return (HANDLE)(intptr_t)g_async_handle_counter++;
}

HANDLE WSAAPI WSAAsyncGetProtoByName(HANDLE hWnd, unsigned int wMsg,
                                     const char* name, char* buf, int buflen)
{
    (void)hWnd; (void)wMsg; (void)name; (void)buf; (void)buflen;
    g_wsa_last_error = 0;
    return (HANDLE)(intptr_t)g_async_handle_counter++;
}

HANDLE WSAAPI WSAAsyncGetProtoByNumber(HANDLE hWnd, unsigned int wMsg,
                                       int number, char* buf, int buflen)
{
    (void)hWnd; (void)wMsg; (void)number; (void)buf; (void)buflen;
    g_wsa_last_error = 0;
    return (HANDLE)(intptr_t)g_async_handle_counter++;
}

int WSAAPI WSACancelAsyncRequest(HANDLE hAsyncTaskHandle)
{
    AsyncRequest* req;
    AsyncRequest* prev;

    pthread_mutex_lock(&g_async_mutex);

    prev = NULL;
    req = g_async_requests;

    while (req != NULL) {
        if ((HANDLE)(intptr_t)req == hAsyncTaskHandle) {
            req->cancelled = 1;
            if (prev != NULL) {
                prev->next = req->next;
            } else {
                g_async_requests = req->next;
            }
            break;
        }
        prev = req;
        req = req->next;
    }

    pthread_mutex_unlock(&g_async_mutex);

    g_wsa_last_error = 0;
    return 0;
}

#define WSA_MAXIMUM_WAIT_EVENTS  64
#define WSA_INFINITE             0xFFFFFFFF
#define WSA_WAIT_EVENT_0         0
#define WSA_WAIT_FAILED          0xFFFFFFFF
#define WSA_WAIT_TIMEOUT         0x00000102
#define FD_CONNECT_BIT           4

#endif /* __linux__ */
