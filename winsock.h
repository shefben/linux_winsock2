/*
 * winsock.h - Windows Socket API 1.1 for Linux
 * Full Winsock 1.1 implementation compatible with wsock32.dll
 */

#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_

#ifdef __linux__

/* Include necessary system headers first */
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
#include <string.h>
#include <stdlib.h>

#include "windows_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================================
 * Winsock 1.1 Version and Basic Types
 * ============================================================================ */

/* Winsock 1.1 version (MAKEWORD is from windows_types.h) */
#define WINSOCK_VERSION MAKEWORD(1,1)

/* Socket type */
typedef int SOCKET;

#define INVALID_SOCKET  (-1)
#define SOCKET_ERROR    (-1)

/* ============================================================================
 * Winsock 1.1 Error Codes
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

/* Extended error codes */
#define WSASYSNOTREADY          (WSABASEERR+91)
#define WSAVERNOTSUPPORTED      (WSABASEERR+92)
#define WSANOTINITIALISED       (WSABASEERR+93)
#define WSAEDISCON              (WSABASEERR+101)

/* Winsock 1.1 specific errors */
#define WSAHOST_NOT_FOUND       (WSABASEERR+1001)
#define WSATRY_AGAIN            (WSABASEERR+1002)
#define WSANO_RECOVERY          (WSABASEERR+1003)
#define WSANO_DATA              (WSABASEERR+1004)

/* Compatibility aliases */
#ifndef HOST_NOT_FOUND
#define HOST_NOT_FOUND          WSAHOST_NOT_FOUND
#endif
#ifndef TRY_AGAIN
#define TRY_AGAIN               WSATRY_AGAIN
#endif
#ifndef NO_RECOVERY
#define NO_RECOVERY             WSANO_RECOVERY
#endif
#ifndef NO_DATA
#define NO_DATA                 WSANO_DATA
#endif
#define NO_ADDRESS              WSANO_DATA

/* ============================================================================
 * WSADATA Structure
 * ============================================================================ */

#define WSADESCRIPTION_LEN      256
#define WSASYS_STATUS_LEN       128

typedef struct WSAData {
    WORD wVersion;
    WORD wHighVersion;
    char szDescription[WSADESCRIPTION_LEN+1];
    char szSystemStatus[WSASYS_STATUS_LEN+1];
    unsigned short iMaxSockets;
    unsigned short iMaxUdpDg;
    char* lpVendorInfo;
} WSADATA, *LPWSADATA;

/* ============================================================================
 * Socket Address and Protocol Structures
 * Use Linux system structures directly
 * ============================================================================ */

/* sockaddr is from system headers */
typedef struct sockaddr SOCKADDR;
typedef struct sockaddr *PSOCKADDR;
typedef struct sockaddr *LPSOCKADDR;

typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr_in *PSOCKADDR_IN;
typedef struct sockaddr_in *LPSOCKADDR_IN;

/* hostent, servent, protoent are from system headers */
typedef struct hostent HOSTENT;
typedef struct hostent *PHOSTENT;
typedef struct hostent *LPHOSTENT;

typedef struct servent SERVENT;
typedef struct servent *PSERVENT;
typedef struct servent *LPSERVENT;

typedef struct protoent PROTOENT;
typedef struct protoent *PPROTOENT;
typedef struct protoent *LPPROTOENT;

/* ============================================================================
 * Socket Options and Flags
 * ============================================================================ */

/* Socket options - use Linux values */
#ifndef SO_DONTLINGER
#define SO_DONTLINGER   (~SO_LINGER)
#endif

/* ioctl commands */
#ifndef FIONBIO
#define FIONBIO         0x5421
#endif
#ifndef FIONREAD
#define FIONREAD        0x541B
#endif
#ifndef SIOCATMARK
#define SIOCATMARK      0x8905
#endif

/* IP options */
#ifndef IP_OPTIONS
#define IP_OPTIONS      4
#endif

/* Shutdown options */
#define SD_RECEIVE      SHUT_RD
#define SD_SEND         SHUT_WR
#define SD_BOTH         SHUT_RDWR

