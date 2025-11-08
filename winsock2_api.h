/*
 * Winsock2 API for Linux
 * Windows Socket API 2.2 compatible header for Linux
 */

#ifndef _WINSOCK2_API_H
#define _WINSOCK2_API_H

#ifdef __linux__

/* Include system headers first */
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <poll.h>
#include <string.h>
#include <stdlib.h>

#include "windows_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Thread-local storage for WSA last error */
extern __thread int g_wsa_last_error;

/* ============================================================================
 * Socket Types and Constants
 * ============================================================================ */

/* Socket type - on Linux this is just an int */
typedef int SOCKET;

/* Special socket values */
#define INVALID_SOCKET  (-1)
#define SOCKET_ERROR    (-1)

/* Winsock version macros */
#define MAKEWORD(a,b) ((WORD)(((BYTE)(a))|(((WORD)((BYTE)(b)))<<8)))
#define LOBYTE(w) ((BYTE)(w))
#define HIBYTE(w) ((BYTE)(((WORD)(w)>>8)&0xFF))

#define WINSOCK_VERSION MAKEWORD(2,2)

/* Shutdown options - map to Linux equivalents */
#ifndef SD_RECEIVE
#define SD_RECEIVE      SHUT_RD
#define SD_SEND         SHUT_WR
#define SD_BOTH         SHUT_RDWR
#endif

/* ============================================================================
 * Error Codes
 * ============================================================================ */

#define WSABASEERR              10000
#define WSAEINTR                (WSABASEERR+4)
#define WSAEBADF                (WSABASEERR+9)
#define WSAEACCES               (WSABASEERR+13)
#define WSAEFAULT               (WSABASEERR+14)
#define WSAEINVAL               (WSABASEERR+22)
#define WSAEMFILE               (WSABASEERR+24)
#define WSAEWOULDBLOCK          (WSABASEERR+35)
#define WSAEINPROGRESS          (WSABASEERR+36)
#define WSAEALREADY             (WSABASEERR+37)
#define WSAENOTSOCK             (WSABASEERR+38)
#define WSAEDESTADDRREQ         (WSABASEERR+39)
#define WSAEMSGSIZE             (WSABASEERR+40)
#define WSAEPROTOTYPE           (WSABASEERR+41)
#define WSAENOPROTOOPT          (WSABASEERR+42)
#define WSAEPROTONOSUPPORT      (WSABASEERR+43)
#define WSAESOCKTNOSUPPORT      (WSABASEERR+44)
#define WSAEOPNOTSUPP           (WSABASEERR+45)
#define WSAEPFNOSUPPORT         (WSABASEERR+46)
#define WSAEAFNOSUPPORT         (WSABASEERR+47)
#define WSAEADDRINUSE           (WSABASEERR+48)
#define WSAEADDRNOTAVAIL        (WSABASEERR+49)
#define WSAENETDOWN             (WSABASEERR+50)
#define WSAENETUNREACH          (WSABASEERR+51)
#define WSAENETRESET            (WSABASEERR+52)
#define WSAECONNABORTED         (WSABASEERR+53)
#define WSAECONNRESET           (WSABASEERR+54)
#define WSAENOBUFS              (WSABASEERR+55)
#define WSAEISCONN              (WSABASEERR+56)
#define WSAENOTCONN             (WSABASEERR+57)
#define WSAESHUTDOWN            (WSABASEERR+58)
#define WSAETOOMANYREFS         (WSABASEERR+59)
#define WSAETIMEDOUT            (WSABASEERR+60)
#define WSAECONNREFUSED         (WSABASEERR+61)
#define WSAELOOP                (WSABASEERR+62)
#define WSAENAMETOOLONG         (WSABASEERR+63)
#define WSAEHOSTDOWN            (WSABASEERR+64)
#define WSAEHOSTUNREACH         (WSABASEERR+65)
#define WSAENOTEMPTY            (WSABASEERR+66)
#define WSAEPROCLIM             (WSABASEERR+67)
#define WSAEUSERS               (WSABASEERR+68)
#define WSAEDQUOT               (WSABASEERR+69)
#define WSAESTALE               (WSABASEERR+70)
#define WSAEREMOTE              (WSABASEERR+71)
#define WSASYSNOTREADY          (WSABASEERR+91)
#define WSAVERNOTSUPPORTED      (WSABASEERR+92)
#define WSANOTINITIALISED       (WSABASEERR+93)
#define WSAEDISCON              (WSABASEERR+101)
#define WSAENOMORE              (WSABASEERR+102)
#define WSAECANCELLED           (WSABASEERR+103)
#define WSAEINVALIDPROCTABLE    (WSABASEERR+104)
#define WSAEINVALIDPROVIDER     (WSABASEERR+105)
#define WSAEPROVIDERFAILEDINIT  (WSABASEERR+106)
#define WSASYSCALLFAILURE       (WSABASEERR+107)
#define WSASERVICE_NOT_FOUND    (WSABASEERR+108)
#define WSATYPE_NOT_FOUND       (WSABASEERR+109)
#define WSA_E_NO_MORE           (WSABASEERR+110)
#define WSA_E_CANCELLED         (WSABASEERR+111)
#define WSAEREFUSED             (WSABASEERR+112)
#define WSAHOST_NOT_FOUND       (WSABASEERR+1001)
#define WSATRY_AGAIN            (WSABASEERR+1002)
#define WSANO_RECOVERY          (WSABASEERR+1003)
#define WSANO_DATA              (WSABASEERR+1004)
#define WSANO_ADDRESS           WSANO_DATA

