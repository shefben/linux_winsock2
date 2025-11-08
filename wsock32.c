/*
 * wsock32.c - Winsock 1.1 Implementation
 * Provides Windows Socket API 1.1 (wsock32.dll) compatibility on Linux
 */

#ifdef __linux__

#include "winsock.h"
#include <pthread.h>
#include <sys/time.h>

/* Thread-local storage for blocking state */
static __thread BOOL g_blocking_in_progress = FALSE;
static __thread BOOL g_blocking_cancelled = FALSE;
static __thread LPWSABLOCKINGHOOK g_blocking_hook = NULL;

/* Default blocking hook - just returns FALSE to continue */
static BOOL WSAAPI default_blocking_hook(void)
{
    /* In a real implementation, this would yield to other tasks */
    /* For now, just check if cancelled */
    return g_blocking_cancelled;
}

/* ============================================================================
 * Winsock 1.1 Blocking Hook Functions
 * ============================================================================ */

LPWSABLOCKINGHOOK WSAAPI WSASetBlockingHook(LPWSABLOCKINGHOOK lpBlockFunc)
{
    LPWSABLOCKINGHOOK prev_hook;

    if (lpBlockFunc == NULL) {
        WSASetLastError(WSAEINVAL);
        return NULL;
    }

    prev_hook = g_blocking_hook;
    g_blocking_hook = lpBlockFunc;

    return prev_hook ? prev_hook : default_blocking_hook;
}

int WSAAPI WSAUnhookBlockingHook(void)
{
    g_blocking_hook = NULL;
    return 0;
}

int WSAAPI WSACancelBlockingCall(void)
{
    if (!g_blocking_in_progress) {
        WSASetLastError(WSAEINVAL);
        return SOCKET_ERROR;
    }

    g_blocking_cancelled = TRUE;
    return 0;
}

BOOL WSAAPI WSAIsBlocking(void)
{
    return g_blocking_in_progress;
}

/* ============================================================================
 * Winsock 1.1 Async Service/Protocol Resolution
 * ============================================================================ */

/* Async request structure for service/protocol lookups */
typedef struct AsyncServiceRequest {
    pthread_t thread;
    HANDLE hWnd;
    unsigned int wMsg;
    char* buffer;
    int buflen;
    int completed;
    int error;
    /* Request parameters */
    union {
        struct {
            char name[256];
            char proto[256];
        } servbyname;
        struct {
            int port;
            char proto[256];
        } servbyport;
        struct {
            char name[256];
        } protobyname;
        struct {
            int number;
        } protobynumber;
    } params;
} AsyncServiceRequest;

/* Thread function for async getservbyname */
static void* async_getservbyname_thread(void* arg)
{
    AsyncServiceRequest* req = (AsyncServiceRequest*)arg;
    struct servent* result;
    struct servent* serv_copy;

    result = getservbyname(req->params.servbyname.name,
                          req->params.servbyname.proto[0] ? req->params.servbyname.proto : NULL);

    if (result != NULL && req->buffer != NULL && req->buflen >= sizeof(struct servent)) {
        serv_copy = (struct servent*)req->buffer;
        memcpy(serv_copy, result, sizeof(struct servent));
        req->error = 0;
    } else {
        req->error = WSAHOST_NOT_FOUND;
    }

    req->completed = 1;
    return NULL;
}

HANDLE WSAAPI WSAAsyncGetServByName(HANDLE hWnd, unsigned int wMsg,
                                    const char* name, const char* proto,
                                    char* buf, int buflen)
{
    AsyncServiceRequest* req;

    if (name == NULL || buf == NULL || buflen < sizeof(struct servent)) {
        WSASetLastError(WSAEINVAL);
        return NULL;
    }

    req = (AsyncServiceRequest*)malloc(sizeof(AsyncServiceRequest));
    if (req == NULL) {
        WSASetLastError(WSAENOBUFS);
        return NULL;
    }

    memset(req, 0, sizeof(AsyncServiceRequest));
    req->hWnd = hWnd;
    req->wMsg = wMsg;
    req->buffer = buf;
    req->buflen = buflen;
    strncpy(req->params.servbyname.name, name, sizeof(req->params.servbyname.name) - 1);
    if (proto != NULL) {
        strncpy(req->params.servbyname.proto, proto, sizeof(req->params.servbyname.proto) - 1);
    }

    if (pthread_create(&req->thread, NULL, async_getservbyname_thread, req) != 0) {
        free(req);
        WSASetLastError(WSAENOBUFS);
        return NULL;
    }

    pthread_detach(req->thread);
    return (HANDLE)req;
}

/* Thread function for async getservbyport */
static void* async_getservbyport_thread(void* arg)
{
    AsyncServiceRequest* req = (AsyncServiceRequest*)arg;
    struct servent* result;
    struct servent* serv_copy;

    result = getservbyport(req->params.servbyport.port,
                          req->params.servbyport.proto[0] ? req->params.servbyport.proto : NULL);

    if (result != NULL && req->buffer != NULL && req->buflen >= sizeof(struct servent)) {
        serv_copy = (struct servent*)req->buffer;
        memcpy(serv_copy, result, sizeof(struct servent));
        req->error = 0;
    } else {
        req->error = WSAHOST_NOT_FOUND;
    }

    req->completed = 1;
    return NULL;
}

