/*
 * WSA Extended Functions Implementation
 * Implements WSASocket, WSASend, WSARecv, WSAIoctl, and related functions
 */

#ifdef __linux__

#include "winsock2.h"
#include "ws2tcpip.h"
#include "mswsock.h"
#include <pthread.h>
#include <sys/uio.h>

extern __thread int g_wsa_last_error;

static void set_wsa_error_from_errno(void);
static int errno_to_wsa_error(int err);

/* ============================================================================
 * WSASocket Functions
 * ============================================================================ */

SOCKET WSAAPI WSASocketA(int af, int type, int protocol,
                         LPWSAPROTOCOL_INFOA lpProtocolInfo,
                         unsigned int g, DWORD dwFlags)
{
    SOCKET s;
    int sock_type;
    int sock_flags;

    (void)lpProtocolInfo; /* Unused on Linux */
    (void)g; /* Unused on Linux */

    sock_type = type;
    sock_flags = 0;

    /* Handle socket flags */
    if (dwFlags & WSA_FLAG_OVERLAPPED) {
        /* Overlapped I/O - set non-blocking on Linux */
        sock_flags |= SOCK_NONBLOCK;
    }

#ifdef SOCK_CLOEXEC
    if (!(dwFlags & WSA_FLAG_NO_HANDLE_INHERIT)) {
        sock_flags |= SOCK_CLOEXEC;
    }
#endif

    s = (SOCKET)socket(af, sock_type | sock_flags, protocol);

    if (s < 0) {
        set_wsa_error_from_errno();
        return INVALID_SOCKET;
    }

    g_wsa_last_error = 0;
    return s;
}

SOCKET WSAAPI WSASocketW(int af, int type, int protocol,
                         LPWSAPROTOCOL_INFOW lpProtocolInfo,
                         unsigned int g, DWORD dwFlags)
{
    /* Same as WSASocketA for Linux implementation */
    return WSASocketA(af, type, protocol, (LPWSAPROTOCOL_INFOA)lpProtocolInfo, g, dwFlags);
}

/* ============================================================================
 * WSAAccept
 * ============================================================================ */

SOCKET WSAAPI WSAAccept(SOCKET s, struct sockaddr* addr, int* addrlen,
                        void* lpfnCondition, DWORD_PTR dwCallbackData)
{
    SOCKET new_sock;
    socklen_t len;

    (void)lpfnCondition; /* Condition function not supported */
    (void)dwCallbackData;

    if (addrlen != NULL) {
        len = (socklen_t)*addrlen;
    } else {
        len = 0;
    }

    new_sock = (SOCKET)accept((int)s, addr, addrlen != NULL ? &len : NULL);

    if (new_sock < 0) {
        set_wsa_error_from_errno();
        return INVALID_SOCKET;
    }

    if (addrlen != NULL) {
        *addrlen = (int)len;
    }

    g_wsa_last_error = 0;
    return new_sock;
}

/* ============================================================================
 * WSAConnect
 * ============================================================================ */

int WSAAPI WSAConnect(SOCKET s, const struct sockaddr* name, int namelen,
                      LPWSABUF lpCallerData, LPWSABUF lpCalleeData,
                      LPQOS lpSQOS, LPQOS lpGQOS)
{
    int result;

    (void)lpCallerData; /* Not supported on Linux */
    (void)lpCalleeData;
    (void)lpSQOS;
    (void)lpGQOS;

    result = connect((int)s, name, (socklen_t)namelen);

    if (result < 0) {
        set_wsa_error_from_errno();
        return SOCKET_ERROR;
    }

    g_wsa_last_error = 0;
    return 0;
}

/* ============================================================================
 * WSASend / WSARecv Functions
 * ============================================================================ */

int WSAAPI WSASend(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount,
                   DWORD* lpNumberOfBytesSent, DWORD dwFlags,
                   LPWSAOVERLAPPED lpOverlapped,
                   LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
    struct iovec* iov;
    ssize_t result;
    DWORD i;

    (void)lpOverlapped; /* Overlapped I/O not fully supported */
    (void)lpCompletionRoutine;

    if (lpBuffers == NULL || dwBufferCount == 0) {
        g_wsa_last_error = WSAEFAULT;
        return SOCKET_ERROR;
    }

    /* Allocate iovec array */
    iov = (struct iovec*)malloc(dwBufferCount * sizeof(struct iovec));
    if (iov == NULL) {
        g_wsa_last_error = WSA_NOT_ENOUGH_MEMORY;
        return SOCKET_ERROR;
    }

    /* Convert WSABUF to iovec */
    for (i = 0; i < dwBufferCount; i++) {
        iov[i].iov_base = lpBuffers[i].buf;
        iov[i].iov_len = lpBuffers[i].len;
    }

    result = writev((int)s, iov, (int)dwBufferCount);

    free(iov);

    if (result < 0) {
        set_wsa_error_from_errno();
        return SOCKET_ERROR;
    }

    if (lpNumberOfBytesSent != NULL) {
        *lpNumberOfBytesSent = (DWORD)result;
    }

    g_wsa_last_error = 0;
    return 0;
}

