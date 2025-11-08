/*
 * Microsoft-Specific Extension Functions
 * Implements AcceptEx, TransmitFile, ConnectEx, DisconnectEx, etc.
 */

#ifdef __linux__

#include "winsock2.h"
#include "ws2tcpip.h"
#include "mswsock.h"
#include <sys/sendfile.h>
#include <sys/stat.h>

extern __thread int g_wsa_last_error;

/* ============================================================================
 * AcceptEx Implementation
 * ============================================================================ */

BOOL WINAPI AcceptEx(
    SOCKET sListenSocket,
    SOCKET sAcceptSocket,
    PVOID lpOutputBuffer,
    DWORD dwReceiveDataLength,
    DWORD dwLocalAddressLength,
    DWORD dwRemoteAddressLength,
    DWORD* lpdwBytesReceived,
    LPOVERLAPPED lpOverlapped)
{
    struct sockaddr_storage addr;
    socklen_t addrlen;
    int result;
    ssize_t recv_result;

    (void)lpOverlapped; /* Overlapped I/O not fully supported */
    (void)dwLocalAddressLength;
    (void)dwRemoteAddressLength;

    /* First, we need to accept the connection on sAcceptSocket */
    /* Since sAcceptSocket should already be created, we'll use dup2 to replace it */

    addrlen = sizeof(addr);
    result = accept((int)sListenSocket, (struct sockaddr*)&addr, &addrlen);

    if (result < 0) {
        g_wsa_last_error = WSAECONNREFUSED;
        return FALSE;
    }

    /* Duplicate the accepted socket to sAcceptSocket */
    if (dup2(result, (int)sAcceptSocket) < 0) {
        close(result);
        g_wsa_last_error = WSAEINVAL;
        return FALSE;
    }

    close(result);

    /* If dwReceiveDataLength > 0, receive initial data */
    if (dwReceiveDataLength > 0 && lpOutputBuffer != NULL) {
        recv_result = recv((int)sAcceptSocket,
                          (char*)lpOutputBuffer + dwLocalAddressLength + dwRemoteAddressLength,
                          dwReceiveDataLength, 0);

        if (recv_result < 0) {
            g_wsa_last_error = WSAECONNRESET;
            return FALSE;
        }

        if (lpdwBytesReceived != NULL) {
            *lpdwBytesReceived = (DWORD)recv_result;
        }
    } else {
        if (lpdwBytesReceived != NULL) {
            *lpdwBytesReceived = 0;
        }
    }

    /* Store addresses in output buffer */
    if (lpOutputBuffer != NULL) {
        struct sockaddr_storage local_addr;
        socklen_t local_addrlen;

        local_addrlen = sizeof(local_addr);
        getsockname((int)sAcceptSocket, (struct sockaddr*)&local_addr, &local_addrlen);

        memcpy(lpOutputBuffer, &local_addr, dwLocalAddressLength);
        memcpy((char*)lpOutputBuffer + dwLocalAddressLength, &addr, dwRemoteAddressLength);
    }

    g_wsa_last_error = 0;
    return TRUE;
}

/* ============================================================================
 * GetAcceptExSockaddrs Implementation
 * ============================================================================ */

void WINAPI GetAcceptExSockaddrs(
    PVOID lpOutputBuffer,
    DWORD dwReceiveDataLength,
    DWORD dwLocalAddressLength,
    DWORD dwRemoteAddressLength,
    struct sockaddr** LocalSockaddr,
    int* LocalSockaddrLength,
    struct sockaddr** RemoteSockaddr,
    int* RemoteSockaddrLength)
{
    char* buffer;

    (void)dwReceiveDataLength;

    buffer = (char*)lpOutputBuffer;

    if (LocalSockaddr != NULL && LocalSockaddrLength != NULL) {
        *LocalSockaddr = (struct sockaddr*)buffer;
        *LocalSockaddrLength = (int)dwLocalAddressLength;
    }

    if (RemoteSockaddr != NULL && RemoteSockaddrLength != NULL) {
        *RemoteSockaddr = (struct sockaddr*)(buffer + dwLocalAddressLength);
        *RemoteSockaddrLength = (int)dwRemoteAddressLength;
    }
}

/* ============================================================================
 * TransmitFile Implementation
 * ============================================================================ */

