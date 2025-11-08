#ifndef _MSWSOCK_H
#define _MSWSOCK_H

#include "winsock2.h"

#ifdef __linux__

#ifdef __cplusplus
extern "C" {
#endif

/* GUIDs for extension functions */
#define WSAID_ACCEPTEX \
    {0xb5367df1,0xcbac,0x11cf,{0x95,0xca,0x00,0x80,0x5f,0x48,0xa1,0x92}}
#define WSAID_CONNECTEX \
    {0x25a207b9,0xddf3,0x4660,{0x8e,0xe9,0x76,0xe5,0x8c,0x74,0x06,0x3e}}
#define WSAID_DISCONNECTEX \
    {0x7fda2e11,0x8630,0x436f,{0xa0,0x31,0xf5,0x36,0xa6,0xee,0xc1,0x57}}
#define WSAID_GETACCEPTEXSOCKADDRS \
    {0xb5367df2,0xcbac,0x11cf,{0x95,0xca,0x00,0x80,0x5f,0x48,0xa1,0x92}}
#define WSAID_TRANSMITFILE \
    {0xb5367df0,0xcbac,0x11cf,{0x95,0xca,0x00,0x80,0x5f,0x48,0xa1,0x92}}
#define WSAID_TRANSMITPACKETS \
    {0xd9689da0,0x1f90,0x11d3,{0x99,0x71,0x00,0xc0,0x4f,0x68,0xc8,0x76}}
#define WSAID_WSARECVMSG \
    {0xf689d7c8,0x6f1f,0x436b,{0x8a,0x53,0xe5,0x4f,0xe3,0x51,0xc3,0x22}}
#define WSAID_WSASENDMSG \
    {0xa441e712,0x754f,0x43ca,{0x84,0xa7,0x0d,0xee,0x44,0xcf,0x60,0x6d}}

/* TransmitFile flags */
#define TF_DISCONNECT       0x01
#define TF_REUSE_SOCKET     0x02
#define TF_WRITE_BEHIND     0x04
#define TF_USE_DEFAULT_WORKER 0x00
#define TF_USE_SYSTEM_THREAD  0x10
#define TF_USE_KERNEL_APC     0x20

/* TransmitPackets flags */
#define TP_ELEMENT_MEMORY   1
#define TP_ELEMENT_FILE     2
#define TP_ELEMENT_EOP      4

/* Socket types for SO_PROTOCOL_INFO */
#define SO_OPENTYPE             0x7008
#define SO_SYNCHRONOUS_ALERT    0x10
#define SO_SYNCHRONOUS_NONALERT 0x20

/* TransmitFile structure */
typedef struct _TRANSMIT_FILE_BUFFERS {
    PVOID Head;
    DWORD HeadLength;
    PVOID Tail;
    DWORD TailLength;
} TRANSMIT_FILE_BUFFERS, *PTRANSMIT_FILE_BUFFERS, *LPTRANSMIT_FILE_BUFFERS;

/* TransmitPackets element */
typedef struct _TRANSMIT_PACKETS_ELEMENT {
    DWORD dwElFlags;
    DWORD cLength;
    union {
        struct {
            LARGE_INTEGER nFileOffset;
            HANDLE hFile;
        };
        PVOID pBuffer;
    };
} TRANSMIT_PACKETS_ELEMENT, *PTRANSMIT_PACKETS_ELEMENT, *LPTRANSMIT_PACKETS_ELEMENT;

typedef LARGE_INTEGER LONGLONG;

/* WSABUF for control data */
typedef struct _WSACMSGHDR {
    size_t cmsg_len;
    int cmsg_level;
    int cmsg_type;
} WSACMSGHDR, *PWSACMSGHDR, *LPWSACMSGHDR;

/* RIO (Registered I/O) structures */
typedef struct _RIO_BUF {
    DWORD BufferId;
    DWORD Offset;
    DWORD Length;
} RIO_BUF, *PRIO_BUF;

typedef DWORD RIO_BUFFERID, *PRIO_BUFFERID;
typedef DWORD RIO_CQ, *PRIO_CQ;
typedef DWORD RIO_RQ, *PRIO_RQ;

typedef enum _RIO_NOTIFICATION_COMPLETION_TYPE {
    RIO_EVENT_COMPLETION = 1,
    RIO_IOCP_COMPLETION = 2
} RIO_NOTIFICATION_COMPLETION_TYPE, *PRIO_NOTIFICATION_COMPLETION_TYPE;

typedef struct _RIO_NOTIFICATION_COMPLETION {
    RIO_NOTIFICATION_COMPLETION_TYPE Type;
    union {
        struct {
            HANDLE EventHandle;
            BOOL NotifyReset;
        } Event;
        struct {
            HANDLE IocpHandle;
            PVOID CompletionKey;
            LPOVERLAPPED Overlapped;
        } Iocp;
    };
} RIO_NOTIFICATION_COMPLETION, *PRIO_NOTIFICATION_COMPLETION;

/* Function pointer types for extension functions */
typedef BOOL (WINAPI *LPFN_ACCEPTEX)(
    SOCKET sListenSocket,
    SOCKET sAcceptSocket,
    PVOID lpOutputBuffer,
    DWORD dwReceiveDataLength,
    DWORD dwLocalAddressLength,
    DWORD dwRemoteAddressLength,
    DWORD* lpdwBytesReceived,
    LPOVERLAPPED lpOverlapped
);

typedef BOOL (WINAPI *LPFN_CONNECTEX)(
    SOCKET s,
    const struct sockaddr* name,
    int namelen,
    PVOID lpSendBuffer,
    DWORD dwSendDataLength,
    DWORD* lpdwBytesSent,
    LPOVERLAPPED lpOverlapped
);

typedef BOOL (WINAPI *LPFN_DISCONNECTEX)(
    SOCKET s,
    LPOVERLAPPED lpOverlapped,
    DWORD dwFlags,
    DWORD dwReserved
);

typedef void (WINAPI *LPFN_GETACCEPTEXSOCKADDRS)(
    PVOID lpOutputBuffer,
    DWORD dwReceiveDataLength,
    DWORD dwLocalAddressLength,
    DWORD dwRemoteAddressLength,
    struct sockaddr** LocalSockaddr,
    int* LocalSockaddrLength,
    struct sockaddr** RemoteSockaddr,
    int* RemoteSockaddrLength
);

typedef BOOL (WINAPI *LPFN_TRANSMITFILE)(
    SOCKET hSocket,
    HANDLE hFile,
    DWORD nNumberOfBytesToWrite,
    DWORD nNumberOfBytesPerSend,
    LPOVERLAPPED lpOverlapped,
    LPTRANSMIT_FILE_BUFFERS lpTransmitBuffers,
    DWORD dwReserved
);

typedef BOOL (WINAPI *LPFN_TRANSMITPACKETS)(
    SOCKET hSocket,
    LPTRANSMIT_PACKETS_ELEMENT lpPacketArray,
    DWORD nElementCount,
    DWORD nSendSize,
    LPOVERLAPPED lpOverlapped,
    DWORD dwFlags
);

typedef INT (WINAPI *LPFN_WSARECVMSG)(
    SOCKET s,
    LPWSAMSG lpMsg,
    DWORD* lpdwNumberOfBytesRecvd,
    LPWSAOVERLAPPED lpOverlapped,
    LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
);

typedef INT (WINAPI *LPFN_WSASENDMSG)(
    SOCKET s,
    LPWSAMSG lpMsg,
    DWORD dwFlags,
    DWORD* lpNumberOfBytesSent,
    LPWSAOVERLAPPED lpOverlapped,
    LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
);

/* Direct function declarations (non-function pointer versions) */
BOOL WINAPI AcceptEx(
    SOCKET sListenSocket,
    SOCKET sAcceptSocket,
    PVOID lpOutputBuffer,
    DWORD dwReceiveDataLength,
    DWORD dwLocalAddressLength,
    DWORD dwRemoteAddressLength,
    DWORD* lpdwBytesReceived,
    LPOVERLAPPED lpOverlapped
);

void WINAPI GetAcceptExSockaddrs(
    PVOID lpOutputBuffer,
    DWORD dwReceiveDataLength,
    DWORD dwLocalAddressLength,
    DWORD dwRemoteAddressLength,
    struct sockaddr** LocalSockaddr,
    int* LocalSockaddrLength,
    struct sockaddr** RemoteSockaddr,
    int* RemoteSockaddrLength
);

BOOL WINAPI TransmitFile(
    SOCKET hSocket,
    HANDLE hFile,
    DWORD nNumberOfBytesToWrite,
    DWORD nNumberOfBytesPerSend,
    LPOVERLAPPED lpOverlapped,
    LPTRANSMIT_FILE_BUFFERS lpTransmitBuffers,
    DWORD dwReserved
);

BOOL WINAPI ConnectEx(
    SOCKET s,
    const struct sockaddr* name,
    int namelen,
    PVOID lpSendBuffer,
    DWORD dwSendDataLength,
    DWORD* lpdwBytesSent,
    LPOVERLAPPED lpOverlapped
);

BOOL WINAPI DisconnectEx(
    SOCKET s,
    LPOVERLAPPED lpOverlapped,
    DWORD dwFlags,
    DWORD dwReserved
);

BOOL WINAPI TransmitPackets(
    SOCKET hSocket,
    LPTRANSMIT_PACKETS_ELEMENT lpPacketArray,
    DWORD nElementCount,
    DWORD nSendSize,
    LPOVERLAPPED lpOverlapped,
    DWORD dwFlags
);

/* Helper function to retrieve extension function pointers */
int WSAAPI WSAGetExtensionFunctionPointer(
    SOCKET s,
    const GUID* lpGuid,
    void** lpfnFunction
);

/* Additional Mswsock-specific structures */
typedef struct _WSATHREADID {
    HANDLE ThreadHandle;
    DWORD_PTR Reserved;
} WSATHREADID, *PWSATHREADID, *LPWSATHREADID;

/* Completion port types */
typedef HANDLE (WINAPI *LPFN_CREATEIOCOMPLETIONPORT)(
    HANDLE FileHandle,
    HANDLE ExistingCompletionPort,
    ULONG_PTR CompletionKey,
    DWORD NumberOfConcurrentThreads
);

typedef BOOL (WINAPI *LPFN_GETQUEUEDCOMPLETIONSTATUS)(
    HANDLE CompletionPort,
    DWORD* lpNumberOfBytesTransferred,
    ULONG_PTR* lpCompletionKey,
    LPOVERLAPPED* lpOverlapped,
    DWORD dwMilliseconds
);

typedef BOOL (WINAPI *LPFN_POSTQUEUEDCOMPLETIONSTATUS)(
    HANDLE CompletionPort,
    DWORD dwNumberOfBytesTransferred,
    ULONG_PTR dwCompletionKey,
    LPOVERLAPPED lpOverlapped
);

/* Completion port functions */
HANDLE WINAPI CreateIoCompletionPort(
    HANDLE FileHandle,
    HANDLE ExistingCompletionPort,
    ULONG_PTR CompletionKey,
    DWORD NumberOfConcurrentThreads
);

BOOL WINAPI GetQueuedCompletionStatus(
    HANDLE CompletionPort,
    DWORD* lpNumberOfBytesTransferred,
    ULONG_PTR* lpCompletionKey,
    LPOVERLAPPED* lpOverlapped,
    DWORD dwMilliseconds
);

BOOL WINAPI PostQueuedCompletionStatus(
    HANDLE CompletionPort,
    DWORD dwNumberOfBytesTransferred,
    ULONG_PTR dwCompletionKey,
    LPOVERLAPPED lpOverlapped
);

BOOL WINAPI GetQueuedCompletionStatusEx(
    HANDLE CompletionPort,
    LPOVERLAPPED* lpCompletionPortEntries,
    DWORD ulCount,
    DWORD* ulNumEntriesRemoved,
    DWORD dwMilliseconds,
    BOOL fAlertable
);

/* Service provider interface */
#define NSP_NOTIFY_IMMEDIATELY   0x00000001
#define NSP_NOTIFY_HWND          0x00000002
#define NSP_NOTIFY_EVENT         0x00000004
#define NSP_NOTIFY_PORT          0x00000008
#define NSP_NOTIFY_APC           0x00000010

/* Service install flags */
#define SERVICE_MULTIPLE         0x00000001

/* Name space provider info */
#define NSPROTO_IPX      1000
#define NSPROTO_SPX      1256
#define NSPROTO_SPXII    1257

/* Address family types for name space providers */
#define AF_12844    AF_12844
#define AF_ATM      AF_ATM
#define AF_NETBIOS  AF_NETBIOS

#ifdef __cplusplus
}
#endif

#endif /* __linux__ */

#endif /* _MSWSOCK_H */