int WSAAPI WSASendTo(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount,
                     DWORD* lpNumberOfBytesSent, DWORD dwFlags,
                     const struct sockaddr* lpTo, int iTolen,
                     LPWSAOVERLAPPED lpOverlapped,
                     LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
    struct msghdr msg;
    struct iovec* iov;
    ssize_t result;
    DWORD i;

    (void)lpOverlapped;
    (void)lpCompletionRoutine;

    if (lpBuffers == NULL || dwBufferCount == 0) {
        g_wsa_last_error = WSAEFAULT;
        return SOCKET_ERROR;
    }

    /* Allocate iovec array */
    iov = (struct iovec*)malloc(dwBufferCount * sizeof(struct iovec));
    if (iov == NULL) {
        g_wsa_last_error = WSA_NOT_ENOUGH_MEMORY;
        return SOCKET_ERROR;
    }

    /* Convert WSABUF to iovec */
    for (i = 0; i < dwBufferCount; i++) {
        iov[i].iov_base = lpBuffers[i].buf;
        iov[i].iov_len = lpBuffers[i].len;
    }

    /* Setup message header */
    memset(&msg, 0, sizeof(msg));
    msg.msg_name = (void*)lpTo;
    msg.msg_namelen = (socklen_t)iTolen;
    msg.msg_iov = iov;
    msg.msg_iovlen = dwBufferCount;

    result = sendmsg((int)s, &msg, (int)dwFlags);

    free(iov);

    if (result < 0) {
        set_wsa_error_from_errno();
        return SOCKET_ERROR;
    }

    if (lpNumberOfBytesSent != NULL) {
        *lpNumberOfBytesSent = (DWORD)result;
    }

    g_wsa_last_error = 0;
    return 0;
}

int WSAAPI WSARecv(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount,
                   DWORD* lpNumberOfBytesRecvd, DWORD* lpFlags,
                   LPWSAOVERLAPPED lpOverlapped,
                   LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
    struct iovec* iov;
    ssize_t result;
    DWORD i;

    (void)lpOverlapped;
    (void)lpCompletionRoutine;

    if (lpBuffers == NULL || dwBufferCount == 0) {
        g_wsa_last_error = WSAEFAULT;
        return SOCKET_ERROR;
    }

    /* Allocate iovec array */
    iov = (struct iovec*)malloc(dwBufferCount * sizeof(struct iovec));
    if (iov == NULL) {
        g_wsa_last_error = WSA_NOT_ENOUGH_MEMORY;
        return SOCKET_ERROR;
    }

    /* Convert WSABUF to iovec */
    for (i = 0; i < dwBufferCount; i++) {
        iov[i].iov_base = lpBuffers[i].buf;
        iov[i].iov_len = lpBuffers[i].len;
    }

    result = readv((int)s, iov, (int)dwBufferCount);

    free(iov);

    if (result < 0) {
        set_wsa_error_from_errno();
        return SOCKET_ERROR;
    }

    if (lpNumberOfBytesRecvd != NULL) {
        *lpNumberOfBytesRecvd = (DWORD)result;
    }

    if (lpFlags != NULL) {
        *lpFlags = 0;
    }

    g_wsa_last_error = 0;
    return 0;
}

int WSAAPI WSARecvFrom(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount,
                       DWORD* lpNumberOfBytesRecvd, DWORD* lpFlags,
                       struct sockaddr* lpFrom, int* lpFromlen,
                       LPWSAOVERLAPPED lpOverlapped,
                       LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
    struct msghdr msg;
    struct iovec* iov;
    ssize_t result;
    DWORD i;

    (void)lpOverlapped;
    (void)lpCompletionRoutine;

    if (lpBuffers == NULL || dwBufferCount == 0) {
        g_wsa_last_error = WSAEFAULT;
        return SOCKET_ERROR;
    }

    /* Allocate iovec array */
    iov = (struct iovec*)malloc(dwBufferCount * sizeof(struct iovec));
    if (iov == NULL) {
        g_wsa_last_error = WSA_NOT_ENOUGH_MEMORY;
        return SOCKET_ERROR;
    }

    /* Convert WSABUF to iovec */
    for (i = 0; i < dwBufferCount; i++) {
        iov[i].iov_base = lpBuffers[i].buf;
        iov[i].iov_len = lpBuffers[i].len;
    }

    /* Setup message header */
    memset(&msg, 0, sizeof(msg));
    if (lpFrom != NULL && lpFromlen != NULL) {
        msg.msg_name = lpFrom;
        msg.msg_namelen = (socklen_t)*lpFromlen;
    }
    msg.msg_iov = iov;
    msg.msg_iovlen = dwBufferCount;

    result = recvmsg((int)s, &msg, lpFlags != NULL ? (int)*lpFlags : 0);

    free(iov);

    if (result < 0) {
        set_wsa_error_from_errno();
        return SOCKET_ERROR;
    }

    if (lpNumberOfBytesRecvd != NULL) {
        *lpNumberOfBytesRecvd = (DWORD)result;
    }

    if (lpFromlen != NULL) {
        *lpFromlen = (int)msg.msg_namelen;
    }

    if (lpFlags != NULL) {
        *lpFlags = (DWORD)msg.msg_flags;
    }

    g_wsa_last_error = 0;
    return 0;
}

