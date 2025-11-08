/*
 * ws2tcpip.h - TCP/IP specific definitions for Winsock2
 * Compatible header for compiling Windows socket applications on Linux
 */

#ifndef _WS2TCPIP_H
#define _WS2TCPIP_H

#include "winsock2.h"

#ifdef __linux__

/* Linux already has these in netinet/in.h and netdb.h */
#include <netinet/in.h>
#include <netdb.h>
#include <net/if.h>

#ifdef __cplusplus
extern "C" {
#endif

/* IPv6 addresses are already defined in netinet/in.h */
typedef struct sockaddr_in6 SOCKADDR_IN6;
typedef struct sockaddr_in6* PSOCKADDR_IN6;
typedef struct sockaddr_in6* LPSOCKADDR_IN6;

/* sockaddr_storage already defined in sys/socket.h */
typedef struct sockaddr_storage SOCKADDR_STORAGE;
typedef struct sockaddr_storage* PSOCKADDR_STORAGE;
typedef struct sockaddr_storage* LPSOCKADDR_STORAGE;

/* addrinfo already defined in netdb.h */
typedef struct addrinfo ADDRINFOA;
typedef struct addrinfo* PADDRINFOA;
typedef struct addrinfo* LPADDRINFOA;

/* Wide character version */
typedef struct addrinfoW {
    int ai_flags;
    int ai_family;
    int ai_socktype;
    int ai_protocol;
    size_t ai_addrlen;
    wchar_t* ai_canonname;
    struct sockaddr* ai_addr;
    struct addrinfoW* ai_next;
} ADDRINFOW, *PADDRINFOW, *LPADDRINFOW;

#ifdef UNICODE
typedef ADDRINFOW ADDRINFO;
typedef PADDRINFOW LPADDRINFO;
#else
typedef ADDRINFOA ADDRINFO;
typedef PADDRINFOA LPADDRINFO;
#endif

/* getaddrinfo flags - use Linux values */
#ifndef AI_PASSIVE
#define AI_PASSIVE      0x00000001
#define AI_CANONNAME    0x00000002
#define AI_NUMERICHOST  0x00000004
#define AI_NUMERICSERV  0x00000008
#define AI_ALL          0x00000100
#define AI_ADDRCONFIG   0x00000400
#define AI_V4MAPPED     0x00000800
#endif

/* Windows-specific AI flags (not supported) */
#define AI_NON_AUTHORITATIVE 0x00004000
#define AI_SECURE       0x00008000
#define AI_RETURN_PREFERRED_NAMES 0x00010000
#define AI_FQDN         0x00020000
#define AI_FILESERVER   0x00040000

/* getnameinfo flags - use Linux values */
#ifndef NI_NOFQDN
#define NI_NOFQDN       0x01
#define NI_NUMERICHOST  0x02
#define NI_NAMEREQD     0x04
#define NI_NUMERICSERV  0x08
#define NI_DGRAM        0x10
#endif

#ifndef NI_MAXHOST
#define NI_MAXHOST      1025
#define NI_MAXSERV      32
#endif

/* Address string lengths */
#ifndef INET_ADDRSTRLEN
#define INET_ADDRSTRLEN  16
#endif
#ifndef INET6_ADDRSTRLEN
#define INET6_ADDRSTRLEN 46
#endif

/* IP packet info structures */
#ifndef _IP_PKTINFO_DEFINED
#define _IP_PKTINFO_DEFINED
typedef struct _IN_PKTINFO {
    struct in_addr ipi_addr;
    int ipi_ifindex;
} IN_PKTINFO;

typedef struct _IN6_PKTINFO {
    struct in6_addr ipi6_addr;
    DWORD ipi6_ifindex;
} IN6_PKTINFO;
#endif

/* TCP INFO structure */
typedef struct _TCP_INFO_v0 {
    DWORD State;
    DWORD Mss;
    DWORD ConnectionTimeMs;
    BOOL TimestampsEnabled;
    DWORD RttUs;
    DWORD MinRttUs;
    DWORD BytesInFlight;
    DWORD Cwnd;
    DWORD SndWnd;
    DWORD RcvWnd;
    DWORD RcvBuf;
    DWORD BytesOut;
    DWORD BytesIn;
    DWORD BytesReordered;
    DWORD BytesRetrans;
    DWORD FastRetrans;
    DWORD DupAcksIn;
    DWORD TimeoutEpisodes;
    BYTE SynRetrans;
} TCP_INFO_v0, *PTCP_INFO_v0;

/* Windows-style address conversion functions
 * Note: inet_pton and inet_ntop are available from arpa/inet.h
 */
int WSAAPI InetPtonA(int Family, const char* pszAddrString, void* pAddrBuf);
int WSAAPI InetPtonW(int Family, const wchar_t* pszAddrString, void* pAddrBuf);
const char* WSAAPI InetNtopA(int Family, const void* pAddr, char* pStringBuf, size_t StringBufSize);
const wchar_t* WSAAPI InetNtopW(int Family, const void* pAddr, wchar_t* pStringBuf, size_t StringBufSize);

#ifdef UNICODE
#define InetPton InetPtonW
#define InetNtop InetNtopW
#else
#define InetPton InetPtonA
#define InetNtop InetNtopA
#endif

/* Windows-style getaddrinfo/getnameinfo
 * Note: getaddrinfo, freeaddrinfo, getnameinfo are available from netdb.h
 */
int WSAAPI GetAddrInfoA(const char* pNodeName, const char* pServiceName,
                        const ADDRINFOA* pHints, ADDRINFOA** ppResult);
int WSAAPI GetAddrInfoW(const wchar_t* pNodeName, const wchar_t* pServiceName,
                        const ADDRINFOW* pHints, ADDRINFOW** ppResult);
void WSAAPI FreeAddrInfoA(ADDRINFOA* pAddrInfo);
void WSAAPI FreeAddrInfoW(ADDRINFOW* pAddrInfo);

int WSAAPI GetNameInfoA(const SOCKADDR* pSockaddr, int SockaddrLength,
                        char* pNodeBuffer, DWORD NodeBufferSize,
                        char* pServiceBuffer, DWORD ServiceBufferSize,
                        INT Flags);
int WSAAPI GetNameInfoW(const SOCKADDR* pSockaddr, int SockaddrLength,
                        wchar_t* pNodeBuffer, DWORD NodeBufferSize,
                        wchar_t* pServiceBuffer, DWORD ServiceBufferSize,
                        INT Flags);

#ifdef UNICODE
#define GetAddrInfo GetAddrInfoW
#define FreeAddrInfo FreeAddrInfoW
#define GetNameInfo GetNameInfoW
#else
#define GetAddrInfo GetAddrInfoA
#define FreeAddrInfo FreeAddrInfoA
#define GetNameInfo GetNameInfoA
#endif

/* Address to string conversion */
int WSAAPI WSAAddressToStringA(LPSOCKADDR lpsaAddress, DWORD dwAddressLength,
                               LPWSAPROTOCOL_INFOA lpProtocolInfo,
                               LPSTR lpszAddressString,
                               DWORD* lpdwAddressStringLength);
int WSAAPI WSAAddressToStringW(LPSOCKADDR lpsaAddress, DWORD dwAddressLength,
                               LPWSAPROTOCOL_INFOW lpProtocolInfo,
                               LPWSTR lpszAddressString,
                               DWORD* lpdwAddressStringLength);
int WSAAPI WSAStringToAddressA(LPSTR AddressString, INT AddressFamily,
                               LPWSAPROTOCOL_INFOA lpProtocolInfo,
                               LPSOCKADDR lpAddress, INT* lpAddressLength);
int WSAAPI WSAStringToAddressW(LPWSTR AddressString, INT AddressFamily,
                               LPWSAPROTOCOL_INFOW lpProtocolInfo,
                               LPSOCKADDR lpAddress, INT* lpAddressLength);

#ifdef UNICODE
#define WSAAddressToString WSAAddressToStringW
#define WSAStringToAddress WSAStringToAddressW
#else
#define WSAAddressToString WSAAddressToStringA
#define WSAStringToAddress WSAStringToAddressA
#endif

/* Interface name/index functions (available from net/if.h as if_nametoindex/if_indextoname) */

#ifdef __cplusplus
}
#endif

#else
/* On Windows, include the real ws2tcpip.h */
#include <ws2tcpip.h>
#endif

#endif /* _WS2TCPIP_H */