/* Message flags - use Linux values from sys/socket.h */
#ifndef MSG_PEEK
#define MSG_PEEK        0x02
#endif
#ifndef MSG_OOB
#define MSG_OOB         0x01
#endif
#ifndef MSG_DONTROUTE
#define MSG_DONTROUTE   0x04
#endif

/* Note: Address families (AF_*) and protocol families (PF_*) are provided
 * by the system headers (sys/socket.h). We use the Linux definitions directly. */

/* ============================================================================
 * Winsock 1.1 Core Functions
 * ============================================================================ */

/* Initialization */
int WSAAPI WSAStartup(WORD wVersionRequested, LPWSADATA lpWSAData);
int WSAAPI WSACleanup(void);

/* Error handling */
int WSAAPI WSAGetLastError(void);
void WSAAPI WSASetLastError(int iError);

/* Socket functions */
int WSAAPI closesocket(SOCKET s);

/* Note: These functions are available directly from POSIX:
 * socket(), bind(), connect(), listen(), accept()
 * send(), recv(), sendto(), recvfrom()
 * shutdown(), getsockopt(), setsockopt()
 * getsockname(), getpeername()
 * select() - uses system fd_set
 */

/* Windows-specific ioctl wrapper */
int WSAAPI ioctlsocket(SOCKET s, long cmd, unsigned long* argp);

/* ============================================================================
 * Name Resolution Functions (Available from POSIX)
 * ============================================================================ */

/* Note: These are available directly from system headers:
 * gethostbyname(), gethostbyaddr(), gethostname()
 * getservbyname(), getservbyport()
 * getprotobyname(), getprotobynumber()
 * inet_addr(), inet_ntoa()
 * htons(), htonl(), ntohs(), ntohl()
 */

/* ============================================================================
 * Winsock 1.1 Asynchronous Functions
 * ============================================================================ */

typedef HANDLE WSAEVENT;

/* Async operations */
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

HANDLE WSAAPI WSAAsyncGetHostByName(HANDLE hWnd, unsigned int wMsg,
                                    const char* name, char* buf, int buflen);

HANDLE WSAAPI WSAAsyncGetHostByAddr(HANDLE hWnd, unsigned int wMsg,
                                    const char* addr, int len, int type,
                                    char* buf, int buflen);

int WSAAPI WSACancelAsyncRequest(HANDLE hAsyncTaskHandle);

/* Async select */
int WSAAPI WSAAsyncSelect(SOCKET s, HANDLE hWnd, unsigned int wMsg, long lEvent);

/* Event constants for WSAAsyncSelect */
#define FD_READ         0x01
#define FD_WRITE        0x02
#define FD_OOB          0x04
#define FD_ACCEPT       0x08
#define FD_CONNECT      0x10
#define FD_CLOSE        0x20

/* ============================================================================
 * Winsock 1.1 Blocking Hook Functions
 * ============================================================================ */

typedef BOOL (WSAAPI *LPWSABLOCKINGHOOK)(void);
typedef void (WSAAPI *LPWSAPROC)(void);

/* Blocking hook management */
LPWSABLOCKINGHOOK WSAAPI WSASetBlockingHook(LPWSABLOCKINGHOOK lpBlockFunc);
int WSAAPI WSAUnhookBlockingHook(void);
int WSAAPI WSACancelBlockingCall(void);
BOOL WSAAPI WSAIsBlocking(void);

/* ============================================================================
 * Database Functions
 * ============================================================================ */

/* These macros extract data from the hostent structure */
#define h_addr h_addr_list[0]  /* For backward compatibility */

/* ============================================================================
 * Byte Order Conversion (Available from POSIX)
 * ============================================================================ */

/* Note: htons(), htonl(), ntohs(), ntohl() available from <arpa/inet.h> */

#ifdef __cplusplus
}
#endif

#else  /* !__linux__ */

/* On Windows, include the real winsock.h */
#include <winsock.h>

#endif /* __linux__ */

#endif /* _WINSOCKAPI_ */