/* ============================================================================
 * WSARecvMsg / WSASendMsg
 * ============================================================================ */

int WSAAPI WSARecvMsg(SOCKET s, LPWSAMSG lpMsg, DWORD* lpdwNumberOfBytesRecvd,
                      LPWSAOVERLAPPED lpOverlapped,
                      LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
    struct msghdr msg;
    struct iovec* iov;
    ssize_t result;
    DWORD i;

    (void)lpOverlapped;
    (void)lpCompletionRoutine;

    if (lpMsg == NULL) {
        g_wsa_last_error = WSAEFAULT;
        return SOCKET_ERROR;
    }

    /* Allocate iovec array */
    if (lpMsg->dwBufferCount > 0) {
        iov = (struct iovec*)malloc(lpMsg->dwBufferCount * sizeof(struct iovec));
        if (iov == NULL) {
            g_wsa_last_error = WSA_NOT_ENOUGH_MEMORY;
            return SOCKET_ERROR;
        }

        /* Convert WSABUF to iovec */
        for (i = 0; i < lpMsg->dwBufferCount; i++) {
            iov[i].iov_base = lpMsg->lpBuffers[i].buf;
            iov[i].iov_len = lpMsg->lpBuffers[i].len;
        }
    } else {
        iov = NULL;
    }

    /* Setup message header */
    memset(&msg, 0, sizeof(msg));
    msg.msg_name = lpMsg->name;
    msg.msg_namelen = (socklen_t)lpMsg->namelen;
    msg.msg_iov = iov;
    msg.msg_iovlen = lpMsg->dwBufferCount;
    msg.msg_control = lpMsg->Control.buf;
    msg.msg_controllen = lpMsg->Control.len;

    result = recvmsg((int)s, &msg, (int)lpMsg->dwFlags);

    if (iov != NULL) {
        free(iov);
    }

    if (result < 0) {
        set_wsa_error_from_errno();
        return SOCKET_ERROR;
    }

    if (lpdwNumberOfBytesRecvd != NULL) {
        *lpdwNumberOfBytesRecvd = (DWORD)result;
    }

    lpMsg->namelen = (INT)msg.msg_namelen;
    lpMsg->dwFlags = (DWORD)msg.msg_flags;
    lpMsg->Control.len = (unsigned long)msg.msg_controllen;

    g_wsa_last_error = 0;
    return 0;
}

int WSAAPI WSASendMsg(SOCKET s, LPWSAMSG lpMsg, DWORD dwFlags,
                      DWORD* lpNumberOfBytesSent,
                      LPWSAOVERLAPPED lpOverlapped,
                      LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
    struct msghdr msg;
    struct iovec* iov;
    ssize_t result;
    DWORD i;

    (void)lpOverlapped;
    (void)lpCompletionRoutine;

    if (lpMsg == NULL) {
        g_wsa_last_error = WSAEFAULT;
        return SOCKET_ERROR;
    }

    /* Allocate iovec array */
    if (lpMsg->dwBufferCount > 0) {
        iov = (struct iovec*)malloc(lpMsg->dwBufferCount * sizeof(struct iovec));
        if (iov == NULL) {
            g_wsa_last_error = WSA_NOT_ENOUGH_MEMORY;
            return SOCKET_ERROR;
        }

        /* Convert WSABUF to iovec */
        for (i = 0; i < lpMsg->dwBufferCount; i++) {
            iov[i].iov_base = lpMsg->lpBuffers[i].buf;
            iov[i].iov_len = lpMsg->lpBuffers[i].len;
        }
    } else {
        iov = NULL;
    }

    /* Setup message header */
    memset(&msg, 0, sizeof(msg));
    msg.msg_name = lpMsg->name;
    msg.msg_namelen = (socklen_t)lpMsg->namelen;
    msg.msg_iov = iov;
    msg.msg_iovlen = lpMsg->dwBufferCount;
    msg.msg_control = lpMsg->Control.buf;
    msg.msg_controllen = lpMsg->Control.len;

    result = sendmsg((int)s, &msg, (int)dwFlags);

    if (iov != NULL) {
        free(iov);
    }

    if (result < 0) {
        set_wsa_error_from_errno();
        return SOCKET_ERROR;
    }

    if (lpNumberOfBytesSent != NULL) {
        *lpNumberOfBytesSent = (DWORD)result;
    }

    g_wsa_last_error = 0;
    return 0;
}

