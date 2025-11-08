#ifndef _WS2TCPIP_H
#define _WS2TCPIP_H

#include "winsock2.h"

#ifdef __linux__

#ifdef __cplusplus
extern "C" {
#endif

/* IPv6 address structure */
struct in6_addr {
    union {
        uint8_t u6_addr8[16];
        uint16_t u6_addr16[8];
        uint32_t u6_addr32[4];
    } u6_addr;
};

#define s6_addr   u6_addr.u6_addr8
#define s6_addr16 u6_addr.u6_addr16
#define s6_addr32 u6_addr.u6_addr32

/* IPv6 socket address */
struct sockaddr_in6 {
    short sin6_family;
    uint16_t sin6_port;
    uint32_t sin6_flowinfo;
    struct in6_addr sin6_addr;
    uint32_t sin6_scope_id;
};

typedef struct sockaddr_in6 SOCKADDR_IN6;
typedef struct sockaddr_in6* PSOCKADDR_IN6;
typedef struct sockaddr_in6* LPSOCKADDR_IN6;

/* Storage for any socket address */
struct sockaddr_storage {
    short ss_family;
    char __ss_pad1[6];
    int64_t __ss_align;
    char __ss_pad2[112];
};

typedef struct sockaddr_storage SOCKADDR_STORAGE;
typedef struct sockaddr_storage* PSOCKADDR_STORAGE;
typedef struct sockaddr_storage* LPSOCKADDR_STORAGE;

/* Address info structures */
typedef struct addrinfo {
    int ai_flags;
    int ai_family;
    int ai_socktype;
    int ai_protocol;
    size_t ai_addrlen;
    char* ai_canonname;
    struct sockaddr* ai_addr;
    struct addrinfo* ai_next;
} ADDRINFOA, *PADDRINFOA;

typedef struct addrinfoW {
    int ai_flags;
    int ai_family;
    int ai_socktype;
    int ai_protocol;
    size_t ai_addrlen;
    wchar_t* ai_canonname;
    struct sockaddr* ai_addr;
    struct addrinfoW* ai_next;
} ADDRINFOW, *PADDRINFOW;

#ifdef UNICODE
typedef ADDRINFOW ADDRINFO;
typedef PADDRINFOW LPADDRINFO;
#else
typedef ADDRINFOA ADDRINFO;
typedef PADDRINFOA LPADDRINFO;
#endif

/* getaddrinfo flags */
#define AI_PASSIVE      0x00000001
#define AI_CANONNAME    0x00000002
#define AI_NUMERICHOST  0x00000004
#define AI_NUMERICSERV  0x00000008
#define AI_ALL          0x00000100
#define AI_ADDRCONFIG   0x00000400
#define AI_V4MAPPED     0x00000800
#define AI_NON_AUTHORITATIVE 0x00004000
#define AI_SECURE       0x00008000
#define AI_RETURN_PREFERRED_NAMES 0x00010000
#define AI_FQDN         0x00020000
#define AI_FILESERVER   0x00040000

/* getnameinfo flags */
#define NI_NOFQDN       0x01
#define NI_NUMERICHOST  0x02
#define NI_NAMEREQD     0x04
#define NI_NUMERICSERV  0x08
#define NI_DGRAM        0x10
#define NI_MAXHOST      1025
#define NI_MAXSERV      32

/* IPv6 address info */
extern const struct in6_addr in6addr_any;
extern const struct in6_addr in6addr_loopback;

#define IN6ADDR_ANY_INIT { { { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } } }
#define IN6ADDR_LOOPBACK_INIT { { { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1 } } }

/* IPv6 macros */
#define IN6_ARE_ADDR_EQUAL(a, b) \
    (memcmp(&(a)->s6_addr[0], &(b)->s6_addr[0], sizeof(struct in6_addr)) == 0)

#define IN6_IS_ADDR_UNSPECIFIED(a) ( \
    ((a)->s6_addr32[0] == 0) && \
    ((a)->s6_addr32[1] == 0) && \
    ((a)->s6_addr32[2] == 0) && \
    ((a)->s6_addr32[3] == 0))

#define IN6_IS_ADDR_LOOPBACK(a) ( \
    ((a)->s6_addr32[0] == 0) && \
    ((a)->s6_addr32[1] == 0) && \
    ((a)->s6_addr32[2] == 0) && \
    ((a)->s6_addr[12] == 0) && \
    ((a)->s6_addr[13] == 0) && \
    ((a)->s6_addr[14] == 0) && \
    ((a)->s6_addr[15] == 1))

#define IN6_IS_ADDR_MULTICAST(a) ((a)->s6_addr[0] == 0xff)

#define IN6_IS_ADDR_LINKLOCAL(a) ( \
    ((a)->s6_addr[0] == 0xfe) && \
    (((a)->s6_addr[1] & 0xc0) == 0x80))

#define IN6_IS_ADDR_SITELOCAL(a) ( \
    ((a)->s6_addr[0] == 0xfe) && \
    (((a)->s6_addr[1] & 0xc0) == 0xc0))

#define IN6_IS_ADDR_V4MAPPED(a) ( \
    ((a)->s6_addr32[0] == 0) && \
    ((a)->s6_addr32[1] == 0) && \
    ((a)->s6_addr[8] == 0) && \
    ((a)->s6_addr[9] == 0) && \
    ((a)->s6_addr[10] == 0xff) && \
    ((a)->s6_addr[11] == 0xff))