/* h_errno equivalents */
#define HOST_NOT_FOUND          WSAHOST_NOT_FOUND
#define TRY_AGAIN               WSATRY_AGAIN
#define NO_RECOVERY             WSANO_RECOVERY
#define NO_DATA                 WSANO_DATA
#define NO_ADDRESS              WSANO_ADDRESS

/* Additional WSA error codes */
#define WSA_NOT_ENOUGH_MEMORY   8
#define WSA_INVALID_HANDLE      6
#define WSA_INVALID_PARAMETER   87
#define WSA_IO_PENDING          997
#define WSA_IO_INCOMPLETE       996
#define WSA_OPERATION_ABORTED   995

/* ============================================================================
 * IOCTL Constants
 * ============================================================================ */

#define IOC_WS2 0x08000000
#define _WSAIO(x,y)   ((x)|(y))
#define _WSAIOR(x,y)  (IOC_OUT|(x)|(y))
#define _WSAIOW(x,y)  (IOC_IN|(x)|(y))
#define _WSAIORW(x,y) (IOC_INOUT|(x)|(y))

#define SIO_ASSOCIATE_HANDLE                _WSAIOW(IOC_WS2,1)
#define SIO_ENABLE_CIRCULAR_QUEUEING        _WSAIO(IOC_WS2,2)
#define SIO_FIND_ROUTE                      _WSAIOR(IOC_WS2,3)
#define SIO_FLUSH                           _WSAIO(IOC_WS2,4)
#define SIO_GET_BROADCAST_ADDRESS           _WSAIOR(IOC_WS2,5)
#define SIO_GET_EXTENSION_FUNCTION_POINTER  _WSAIORW(IOC_WS2,6)
#define SIO_GET_QOS                         _WSAIORW(IOC_WS2,7)
#define SIO_GET_GROUP_QOS                   _WSAIORW(IOC_WS2,8)
#define SIO_MULTIPOINT_LOOPBACK             _WSAIOW(IOC_WS2,9)
#define SIO_MULTICAST_SCOPE                 _WSAIOW(IOC_WS2,10)
#define SIO_SET_QOS                         _WSAIOW(IOC_WS2,11)
#define SIO_SET_GROUP_QOS                   _WSAIOW(IOC_WS2,12)
#define SIO_TRANSLATE_HANDLE                _WSAIORW(IOC_WS2,13)
#define SIO_ROUTING_INTERFACE_QUERY         _WSAIORW(IOC_WS2,20)
#define SIO_ROUTING_INTERFACE_CHANGE        _WSAIOW(IOC_WS2,21)
#define SIO_ADDRESS_LIST_QUERY              _WSAIOR(IOC_WS2,22)
#define SIO_ADDRESS_LIST_CHANGE             _WSAIO(IOC_WS2,23)
#define SIO_QUERY_TARGET_PNP_HANDLE         _WSAIOR(IOC_WS2,24)
#define SIO_ADDRESS_LIST_SORT               _WSAIORW(IOC_WS2,25)