BOOL WINAPI TransmitFile(
    SOCKET hSocket,
    HANDLE hFile,
    DWORD nNumberOfBytesToWrite,
    DWORD nNumberOfBytesPerSend,
    LPOVERLAPPED lpOverlapped,
    LPTRANSMIT_FILE_BUFFERS lpTransmitBuffers,
    DWORD dwReserved)
{
    off_t offset;
    ssize_t sent;
    int fd;

    (void)nNumberOfBytesPerSend;
    (void)lpOverlapped;
    (void)dwReserved;

    fd = (int)(intptr_t)hFile;

    /* Send head buffer if provided */
    if (lpTransmitBuffers != NULL && lpTransmitBuffers->Head != NULL &&
        lpTransmitBuffers->HeadLength > 0) {
        sent = send((int)hSocket, lpTransmitBuffers->Head,
                   lpTransmitBuffers->HeadLength, 0);
        if (sent < 0) {
            g_wsa_last_error = WSAECONNRESET;
            return FALSE;
        }
    }

    /* Send file data */
    if (nNumberOfBytesToWrite > 0) {
        offset = 0;
        sent = sendfile((int)hSocket, fd, &offset, nNumberOfBytesToWrite);
        if (sent < 0) {
            g_wsa_last_error = WSAECONNRESET;
            return FALSE;
        }
    }

    /* Send tail buffer if provided */
    if (lpTransmitBuffers != NULL && lpTransmitBuffers->Tail != NULL &&
        lpTransmitBuffers->TailLength > 0) {
        sent = send((int)hSocket, lpTransmitBuffers->Tail,
                   lpTransmitBuffers->TailLength, 0);
        if (sent < 0) {
            g_wsa_last_error = WSAECONNRESET;
            return FALSE;
        }
    }

    g_wsa_last_error = 0;
    return TRUE;
}

/* ============================================================================
 * ConnectEx Implementation
 * ============================================================================ */

BOOL WINAPI ConnectEx(
    SOCKET s,
    const struct sockaddr* name,
    int namelen,
    PVOID lpSendBuffer,
    DWORD dwSendDataLength,
    DWORD* lpdwBytesSent,
    LPOVERLAPPED lpOverlapped)
{
    int result;
    ssize_t sent;

    (void)lpOverlapped;

    /* Perform connection */
    result = connect((int)s, name, (socklen_t)namelen);

    if (result < 0) {
        if (errno == EINPROGRESS) {
            g_wsa_last_error = WSA_IO_PENDING;
            return FALSE;
        }
        g_wsa_last_error = WSAECONNREFUSED;
        return FALSE;
    }

    /* Send initial data if provided */
    if (lpSendBuffer != NULL && dwSendDataLength > 0) {
        sent = send((int)s, lpSendBuffer, dwSendDataLength, 0);
        if (sent < 0) {
            g_wsa_last_error = WSAECONNRESET;
            return FALSE;
        }
        if (lpdwBytesSent != NULL) {
            *lpdwBytesSent = (DWORD)sent;
        }
    } else {
        if (lpdwBytesSent != NULL) {
            *lpdwBytesSent = 0;
        }
    }

    g_wsa_last_error = 0;
    return TRUE;
}

/* ============================================================================
 * DisconnectEx Implementation
 * ============================================================================ */

BOOL WINAPI DisconnectEx(
    SOCKET s,
    LPOVERLAPPED lpOverlapped,
    DWORD dwFlags,
    DWORD dwReserved)
{
    int result;

    (void)lpOverlapped;
    (void)dwReserved;

    /* Shutdown the connection */
    result = shutdown((int)s, SHUT_RDWR);

    if (result < 0) {
        g_wsa_last_error = WSAENOTCONN;
        return FALSE;
    }

    /* If TF_REUSE_SOCKET flag is not set, close the socket */
    if (!(dwFlags & TF_REUSE_SOCKET)) {
        close((int)s);
    }

    g_wsa_last_error = 0;
    return TRUE;
}

/* ============================================================================
 * TransmitPackets Implementation
 * ============================================================================ */

BOOL WINAPI TransmitPackets(
    SOCKET hSocket,
    LPTRANSMIT_PACKETS_ELEMENT lpPacketArray,
    DWORD nElementCount,
    DWORD nSendSize,
    LPOVERLAPPED lpOverlapped,
    DWORD dwFlags)
{
    DWORD i;
    ssize_t sent;

    (void)nSendSize;
    (void)lpOverlapped;
    (void)dwFlags;

    if (lpPacketArray == NULL || nElementCount == 0) {
        g_wsa_last_error = WSAEINVAL;
        return FALSE;
    }

    for (i = 0; i < nElementCount; i++) {
        if (lpPacketArray[i].dwElFlags & TP_ELEMENT_MEMORY) {
            /* Send memory buffer */
            sent = send((int)hSocket, lpPacketArray[i].pBuffer,
                       lpPacketArray[i].cLength, 0);
            if (sent < 0) {
                g_wsa_last_error = WSAECONNRESET;
                return FALSE;
            }
        } else if (lpPacketArray[i].dwElFlags & TP_ELEMENT_FILE) {
            /* Send file data */
            off_t offset;
            int fd;

            fd = (int)(intptr_t)lpPacketArray[i].hFile;
            offset = lpPacketArray[i].nFileOffset.QuadPart;

            sent = sendfile((int)hSocket, fd, &offset, lpPacketArray[i].cLength);
            if (sent < 0) {
                g_wsa_last_error = WSAECONNRESET;
                return FALSE;
            }
        }
    }

    g_wsa_last_error = 0;
    return TRUE;
}

