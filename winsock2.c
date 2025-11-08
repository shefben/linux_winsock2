/*
 * Linux Winsock2 Wrapper Implementation
 * Provides Windows Winsock2 API on Linux using POSIX sockets
 */

#ifdef __linux__

#include "winsock2.h"
#include "ws2tcpip.h"
#include "mswsock.h"
#include <pthread.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/eventfd.h>
#include <limits.h>

/* Thread-local storage for last error */
static __thread int g_wsa_last_error = 0;

/* Initialization counter */
static int g_wsa_init_count = 0;
static pthread_mutex_t g_wsa_init_mutex = PTHREAD_MUTEX_INITIALIZER;

/* Event structure implementation */
typedef struct WSAEventStruct {
    int eventfd;
    int manual_reset;
    int signaled;
    pthread_mutex_t mutex;
} WSAEventStruct;

/* Error code mapping from errno to WSA errors */
static int errno_to_wsa_error(int err)
{
    switch (err) {
        case 0: return 0;
        case EINTR: return WSAEINTR;
        case EBADF: return WSAEBADF;
        case EACCES: return WSAEACCES;
        case EFAULT: return WSAEFAULT;
        case EINVAL: return WSAEINVAL;
        case EMFILE: return WSAEMFILE;
        case EWOULDBLOCK: return WSAEWOULDBLOCK;
        case EINPROGRESS: return WSAEINPROGRESS;
        case EALREADY: return WSAEALREADY;
        case ENOTSOCK: return WSAENOTSOCK;
        case EDESTADDRREQ: return WSAEDESTADDRREQ;
        case EMSGSIZE: return WSAEMSGSIZE;
        case EPROTOTYPE: return WSAEPROTOTYPE;
        case ENOPROTOOPT: return WSAENOPROTOOPT;
        case EPROTONOSUPPORT: return WSAEPROTONOSUPPORT;
        case ESOCKTNOSUPPORT: return WSAESOCKTNOSUPPORT;
        case EOPNOTSUPP: return WSAEOPNOTSUPP;
        case EPFNOSUPPORT: return WSAEPFNOSUPPORT;
        case EAFNOSUPPORT: return WSAEAFNOSUPPORT;
        case EADDRINUSE: return WSAEADDRINUSE;
        case EADDRNOTAVAIL: return WSAEADDRNOTAVAIL;
        case ENETDOWN: return WSAENETDOWN;
        case ENETUNREACH: return WSAENETUNREACH;
        case ENETRESET: return WSAENETRESET;
        case ECONNABORTED: return WSAECONNABORTED;
        case ECONNRESET: return WSAECONNRESET;
        case ENOBUFS: return WSAENOBUFS;
        case EISCONN: return WSAEISCONN;
        case ENOTCONN: return WSAENOTCONN;
        case ESHUTDOWN: return WSAESHUTDOWN;
        case ETOOMANYREFS: return WSAETOOMANYREFS;
        case ETIMEDOUT: return WSAETIMEDOUT;
        case ECONNREFUSED: return WSAECONNREFUSED;
        case ELOOP: return WSAELOOP;
        case ENAMETOOLONG: return WSAENAMETOOLONG;
        case EHOSTDOWN: return WSAEHOSTDOWN;
        case EHOSTUNREACH: return WSAEHOSTUNREACH;
        case ENOTEMPTY: return WSAENOTEMPTY;
        case EUSERS: return WSAEUSERS;
        case EDQUOT: return WSAEDQUOT;
        case ESTALE: return WSAESTALE;
        case EREMOTE: return WSAEREMOTE;
        default: return WSABASEERR + err;
    }
}

static void set_wsa_error_from_errno(void)
{
    g_wsa_last_error = errno_to_wsa_error(errno);
}

/* ============================================================================
 * Core Initialization Functions
 * ============================================================================ */

int WSAAPI WSAStartup(WORD wVersionRequested, LPWSADATA lpWSAData)
{
    int result;

    pthread_mutex_lock(&g_wsa_init_mutex);

    if (lpWSAData == NULL) {
        pthread_mutex_unlock(&g_wsa_init_mutex);
        return WSAEFAULT;
    }

    /* Initialize WSADATA structure */
    memset(lpWSAData, 0, sizeof(WSADATA));
    lpWSAData->wVersion = wVersionRequested;
    lpWSAData->wHighVersion = MAKEWORD(2, 2);
    lpWSAData->iMaxSockets = 0; /* No limit on Linux */
    lpWSAData->iMaxUdpDg = 0; /* No limit on Linux */
    strncpy(lpWSAData->szDescription, "Linux Winsock2 Wrapper", 256);
    strncpy(lpWSAData->szSystemStatus, "Running", 128);
    lpWSAData->lpVendorInfo = NULL;

    /* Check version */
    if (LOBYTE(wVersionRequested) < 1 ||
        (LOBYTE(wVersionRequested) == 1 && HIBYTE(wVersionRequested) < 1)) {
        pthread_mutex_unlock(&g_wsa_init_mutex);
        return WSAVERNOTSUPPORTED;
    }

    g_wsa_init_count++;
    result = 0;

    pthread_mutex_unlock(&g_wsa_init_mutex);
    return result;
}

int WSAAPI WSACleanup(void)
{
    int result;

    pthread_mutex_lock(&g_wsa_init_mutex);

    if (g_wsa_init_count == 0) {
        pthread_mutex_unlock(&g_wsa_init_mutex);
        return WSANOTINITIALISED;
    }

    g_wsa_init_count--;
    result = 0;

    pthread_mutex_unlock(&g_wsa_init_mutex);
    return result;
}

int WSAAPI WSAGetLastError(void)
{
    return g_wsa_last_error;
}

void WSAAPI WSASetLastError(int iError)
{
    g_wsa_last_error = iError;
}

/* ============================================================================
 * Windows-Specific Socket Functions
 * Note: Basic POSIX functions (socket, bind, etc.) already exist on Linux
 * We only implement Windows-specific wrappers here
 * ============================================================================ */

int WSAAPI closesocket(SOCKET s)
{
    int result;

    result = close((int)s);

    if (result < 0) {
        set_wsa_error_from_errno();
        return SOCKET_ERROR;
    }

    g_wsa_last_error = 0;
    return 0;
}

/* Note: getsockname and getpeername are available as POSIX functions */

/* Note: getsockopt, setsockopt, ioctlsocket available as POSIX functions.
 * Windows ioctlsocket maps directly to Linux ioctl */

int WSAAPI ioctlsocket(SOCKET s, long cmd, unsigned long* argp)
{
    int result;

    result = ioctl((int)s, (unsigned long)cmd, argp);

    if (result < 0) {
        set_wsa_error_from_errno();
        return SOCKET_ERROR;
    }

    g_wsa_last_error = 0;
    return 0;
}

/* Note: send, recv, sendto, recvfrom available as POSIX functions */

/* ============================================================================
 * FD_SET Helper Functions
 * ============================================================================ */

int __WSA_FD_ISSET(SOCKET fd, fd_set* set)
{
    unsigned int i;

    for (i = 0; i < set->fd_count; i++) {
        if (set->fd_array[i] == fd) {
            return 1;
        }
    }
    return 0;
}

/* Note: select() available as POSIX function.
 * Name resolution functions (gethostbyname, gethostbyaddr, gethostname,
 * getservbyname, getservbyport, getprotobyname, getprotobynumber) are
 * available as POSIX functions and can be used directly. */

#endif /* __linux__ */