HANDLE WSAAPI WSAAsyncGetServByPort(HANDLE hWnd, unsigned int wMsg,
                                    int port, const char* proto,
                                    char* buf, int buflen)
{
    AsyncServiceRequest* req;

    if (buf == NULL || buflen < sizeof(struct servent)) {
        WSASetLastError(WSAEINVAL);
        return NULL;
    }

    req = (AsyncServiceRequest*)malloc(sizeof(AsyncServiceRequest));
    if (req == NULL) {
        WSASetLastError(WSAENOBUFS);
        return NULL;
    }

    memset(req, 0, sizeof(AsyncServiceRequest));
    req->hWnd = hWnd;
    req->wMsg = wMsg;
    req->buffer = buf;
    req->buflen = buflen;
    req->params.servbyport.port = port;
    if (proto != NULL) {
        strncpy(req->params.servbyport.proto, proto, sizeof(req->params.servbyport.proto) - 1);
    }

    if (pthread_create(&req->thread, NULL, async_getservbyport_thread, req) != 0) {
        free(req);
        WSASetLastError(WSAENOBUFS);
        return NULL;
    }

    pthread_detach(req->thread);
    return (HANDLE)req;
}

/* Thread function for async getprotobyname */
static void* async_getprotobyname_thread(void* arg)
{
    AsyncServiceRequest* req = (AsyncServiceRequest*)arg;
    struct protoent* result;
    struct protoent* proto_copy;

    result = getprotobyname(req->params.protobyname.name);

    if (result != NULL && req->buffer != NULL && req->buflen >= sizeof(struct protoent)) {
        proto_copy = (struct protoent*)req->buffer;
        memcpy(proto_copy, result, sizeof(struct protoent));
        req->error = 0;
    } else {
        req->error = WSAHOST_NOT_FOUND;
    }

    req->completed = 1;
    return NULL;
}

HANDLE WSAAPI WSAAsyncGetProtoByName(HANDLE hWnd, unsigned int wMsg,
                                     const char* name, char* buf, int buflen)
{
    AsyncServiceRequest* req;

    if (name == NULL || buf == NULL || buflen < sizeof(struct protoent)) {
        WSASetLastError(WSAEINVAL);
        return NULL;
    }

    req = (AsyncServiceRequest*)malloc(sizeof(AsyncServiceRequest));
    if (req == NULL) {
        WSASetLastError(WSAENOBUFS);
        return NULL;
    }

    memset(req, 0, sizeof(AsyncServiceRequest));
    req->hWnd = hWnd;
    req->wMsg = wMsg;
    req->buffer = buf;
    req->buflen = buflen;
    strncpy(req->params.protobyname.name, name, sizeof(req->params.protobyname.name) - 1);

    if (pthread_create(&req->thread, NULL, async_getprotobyname_thread, req) != 0) {
        free(req);
        WSASetLastError(WSAENOBUFS);
        return NULL;
    }

    pthread_detach(req->thread);
    return (HANDLE)req;
}

/* Thread function for async getprotobynumber */
static void* async_getprotobynumber_thread(void* arg)
{
    AsyncServiceRequest* req = (AsyncServiceRequest*)arg;
    struct protoent* result;
    struct protoent* proto_copy;

    result = getprotobynumber(req->params.protobynumber.number);

    if (result != NULL && req->buffer != NULL && req->buflen >= sizeof(struct protoent)) {
        proto_copy = (struct protoent*)req->buffer;
        memcpy(proto_copy, result, sizeof(struct protoent));
        req->error = 0;
    } else {
        req->error = WSAHOST_NOT_FOUND;
    }

    req->completed = 1;
    return NULL;
}

HANDLE WSAAPI WSAAsyncGetProtoByNumber(HANDLE hWnd, unsigned int wMsg,
                                       int number, char* buf, int buflen)
{
    AsyncServiceRequest* req;

    if (buf == NULL || buflen < sizeof(struct protoent)) {
        WSASetLastError(WSAEINVAL);
        return NULL;
    }

    req = (AsyncServiceRequest*)malloc(sizeof(AsyncServiceRequest));
    if (req == NULL) {
        WSASetLastError(WSAENOBUFS);
        return NULL;
    }

    memset(req, 0, sizeof(AsyncServiceRequest));
    req->hWnd = hWnd;
    req->wMsg = wMsg;
    req->buffer = buf;
    req->buflen = buflen;
    req->params.protobynumber.number = number;

    if (pthread_create(&req->thread, NULL, async_getprotobynumber_thread, req) != 0) {
        free(req);
        WSASetLastError(WSAENOBUFS);
        return NULL;
    }

    pthread_detach(req->thread);
    return (HANDLE)req;
}

#endif /* __linux__ */