/* ============================================================================
 * I/O Completion Port Functions (Stubs)
 * ============================================================================ */

HANDLE WINAPI CreateIoCompletionPort(
    HANDLE FileHandle,
    HANDLE ExistingCompletionPort,
    ULONG_PTR CompletionKey,
    DWORD NumberOfConcurrentThreads)
{
    (void)FileHandle;
    (void)ExistingCompletionPort;
    (void)CompletionKey;
    (void)NumberOfConcurrentThreads;

    /* I/O Completion Ports not supported on Linux */
    /* Return a dummy handle to prevent crashes */
    g_wsa_last_error = 0;
    return (HANDLE)(intptr_t)1;
}

BOOL WINAPI GetQueuedCompletionStatus(
    HANDLE CompletionPort,
    DWORD* lpNumberOfBytesTransferred,
    ULONG_PTR* lpCompletionKey,
    LPOVERLAPPED* lpOverlapped,
    DWORD dwMilliseconds)
{
    (void)CompletionPort;
    (void)lpNumberOfBytesTransferred;
    (void)lpCompletionKey;
    (void)lpOverlapped;
    (void)dwMilliseconds;

    /* Not supported */
    g_wsa_last_error = WSAEOPNOTSUPP;
    return FALSE;
}

BOOL WINAPI PostQueuedCompletionStatus(
    HANDLE CompletionPort,
    DWORD dwNumberOfBytesTransferred,
    ULONG_PTR dwCompletionKey,
    LPOVERLAPPED lpOverlapped)
{
    (void)CompletionPort;
    (void)dwNumberOfBytesTransferred;
    (void)dwCompletionKey;
    (void)lpOverlapped;

    /* Not supported */
    g_wsa_last_error = WSAEOPNOTSUPP;
    return FALSE;
}

BOOL WINAPI GetQueuedCompletionStatusEx(
    HANDLE CompletionPort,
    LPOVERLAPPED* lpCompletionPortEntries,
    DWORD ulCount,
    DWORD* ulNumEntriesRemoved,
    DWORD dwMilliseconds,
    BOOL fAlertable)
{
    (void)CompletionPort;
    (void)lpCompletionPortEntries;
    (void)ulCount;
    (void)ulNumEntriesRemoved;
    (void)dwMilliseconds;
    (void)fAlertable;

    /* Not supported */
    g_wsa_last_error = WSAEOPNOTSUPP;
    return FALSE;
}

/* ============================================================================
 * WSAGetExtensionFunctionPointer Implementation
 * ============================================================================ */

int WSAAPI WSAGetExtensionFunctionPointer(
    SOCKET s,
    const GUID* lpGuid,
    void** lpfnFunction)
{
    (void)s;

    if (lpGuid == NULL || lpfnFunction == NULL) {
        g_wsa_last_error = WSAEFAULT;
        return SOCKET_ERROR;
    }

    /* Return pointers to our extension functions */
    /* Compare GUIDs and return appropriate function pointers */

    /* For simplicity, we'll return function pointers directly */
    /* In a full implementation, you would compare lpGuid with known GUIDs */

    *lpfnFunction = NULL;
    g_wsa_last_error = WSAEINVAL;
    return SOCKET_ERROR;
}

/* ============================================================================
 * WSAGetOverlappedResult Implementation
 * ============================================================================ */

BOOL WSAAPI WSAGetOverlappedResult(SOCKET s, LPWSAOVERLAPPED lpOverlapped,
                                   DWORD* lpcbTransfer, BOOL fWait,
                                   DWORD* lpdwFlags)
{
    (void)s;
    (void)lpOverlapped;
    (void)lpcbTransfer;
    (void)fWait;
    (void)lpdwFlags;

    /* Overlapped I/O not fully supported */
    g_wsa_last_error = WSAEOPNOTSUPP;
    return FALSE;
}

#endif /* __linux__ */