/* ============================================================================
 * WSAIoctl
 * ============================================================================ */

int WSAAPI WSAIoctl(SOCKET s, DWORD dwIoControlCode,
                    LPVOID lpvInBuffer, DWORD cbInBuffer,
                    LPVOID lpvOutBuffer, DWORD cbOutBuffer,
                    DWORD* lpcbBytesReturned,
                    LPWSAOVERLAPPED lpOverlapped,
                    LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
    int result;

    (void)cbInBuffer;
    (void)cbOutBuffer;
    (void)lpOverlapped;
    (void)lpCompletionRoutine;

    /* Handle specific I/O control codes */
    switch (dwIoControlCode) {
        case FIONREAD:
        case FIONBIO:
        case FIOASYNC:
        case SIOCATMARK:
            result = ioctl((int)s, (unsigned long)dwIoControlCode,
                          lpvInBuffer != NULL ? lpvInBuffer : lpvOutBuffer);
            if (result < 0) {
                set_wsa_error_from_errno();
                return SOCKET_ERROR;
            }
            if (lpcbBytesReturned != NULL) {
                *lpcbBytesReturned = sizeof(unsigned long);
            }
            break;

        case SIO_GET_EXTENSION_FUNCTION_POINTER:
            /* Handle extension function pointer requests */
            if (lpvInBuffer == NULL || cbInBuffer < sizeof(GUID)) {
                g_wsa_last_error = WSAEFAULT;
                return SOCKET_ERROR;
            }
            if (lpvOutBuffer == NULL || cbOutBuffer < sizeof(void*)) {
                g_wsa_last_error = WSAEFAULT;
                return SOCKET_ERROR;
            }
            /* Extension functions are implemented directly */
            if (lpcbBytesReturned != NULL) {
                *lpcbBytesReturned = sizeof(void*);
            }
            g_wsa_last_error = WSAEOPNOTSUPP;
            return SOCKET_ERROR;

        default:
            g_wsa_last_error = WSAEINVAL;
            return SOCKET_ERROR;
    }

    g_wsa_last_error = 0;
    return 0;
}

/* ============================================================================
 * Byte Order Conversion Functions
 * ============================================================================ */

int WSAAPI WSAHtonl(SOCKET s, unsigned long hostlong, unsigned long* lpnetlong)
{
    (void)s;

    if (lpnetlong == NULL) {
        g_wsa_last_error = WSAEFAULT;
        return SOCKET_ERROR;
    }

    *lpnetlong = htonl(hostlong);
    g_wsa_last_error = 0;
    return 0;
}

int WSAAPI WSAHtons(SOCKET s, unsigned short hostshort, unsigned short* lpnetshort)
{
    (void)s;

    if (lpnetshort == NULL) {
        g_wsa_last_error = WSAEFAULT;
        return SOCKET_ERROR;
    }

    *lpnetshort = htons(hostshort);
    g_wsa_last_error = 0;
    return 0;
}

int WSAAPI WSANtohl(SOCKET s, unsigned long netlong, unsigned long* lphostlong)
{
    (void)s;

    if (lphostlong == NULL) {
        g_wsa_last_error = WSAEFAULT;
        return SOCKET_ERROR;
    }

    *lphostlong = ntohl(netlong);
    g_wsa_last_error = 0;
    return 0;
}

int WSAAPI WSANtohs(SOCKET s, unsigned short netshort, unsigned short* lphostshort)
{
    (void)s;

    if (lphostshort == NULL) {
        g_wsa_last_error = WSAEFAULT;
        return SOCKET_ERROR;
    }

    *lphostshort = ntohs(netshort);
    g_wsa_last_error = 0;
    return 0;
}

/* ============================================================================
 * Socket Pair (useful utility)
 * ============================================================================ */

int WSAAPI WSASocketPair(int af, int type, int protocol, SOCKET socks[2])
{
    int result;
    int linux_socks[2];

    result = socketpair(af, type, protocol, linux_socks);

    if (result < 0) {
        set_wsa_error_from_errno();
        return SOCKET_ERROR;
    }

    socks[0] = (SOCKET)linux_socks[0];
    socks[1] = (SOCKET)linux_socks[1];

    g_wsa_last_error = 0;
    return 0;
}

/* Helper function implementations */
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

#endif /* __linux__ */