/* ============================================================================
 * WSADATA Structure
 * ============================================================================ */

typedef struct WSAData {
    WORD wVersion;
    WORD wHighVersion;
    unsigned short iMaxSockets;
    unsigned short iMaxUdpDg;
    char* lpVendorInfo;
    char szDescription[257];
    char szSystemStatus[129];
} WSADATA, *LPWSADATA;

/* ============================================================================
 * Socket Address Typedefs (use system structures)
 * ============================================================================ */

typedef struct sockaddr SOCKADDR;
typedef struct sockaddr* PSOCKADDR;
typedef struct sockaddr* LPSOCKADDR;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr_in* PSOCKADDR_IN;
typedef struct sockaddr_in* LPSOCKADDR_IN;

/* ============================================================================
 * WSABUF Structure
 * ============================================================================ */

typedef struct _WSABUF {
    unsigned long len;
    char* buf;
} WSABUF, *LPWSABUF;

/* ============================================================================
 * WSAMSG Structure
 * ============================================================================ */

typedef struct _WSAMSG {
    LPSOCKADDR name;
    INT namelen;
    LPWSABUF lpBuffers;
    DWORD dwBufferCount;
    WSABUF Control;
    DWORD dwFlags;
} WSAMSG, *PWSAMSG, *LPWSAMSG;

/* ============================================================================
 * Overlapped Structures
 * ============================================================================ */

typedef struct _OVERLAPPED {
    ULONG_PTR Internal;
    ULONG_PTR InternalHigh;
    union {
        struct {
            DWORD Offset;
            DWORD OffsetHigh;
        };
        PVOID Pointer;
    };
    HANDLE hEvent;
} OVERLAPPED, *LPOVERLAPPED;

typedef HANDLE WSAEVENT;
typedef OVERLAPPED WSAOVERLAPPED;
typedef LPOVERLAPPED LPWSAOVERLAPPED;

/* Completion routine */
typedef void (WSAAPI *LPWSAOVERLAPPED_COMPLETION_ROUTINE)(
    DWORD dwError,
    DWORD cbTransferred,
    LPWSAOVERLAPPED lpOverlapped,
    DWORD dwFlags
);

/* ============================================================================
 * WSANETWORKEVENTS Structure
 * ============================================================================ */

#define FD_READ         0x01
#define FD_WRITE        0x02
#define FD_OOB          0x04
#define FD_ACCEPT       0x08
#define FD_CONNECT      0x10
#define FD_CLOSE        0x20
#define FD_QOS          0x40
#define FD_GROUP_QOS    0x80
#define FD_ROUTING_INTERFACE_CHANGE 0x100
#define FD_ADDRESS_LIST_CHANGE 0x200
#define FD_MAX_EVENTS   10

/* Bit positions for iErrorCode array indexing */
#define FD_READ_BIT      0
#define FD_WRITE_BIT     1
#define FD_OOB_BIT       2
#define FD_ACCEPT_BIT    3
#define FD_CONNECT_BIT   4
#define FD_CLOSE_BIT     5
#define FD_QOS_BIT       6
#define FD_GROUP_QOS_BIT 7
#define FD_ROUTING_INTERFACE_CHANGE_BIT 8
#define FD_ADDRESS_LIST_CHANGE_BIT 9
#define FD_ALL_EVENTS   0x3FF

typedef struct _WSANETWORKEVENTS {
    long lNetworkEvents;
    int iErrorCode[FD_MAX_EVENTS];
} WSANETWORKEVENTS, *LPWSANETWORKEVENTS;