#define IN6_IS_ADDR_V4COMPAT(a) ( \
    ((a)->s6_addr32[0] == 0) && \
    ((a)->s6_addr32[1] == 0) && \
    ((a)->s6_addr32[2] == 0) && \
    ((a)->s6_addr32[3] != 0) && \
    ((a)->s6_addr32[3] != htonl(1)))

#define IN6_IS_ADDR_MC_NODELOCAL(a) ( \
    IN6_IS_ADDR_MULTICAST(a) && \
    (((a)->s6_addr[1] & 0x0f) == 0x01))

#define IN6_IS_ADDR_MC_LINKLOCAL(a) ( \
    IN6_IS_ADDR_MULTICAST(a) && \
    (((a)->s6_addr[1] & 0x0f) == 0x02))

#define IN6_IS_ADDR_MC_SITELOCAL(a) ( \
    IN6_IS_ADDR_MULTICAST(a) && \
    (((a)->s6_addr[1] & 0x0f) == 0x05))

#define IN6_IS_ADDR_MC_ORGLOCAL(a) ( \
    IN6_IS_ADDR_MULTICAST(a) && \
    (((a)->s6_addr[1] & 0x0f) == 0x08))

#define IN6_IS_ADDR_MC_GLOBAL(a) ( \
    IN6_IS_ADDR_MULTICAST(a) && \
    (((a)->s6_addr[1] & 0x0f) == 0x0e))

/* Address to string conversion */
int WSAAPI inet_pton(int af, const char* src, void* dst);
const char* WSAAPI inet_ntop(int af, const void* src, char* dst, size_t size);
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

/* Name resolution */
int WSAAPI getaddrinfo(const char* nodename, const char* servname,
                       const struct addrinfo* hints, struct addrinfo** res);
void WSAAPI freeaddrinfo(struct addrinfo* ai);
int WSAAPI getnameinfo(const struct sockaddr* sa, int salen,
                       char* host, DWORD hostlen,
                       char* serv, DWORD servlen, int flags);

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

/* EAI error codes */
#define EAI_AGAIN       WSATRY_AGAIN
#define EAI_BADFLAGS    WSAEINVAL
#define EAI_FAIL        WSANO_RECOVERY
#define EAI_FAMILY      WSAEAFNOSUPPORT
#define EAI_MEMORY      WSA_NOT_ENOUGH_MEMORY
#define EAI_NONAME      WSAHOST_NOT_FOUND
#define EAI_SERVICE     WSATYPE_NOT_FOUND
#define EAI_SOCKTYPE    WSAESOCKTNOSUPPORT
#define EAI_NODATA      WSANO_DATA

/* TCP INFO structure (Linux-specific but useful) */
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

/* Socket address length */
#define INET_ADDRSTRLEN  16
#define INET6_ADDRSTRLEN 46

/* IP packet info structures */
typedef struct _IN_PKTINFO {
    struct in_addr ipi_addr;
    int ipi_ifindex;
} IN_PKTINFO;

typedef struct _IN6_PKTINFO {
    struct in6_addr ipi6_addr;
    DWORD ipi6_ifindex;
} IN6_PKTINFO;

/* Multicast source group */
struct group_source_req {
    uint32_t gsr_interface;
    struct sockaddr_storage gsr_group;
    struct sockaddr_storage gsr_source;
};

/* Socket control messages */
#ifndef CMSG_FIRSTHDR
#define CMSG_FIRSTHDR(mhdr) \
    ((mhdr)->Control.len >= sizeof(struct cmsghdr) ? \
     (struct cmsghdr*)(mhdr)->Control.buf : \
     (struct cmsghdr*)NULL)

#define CMSG_NXTHDR(mhdr, cmsg) \
    (((cmsg) == NULL) ? CMSG_FIRSTHDR(mhdr) : \
     (((unsigned char*)(cmsg) + CMSG_ALIGN((cmsg)->cmsg_len) + \
       CMSG_ALIGN(sizeof(struct cmsghdr)) > \
       (unsigned char*)((mhdr)->Control.buf) + (mhdr)->Control.len) ? \
      (struct cmsghdr*)NULL : \
      (struct cmsghdr*)((unsigned char*)(cmsg) + CMSG_ALIGN((cmsg)->cmsg_len))))

#define CMSG_DATA(cmsg) \
    ((unsigned char*)(cmsg) + CMSG_ALIGN(sizeof(struct cmsghdr)))

#define CMSG_SPACE(len) \
    (CMSG_ALIGN(sizeof(struct cmsghdr)) + CMSG_ALIGN(len))

#define CMSG_LEN(len) \
    (CMSG_ALIGN(sizeof(struct cmsghdr)) + (len))

#define CMSG_ALIGN(len) \
    (((len) + sizeof(size_t) - 1) & ~(sizeof(size_t) - 1))
#endif

/* Control message structure */
struct cmsghdr {
    size_t cmsg_len;
    int cmsg_level;
    int cmsg_type;
};

/* SCM rights */
#define SCM_RIGHTS 1

/* Interface name/index functions */
#define IF_NAMESIZE 16

char* if_indextoname(unsigned int ifindex, char* ifname);
unsigned int if_nametoindex(const char* ifname);

#ifdef __cplusplus
}
#endif

#endif /* __linux__ */

#endif /* _WS2TCPIP_H */