/* ============================================================================
 * WSAPROTOCOL_INFO Structure
 * ============================================================================ */

#define MAX_PROTOCOL_CHAIN 7
#define WSAPROTOCOL_LEN 255

typedef struct _GUID {
    uint32_t Data1;
    uint16_t Data2;
    uint16_t Data3;
    uint8_t  Data4[8];
} GUID;

typedef struct _WSAPROTOCOLCHAIN {
    int ChainLen;
    DWORD ChainEntries[MAX_PROTOCOL_CHAIN];
} WSAPROTOCOLCHAIN, *LPWSAPROTOCOLCHAIN;

typedef struct _WSAPROTOCOL_INFOA {
    DWORD dwServiceFlags1;
    DWORD dwServiceFlags2;
    DWORD dwServiceFlags3;
    DWORD dwServiceFlags4;
    DWORD dwProviderFlags;
    GUID ProviderId;
    DWORD dwCatalogEntryId;
    WSAPROTOCOLCHAIN ProtocolChain;
    int iVersion;
    int iAddressFamily;
    int iMaxSockAddr;
    int iMinSockAddr;
    int iSocketType;
    int iProtocol;
    int iProtocolMaxOffset;
    int iNetworkByteOrder;
    int iSecurityScheme;
    DWORD dwMessageSize;
    DWORD dwProviderReserved;
    char szProtocol[WSAPROTOCOL_LEN+1];
} WSAPROTOCOL_INFOA, *LPWSAPROTOCOL_INFOA;

typedef struct _WSAPROTOCOL_INFOW {
    DWORD dwServiceFlags1;
    DWORD dwServiceFlags2;
    DWORD dwServiceFlags3;
    DWORD dwServiceFlags4;
    DWORD dwProviderFlags;
    GUID ProviderId;
    DWORD dwCatalogEntryId;
    WSAPROTOCOLCHAIN ProtocolChain;
    int iVersion;
    int iAddressFamily;
    int iMaxSockAddr;
    int iMinSockAddr;
    int iSocketType;
    int iProtocol;
    int iProtocolMaxOffset;
    int iNetworkByteOrder;
    int iSecurityScheme;
    DWORD dwMessageSize;
    DWORD dwProviderReserved;
    wchar_t szProtocol[WSAPROTOCOL_LEN+1];
} WSAPROTOCOL_INFOW, *LPWSAPROTOCOL_INFOW;

#ifdef UNICODE
typedef WSAPROTOCOL_INFOW WSAPROTOCOL_INFO;
typedef LPWSAPROTOCOL_INFOW LPWSAPROTOCOL_INFO;
#else
typedef WSAPROTOCOL_INFOA WSAPROTOCOL_INFO;
typedef LPWSAPROTOCOL_INFOA LPWSAPROTOCOL_INFO;
#endif

/* ============================================================================
 * WSASocket Flags
 * ============================================================================ */

#define WSA_FLAG_OVERLAPPED             0x01
#define WSA_FLAG_MULTIPOINT_C_ROOT      0x02
#define WSA_FLAG_MULTIPOINT_C_LEAF      0x04
#define WSA_FLAG_MULTIPOINT_D_ROOT      0x08
#define WSA_FLAG_MULTIPOINT_D_LEAF      0x10
#define WSA_FLAG_ACCESS_SYSTEM_SECURITY 0x40
#define WSA_FLAG_NO_HANDLE_INHERIT      0x80
#define WSA_FLAG_REGISTERED_IO          0x100

/* ============================================================================
 * QOS Structures
 * ============================================================================ */

typedef struct _FLOWSPEC {
    uint32_t TokenRate;
    uint32_t TokenBucketSize;
    uint32_t PeakBandwidth;
    uint32_t Latency;
    uint32_t DelayVariation;
    uint32_t ServiceType;
    uint32_t MaxSduSize;
    uint32_t MinimumPolicedSize;
} FLOWSPEC, *PFLOWSPEC, *LPFLOWSPEC;

typedef struct _QualityOfService {
    FLOWSPEC SendingFlowspec;
    FLOWSPEC ReceivingFlowspec;
    WSABUF ProviderSpecific;
} QOS, *LPQOS;

/* ============================================================================
 * Winsock API Functions
 * ============================================================================ */

/* Initialization */
int WSAAPI WSAStartup(WORD wVersionRequested, LPWSADATA lpWSAData);
int WSAAPI WSACleanup(void);
int WSAAPI WSAGetLastError(void);
void WSAAPI WSASetLastError(int iError);

/* Windows-specific socket functions */
int WSAAPI closesocket(SOCKET s);
int WSAAPI ioctlsocket(SOCKET s, long cmd, unsigned long* argp);

/* WSASocket functions */
SOCKET WSAAPI WSASocketA(int af, int type, int protocol,
                         LPWSAPROTOCOL_INFOA lpProtocolInfo,
                         unsigned int g, DWORD dwFlags);
SOCKET WSAAPI WSASocketW(int af, int type, int protocol,
                         LPWSAPROTOCOL_INFOW lpProtocolInfo,
                         unsigned int g, DWORD dwFlags);

#ifdef UNICODE
#define WSASocket WSASocketW
#else
#define WSASocket WSASocketA
#endif

/* Extended connection functions */
SOCKET WSAAPI WSAAccept(SOCKET s, struct sockaddr* addr, int* addrlen,
                        void* lpfnCondition, DWORD_PTR dwCallbackData);

int WSAAPI WSAConnect(SOCKET s, const struct sockaddr* name, int namelen,
                      LPWSABUF lpCallerData, LPWSABUF lpCalleeData,
                      LPQOS lpSQOS, LPQOS lpGQOS);

/* Scatter-gather I/O */
int WSAAPI WSASend(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount,
                   DWORD* lpNumberOfBytesSent, DWORD dwFlags,
                   LPWSAOVERLAPPED lpOverlapped,
                   LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);

int WSAAPI WSASendTo(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount,
                     DWORD* lpNumberOfBytesSent, DWORD dwFlags,
                     const struct sockaddr* lpTo, int iTolen,
                     LPWSAOVERLAPPED lpOverlapped,
                     LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);

int WSAAPI WSARecv(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount,
                   DWORD* lpNumberOfBytesRecvd, DWORD* lpFlags,
                   LPWSAOVERLAPPED lpOverlapped,
                   LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);

int WSAAPI WSARecvFrom(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount,
                       DWORD* lpNumberOfBytesRecvd, DWORD* lpFlags,
                       struct sockaddr* lpFrom, int* lpFromlen,
                       LPWSAOVERLAPPED lpOverlapped,
                       LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);

int WSAAPI WSARecvMsg(SOCKET s, LPWSAMSG lpMsg, DWORD* lpdwNumberOfBytesRecvd,
                      LPWSAOVERLAPPED lpOverlapped,
                      LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);

int WSAAPI WSASendMsg(SOCKET s, LPWSAMSG lpMsg, DWORD dwFlags,
                      DWORD* lpNumberOfBytesSent,
                      LPWSAOVERLAPPED lpOverlapped,
                      LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);

/* I/O control */
int WSAAPI WSAIoctl(SOCKET s, DWORD dwIoControlCode,
                    LPVOID lpvInBuffer, DWORD cbInBuffer,
                    LPVOID lpvOutBuffer, DWORD cbOutBuffer,
                    DWORD* lpcbBytesReturned,
                    LPWSAOVERLAPPED lpOverlapped,
                    LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);

/* Event functions */
WSAEVENT WSAAPI WSACreateEvent(void);
BOOL WSAAPI WSACloseEvent(WSAEVENT hEvent);
BOOL WSAAPI WSASetEvent(WSAEVENT hEvent);
BOOL WSAAPI WSAResetEvent(WSAEVENT hEvent);
DWORD WSAAPI WSAWaitForMultipleEvents(DWORD cEvents, const WSAEVENT* lphEvents,
                                      BOOL fWaitAll, DWORD dwTimeout,
                                      BOOL fAlertable);
int WSAAPI WSAEventSelect(SOCKET s, WSAEVENT hEventObject, long lNetworkEvents);
int WSAAPI WSAEnumNetworkEvents(SOCKET s, WSAEVENT hEventObject,
                                LPWSANETWORKEVENTS lpNetworkEvents);

/* Async functions */
HANDLE WSAAPI WSAAsyncGetHostByName(HANDLE hWnd, unsigned int wMsg,
                                    const char* name, char* buf, int buflen);
HANDLE WSAAPI WSAAsyncGetHostByAddr(HANDLE hWnd, unsigned int wMsg,
                                    const char* addr, int len, int type,
                                    char* buf, int buflen);
HANDLE WSAAPI WSAAsyncGetServByName(HANDLE hWnd, unsigned int wMsg,
                                    const char* name, const char* proto,
                                    char* buf, int buflen);
HANDLE WSAAPI WSAAsyncGetServByPort(HANDLE hWnd, unsigned int wMsg,
                                    int port, const char* proto,
                                    char* buf, int buflen);
HANDLE WSAAPI WSAAsyncGetProtoByName(HANDLE hWnd, unsigned int wMsg,
                                     const char* name, char* buf, int buflen);
HANDLE WSAAPI WSAAsyncGetProtoByNumber(HANDLE hWnd, unsigned int wMsg,
                                       int number, char* buf, int buflen);
int WSAAPI WSACancelAsyncRequest(HANDLE hAsyncTaskHandle);
int WSAAPI WSAAsyncSelect(SOCKET s, HANDLE hWnd, unsigned int wMsg,
                          long lEvent);

/* Byte order conversion */
int WSAAPI WSAHtonl(SOCKET s, unsigned long hostlong, unsigned long* lpnetlong);
int WSAAPI WSAHtons(SOCKET s, unsigned short hostshort, unsigned short* lpnetshort);
int WSAAPI WSANtohl(SOCKET s, unsigned long netlong, unsigned long* lphostlong);
int WSAAPI WSANtohs(SOCKET s, unsigned short netshort, unsigned short* lphostshort);

/* Protocol enumeration */
int WSAAPI WSAEnumProtocolsA(INT* lpiProtocols,
                             LPWSAPROTOCOL_INFOA lpProtocolBuffer,
                             DWORD* lpdwBufferLength);
int WSAAPI WSAEnumProtocolsW(INT* lpiProtocols,
                             LPWSAPROTOCOL_INFOW lpProtocolBuffer,
                             DWORD* lpdwBufferLength);

#ifdef UNICODE
#define WSAEnumProtocols WSAEnumProtocolsW
#else
#define WSAEnumProtocols WSAEnumProtocolsA
#endif

/* Socket duplication */
int WSAAPI WSADuplicateSocketA(SOCKET s, DWORD dwProcessId,
                               LPWSAPROTOCOL_INFOA lpProtocolInfo);
int WSAAPI WSADuplicateSocketW(SOCKET s, DWORD dwProcessId,
                               LPWSAPROTOCOL_INFOW lpProtocolInfo);

#ifdef UNICODE
#define WSADuplicateSocket WSADuplicateSocketW
#else
#define WSADuplicateSocket WSADuplicateSocketA
#endif

/* Other functions */
BOOL WSAAPI WSAGetOverlappedResult(SOCKET s, LPWSAOVERLAPPED lpOverlapped,
                                   DWORD* lpcbTransfer, BOOL fWait,
                                   DWORD* lpdwFlags);

/* Socket pair (not in Windows but useful) */
int WSAAPI WSASocketPair(int af, int type, int protocol, SOCKET socks[2]);

/* Event constants */
#define WSA_INFINITE            0xFFFFFFFF
#define WSA_WAIT_EVENT_0        0
#define WSA_WAIT_FAILED         0xFFFFFFFF
#define WSA_WAIT_TIMEOUT        0x00000102
#define WSA_MAXIMUM_WAIT_EVENTS 64

#ifdef __cplusplus
}
#endif

#endif /* __linux__ */

#endif /* _WINSOCK2_API_H */
