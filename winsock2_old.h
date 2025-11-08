#ifndef _WINSOCK2_H
#define _WINSOCK2_H

#ifdef __linux__

#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
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

#ifdef __cplusplus
extern "C" {
#endif

/* Windows-style types */
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef int BOOL;
typedef void* HANDLE;
typedef void* PVOID;
typedef void* LPVOID;
typedef const void* LPCVOID;
typedef char* LPSTR;
typedef const char* LPCSTR;
typedef wchar_t* LPWSTR;
typedef const wchar_t* LPCWSTR;
typedef uint64_t UINT_PTR;
typedef int64_t INT_PTR;
typedef uint64_t ULONG_PTR;
typedef ULONG_PTR DWORD_PTR;
typedef HANDLE WSAEVENT;
typedef void* LPWSAOVERLAPPED;
typedef struct _GUID {
    uint32_t Data1;
    uint16_t Data2;
    uint16_t Data3;
    uint8_t  Data4[8];
} GUID;

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#ifndef INVALID_HANDLE_VALUE
#define INVALID_HANDLE_VALUE ((HANDLE)(long)-1)
#endif

#define WINAPI
#define WSAAPI
#define FAR
#define PASCAL

/* Socket types */
typedef int SOCKET;
typedef struct sockaddr SOCKADDR;
typedef struct sockaddr* PSOCKADDR;
typedef struct sockaddr* LPSOCKADDR;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr_in* PSOCKADDR_IN;
typedef struct sockaddr_in* LPSOCKADDR_IN;

#define INVALID_SOCKET  (-1)
#define SOCKET_ERROR    (-1)

/* Winsock version info */
#define MAKEWORD(a,b) ((WORD)(((BYTE)(a))|(((WORD)((BYTE)(b)))<<8)))
#define LOBYTE(w) ((BYTE)(w))
#define HIBYTE(w) ((BYTE)(((WORD)(w)>>8)&0xFF))

#define WINSOCK_VERSION MAKEWORD(2,2)

/* Maximum queue length for pending connections */
#define SOMAXCONN 128

/* Socket types */
#define SOCK_STREAM     1
#define SOCK_DGRAM      2
#define SOCK_RAW        3
#define SOCK_RDM        4
#define SOCK_SEQPACKET  5

/* Address families */
#define AF_UNSPEC       0
#define AF_UNIX         1
#define AF_INET         2
#define AF_IMPLINK      3
#define AF_PUP          4
#define AF_CHAOS        5
#define AF_NS           6
#define AF_IPX          6
#define AF_ISO          7
#define AF_OSI          7
#define AF_ECMA         8
#define AF_DATAKIT      9
#define AF_CCITT        10
#define AF_SNA          11
#define AF_DECnet       12
#define AF_DLI          13
#define AF_LAT          14
#define AF_HYLINK       15
#define AF_APPLETALK    16
#define AF_NETBIOS      17
#define AF_VOICEVIEW    18
#define AF_FIREFOX      19
#define AF_UNKNOWN1     20
#define AF_BAN          21
#define AF_ATM          22
#define AF_INET6        23
#define AF_CLUSTER      24
#define AF_12844        25
#define AF_IRDA         26
#define AF_NETDES       28
#define AF_TCNPROCESS   29
#define AF_TCNMESSAGE   30
#define AF_ICLFXBM      31
#define AF_BTH          32
#define AF_MAX          33

/* Protocol families (same as address families) */
#define PF_UNSPEC       AF_UNSPEC
#define PF_UNIX         AF_UNIX
#define PF_INET         AF_INET
#define PF_IMPLINK      AF_IMPLINK
#define PF_PUP          AF_PUP
#define PF_CHAOS        AF_CHAOS
#define PF_NS           AF_NS
#define PF_IPX          AF_IPX
#define PF_ISO          AF_ISO
#define PF_OSI          AF_OSI
#define PF_ECMA         AF_ECMA
#define PF_DATAKIT      AF_DATAKIT
#define PF_CCITT        AF_CCITT
#define PF_SNA          AF_SNA
#define PF_DECnet       AF_DECnet
#define PF_DLI          AF_DLI
#define PF_LAT          AF_LAT
#define PF_HYLINK       AF_HYLINK
#define PF_APPLETALK    AF_APPLETALK
#define PF_VOICEVIEW    AF_VOICEVIEW
#define PF_FIREFOX      AF_FIREFOX
#define PF_UNKNOWN1     AF_UNKNOWN1
#define PF_BAN          AF_BAN
#define PF_ATM          AF_ATM
#define PF_INET6        AF_INET6
#define PF_MAX          AF_MAX

/* Protocol numbers */
#define IPPROTO_IP      0
#define IPPROTO_ICMP    1
#define IPPROTO_IGMP    2
#define IPPROTO_GGP     3
#define IPPROTO_TCP     6
#define IPPROTO_PUP     12
#define IPPROTO_UDP     17
#define IPPROTO_IDP     22
#define IPPROTO_IPV6    41
#define IPPROTO_ROUTING 43
#define IPPROTO_FRAGMENT 44
#define IPPROTO_ESP     50
#define IPPROTO_AH      51
#define IPPROTO_ICMPV6  58
#define IPPROTO_NONE    59
#define IPPROTO_DSTOPTS 60
#define IPPROTO_ND      77
#define IPPROTO_ICLFXBM 78
#define IPPROTO_RAW     255
#define IPPROTO_MAX     256

/* Socket options - socket level */
#define SOL_SOCKET      0xffff

#define SO_DEBUG        0x0001
#define SO_ACCEPTCONN   0x0002
#define SO_REUSEADDR    0x0004
#define SO_KEEPALIVE    0x0008
#define SO_DONTROUTE    0x0010
#define SO_BROADCAST    0x0020
#define SO_USELOOPBACK  0x0040
#define SO_LINGER       0x0080
#define SO_OOBINLINE    0x0100
#define SO_DONTLINGER   (int)(~SO_LINGER)
#define SO_EXCLUSIVEADDRUSE ((int)(~SO_REUSEADDR))
#define SO_SNDBUF       0x1001
#define SO_RCVBUF       0x1002
#define SO_SNDLOWAT     0x1003
#define SO_RCVLOWAT     0x1004
#define SO_SNDTIMEO     0x1005
#define SO_RCVTIMEO     0x1006
#define SO_ERROR        0x1007
#define SO_TYPE         0x1008
#define SO_GROUP_ID     0x2001
#define SO_GROUP_PRIORITY 0x2002
#define SO_MAX_MSG_SIZE 0x2003
#define SO_PROTOCOL_INFOA 0x2004
#define SO_PROTOCOL_INFOW 0x2005
#define SO_CONDITIONAL_ACCEPT 0x3002
#define SO_PAUSE_ACCEPT 0x3003
#define SO_COMPARTMENT_ID 0x3004
#define SO_RANDOMIZE_PORT 0x3005
#define SO_PORT_SCALABILITY 0x3006

/* TCP options */
#define TCP_NODELAY     0x0001
#define TCP_EXPEDITED_1122 0x0002
#define TCP_KEEPALIVE   3
#define TCP_MAXSEG      4
#define TCP_MAXRT       5
#define TCP_STDURG      6
#define TCP_NOPUSH      7
#define TCP_NOOPT       8
#define TCP_BSDURGENT   0x7000

/* IP options */
#define IP_OPTIONS          1
#define IP_HDRINCL          2
#define IP_TOS              3
#define IP_TTL              4
#define IP_MULTICAST_IF     9
#define IP_MULTICAST_TTL    10
#define IP_MULTICAST_LOOP   11
#define IP_ADD_MEMBERSHIP   12
#define IP_DROP_MEMBERSHIP  13
#define IP_DONTFRAGMENT     14
#define IP_ADD_SOURCE_MEMBERSHIP  15
#define IP_DROP_SOURCE_MEMBERSHIP 16
#define IP_BLOCK_SOURCE     17
#define IP_UNBLOCK_SOURCE   18
#define IP_PKTINFO          19
#define IP_RECEIVE_BROADCAST 22

/* IPv6 options */
#define IPV6_HDRINCL        2
#define IPV6_UNICAST_HOPS   4
#define IPV6_MULTICAST_IF   9
#define IPV6_MULTICAST_HOPS 10
#define IPV6_MULTICAST_LOOP 11
#define IPV6_ADD_MEMBERSHIP 12
#define IPV6_DROP_MEMBERSHIP 13
#define IPV6_JOIN_GROUP     IPV6_ADD_MEMBERSHIP
#define IPV6_LEAVE_GROUP    IPV6_DROP_MEMBERSHIP
#define IPV6_DONTFRAG       14
#define IPV6_PKTINFO        19
#define IPV6_HOPLIMIT       21
#define IPV6_CHECKSUM       26
#define IPV6_V6ONLY         27

/* Flags for send/recv */
#define MSG_OOB         0x1
#define MSG_PEEK        0x2
#define MSG_DONTROUTE   0x4
#define MSG_WAITALL     0x8
#define MSG_PARTIAL     0x8000

/* Shutdown options */
#define SD_RECEIVE      0x00
#define SD_SEND         0x01
#define SD_BOTH         0x02
#define SHUT_RD         SD_RECEIVE
#define SHUT_WR         SD_SEND
#define SHUT_RDWR       SD_BOTH

/* IOCTLs */
#define FIONREAD    0x4004667f
#define FIONBIO     0x8004667e
#define FIOASYNC    0x8004667d
#define SIOCSHIWAT  0x80047300
#define SIOCGHIWAT  0x40047301
#define SIOCSLOWAT  0x80047302
#define SIOCGLOWAT  0x40047303
#define SIOCATMARK  0x40047307

/* IOCTLs for Windows compatibility */
#define IOCPARM_MASK    0x7f
#define IOC_VOID        0x20000000
#define IOC_OUT         0x40000000
#define IOC_IN          0x80000000
#define IOC_INOUT       (IOC_IN|IOC_OUT)

#define _IO(x,y)        (IOC_VOID|((x)<<8)|(y))
#define _IOR(x,y,t)     (IOC_OUT|(((long)sizeof(t)&IOCPARM_MASK)<<16)|((x)<<8)|(y))
#define _IOW(x,y,t)     (IOC_IN|(((long)sizeof(t)&IOCPARM_MASK)<<16)|((x)<<8)|(y))

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

#define IOC_WS2 0x08000000
#define _WSAIO(x,y)   (IOC_VOID|(x)|(y))
#define _WSAIOR(x,y)  (IOC_OUT|(x)|(y))
#define _WSAIOW(x,y)  (IOC_IN|(x)|(y))
#define _WSAIORW(x,y) (IOC_INOUT|(x)|(y))

/* Host to network byte order conversion */
#define htons(x) htons(x)
#define ntohs(x) ntohs(x)
#define htonl(x) htonl(x)
#define ntohl(x) ntohl(x)

/* Special addresses */
#define INADDR_ANY          ((uint32_t)0x00000000)
#define INADDR_LOOPBACK     ((uint32_t)0x7f000001)
#define INADDR_BROADCAST    ((uint32_t)0xffffffff)
#define INADDR_NONE         ((uint32_t)0xffffffff)

/* Error codes */
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
#define WSA_QOS_RECEIVERS       (WSABASEERR+1005)
#define WSA_QOS_SENDERS         (WSABASEERR+1006)
#define WSA_QOS_NO_SENDERS      (WSABASEERR+1007)
#define WSA_QOS_NO_RECEIVERS    (WSABASEERR+1008)
#define WSA_QOS_REQUEST_CONFIRMED (WSABASEERR+1009)
#define WSA_QOS_ADMISSION_FAILURE (WSABASEERR+1010)
#define WSA_QOS_POLICY_FAILURE  (WSABASEERR+1011)
#define WSA_QOS_BAD_STYLE       (WSABASEERR+1012)
#define WSA_QOS_BAD_OBJECT      (WSABASEERR+1013)
#define WSA_QOS_TRAFFIC_CTRL_ERROR (WSABASEERR+1014)
#define WSA_QOS_GENERIC_ERROR   (WSABASEERR+1015)
#define WSA_QOS_ESERVICETYPE    (WSABASEERR+1016)
#define WSA_QOS_EFLOWSPEC       (WSABASEERR+1017)
#define WSA_QOS_EPROVSPECBUF    (WSABASEERR+1018)
#define WSA_QOS_EFILTERSTYLE    (WSABASEERR+1019)
#define WSA_QOS_EFILTERTYPE     (WSABASEERR+1020)
#define WSA_QOS_EFILTERCOUNT    (WSABASEERR+1021)
#define WSA_QOS_EOBJLENGTH      (WSABASEERR+1022)
#define WSA_QOS_EFLOWCOUNT      (WSABASEERR+1023)
#define WSA_QOS_EUNKOWNPSOBJ    (WSABASEERR+1024)
#define WSA_QOS_EPOLICYOBJ      (WSABASEERR+1025)
#define WSA_QOS_EFLOWDESC       (WSABASEERR+1026)
#define WSA_QOS_EPSFLOWSPEC     (WSABASEERR+1027)
#define WSA_QOS_EPSFILTERSPEC   (WSABASEERR+1028)
#define WSA_QOS_ESDMODEOBJ      (WSABASEERR+1029)
#define WSA_QOS_ESHAPERATEOBJ   (WSABASEERR+1030)
#define WSA_QOS_RESERVED_PETYPE (WSABASEERR+1031)

/* h_errno equivalents */
#define HOST_NOT_FOUND          WSAHOST_NOT_FOUND
#define TRY_AGAIN               WSATRY_AGAIN
#define NO_RECOVERY             WSANO_RECOVERY
#define NO_DATA                 WSANO_DATA
#define WSANO_ADDRESS           WSANO_DATA
#define NO_ADDRESS              WSANO_ADDRESS

/* Events */
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
#define FD_ALL_EVENTS   0x3FF

/* FD_SET operations */
#define FD_SETSIZE      1024

typedef struct fd_set {
    unsigned int fd_count;
    SOCKET fd_array[FD_SETSIZE];
} fd_set;

#define FD_CLR(fd, set) do { \
    unsigned int __i; \
    for (__i = 0; __i < ((fd_set*)(set))->fd_count; __i++) { \
        if (((fd_set*)(set))->fd_array[__i] == fd) { \
            while (__i < ((fd_set*)(set))->fd_count - 1) { \
                ((fd_set*)(set))->fd_array[__i] = ((fd_set*)(set))->fd_array[__i+1]; \
                __i++; \
            } \
            ((fd_set*)(set))->fd_count--; \
            break; \
        } \
    } \
} while(0)

#define FD_SET(fd, set) do { \
    unsigned int __i; \
    for (__i = 0; __i < ((fd_set*)(set))->fd_count; __i++) { \
        if (((fd_set*)(set))->fd_array[__i] == (fd)) { \
            break; \
        } \
    } \
    if (__i == ((fd_set*)(set))->fd_count) { \
        if (((fd_set*)(set))->fd_count < FD_SETSIZE) { \
            ((fd_set*)(set))->fd_array[__i] = (fd); \
            ((fd_set*)(set))->fd_count++; \
        } \
    } \
} while(0)

#define FD_ZERO(set) (((fd_set*)(set))->fd_count = 0)

#define FD_ISSET(fd, set) __WSA_FD_ISSET((SOCKET)(fd), (fd_set*)(set))

int __WSA_FD_ISSET(SOCKET fd, fd_set* set);

/* timeval structure */
struct timeval {
    long tv_sec;
    long tv_usec;
};

/* linger structure */
struct linger {
    unsigned short l_onoff;
    unsigned short l_linger;
};

/* WSADATA structure */
typedef struct WSAData {
    WORD wVersion;
    WORD wHighVersion;
    unsigned short iMaxSockets;
    unsigned short iMaxUdpDg;
    char* lpVendorInfo;
    char szDescription[257];
    char szSystemStatus[129];
} WSADATA, *LPWSADATA;

/* hostent structure */
struct hostent {
    char* h_name;
    char** h_aliases;
    short h_addrtype;
    short h_length;
    char** h_addr_list;
};
#define h_addr h_addr_list[0]

/* servent structure */
struct servent {
    char* s_name;
    char** s_aliases;
    short s_port;
    char* s_proto;
};

/* protoent structure */
struct protoent {
    char* p_name;
    char** p_aliases;
    short p_proto;
};

/* QOS structures */
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

/* WSABUF structure */
typedef struct _WSABUF {
    unsigned long len;
    char* buf;
} WSABUF, *LPWSABUF;

/* WSAMSG structure */
typedef struct _WSAMSG {
    LPSOCKADDR name;
    INT namelen;
    LPWSABUF lpBuffers;
    DWORD dwBufferCount;
    WSABUF Control;
    DWORD dwFlags;
} WSAMSG, *PWSAMSG, *LPWSAMSG;

/* Overlapped structure */
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

typedef struct _WSAOVERLAPPED {
    ULONG_PTR Internal;
    ULONG_PTR InternalHigh;
    union {
        struct {
            DWORD Offset;
            DWORD OffsetHigh;
        };
        PVOID Pointer;
    };
    WSAEVENT hEvent;
} WSAOVERLAPPED, *LPWSAOVERLAPPED_REAL;

/* Completion routine */
typedef void (*LPWSAOVERLAPPED_COMPLETION_ROUTINE)(
    DWORD dwError,
    DWORD cbTransferred,
    LPWSAOVERLAPPED lpOverlapped,
    DWORD dwFlags
);

/* WSANETWORKEVENTS */
typedef struct _WSANETWORKEVENTS {
    long lNetworkEvents;
    int iErrorCode[FD_MAX_EVENTS];
} WSANETWORKEVENTS, *LPWSANETWORKEVENTS;

/* WSAPROTOCOL_INFO */
#define MAX_PROTOCOL_CHAIN 7
#define WSAPROTOCOL_LEN 255

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

/* Group/socket options */
#define SG_UNCONSTRAINED_GROUP   0x01
#define SG_CONSTRAINED_GROUP     0x02

/* Service flags */
#define XP1_CONNECTIONLESS           0x00000001
#define XP1_GUARANTEED_DELIVERY      0x00000002
#define XP1_GUARANTEED_ORDER         0x00000004
#define XP1_MESSAGE_ORIENTED         0x00000008
#define XP1_PSEUDO_STREAM            0x00000010
#define XP1_GRACEFUL_CLOSE           0x00000020
#define XP1_EXPEDITED_DATA           0x00000040
#define XP1_CONNECT_DATA             0x00000080
#define XP1_DISCONNECT_DATA          0x00000100
#define XP1_SUPPORT_BROADCAST        0x00000200
#define XP1_SUPPORT_MULTIPOINT       0x00000400
#define XP1_MULTIPOINT_CONTROL_PLANE 0x00000800
#define XP1_MULTIPOINT_DATA_PLANE    0x00001000
#define XP1_QOS_SUPPORTED            0x00002000
#define XP1_INTERRUPT                0x00004000
#define XP1_UNI_SEND                 0x00008000
#define XP1_UNI_RECV                 0x00010000
#define XP1_IFS_HANDLES              0x00020000
#define XP1_PARTIAL_MESSAGE          0x00040000

/* Provider flags */
#define PFL_MULTIPLE_PROTO_ENTRIES  0x00000001
#define PFL_RECOMMENDED_PROTO_ENTRY 0x00000002
#define PFL_HIDDEN                  0x00000004
#define PFL_MATCHES_PROTOCOL_ZERO   0x00000008

/* WSASocket flags */
#define WSA_FLAG_OVERLAPPED             0x01
#define WSA_FLAG_MULTIPOINT_C_ROOT      0x02
#define WSA_FLAG_MULTIPOINT_C_LEAF      0x04
#define WSA_FLAG_MULTIPOINT_D_ROOT      0x08
#define WSA_FLAG_MULTIPOINT_D_LEAF      0x10
#define WSA_FLAG_ACCESS_SYSTEM_SECURITY 0x40
#define WSA_FLAG_NO_HANDLE_INHERIT      0x80
#define WSA_FLAG_REGISTERED_IO          0x100

/* WSAIoctl options */
#define WSA_IO_PENDING          997
#define WSA_IO_INCOMPLETE       996
#define WSA_INVALID_HANDLE      6
#define WSA_INVALID_PARAMETER   87
#define WSA_NOT_ENOUGH_MEMORY   8
#define WSA_OPERATION_ABORTED   995

/* Core Winsock 1.1 API */
SOCKET WSAAPI socket(int af, int type, int protocol);
int WSAAPI bind(SOCKET s, const struct sockaddr* name, int namelen);
int WSAAPI listen(SOCKET s, int backlog);
SOCKET WSAAPI accept(SOCKET s, struct sockaddr* addr, int* addrlen);
int WSAAPI connect(SOCKET s, const struct sockaddr* name, int namelen);
int WSAAPI send(SOCKET s, const char* buf, int len, int flags);
int WSAAPI recv(SOCKET s, char* buf, int len, int flags);
int WSAAPI sendto(SOCKET s, const char* buf, int len, int flags,
                  const struct sockaddr* to, int tolen);
int WSAAPI recvfrom(SOCKET s, char* buf, int len, int flags,
                    struct sockaddr* from, int* fromlen);
int WSAAPI shutdown(SOCKET s, int how);
int WSAAPI closesocket(SOCKET s);
int WSAAPI getsockname(SOCKET s, struct sockaddr* name, int* namelen);
int WSAAPI getpeername(SOCKET s, struct sockaddr* name, int* namelen);
int WSAAPI getsockopt(SOCKET s, int level, int optname, char* optval, int* optlen);
int WSAAPI setsockopt(SOCKET s, int level, int optname, const char* optval, int optlen);
int WSAAPI ioctlsocket(SOCKET s, long cmd, unsigned long* argp);
int WSAAPI select(int nfds, fd_set* readfds, fd_set* writefds,
                  fd_set* exceptfds, const struct timeval* timeout);

/* Database functions */
struct hostent* WSAAPI gethostbyname(const char* name);
struct hostent* WSAAPI gethostbyaddr(const char* addr, int len, int type);
int WSAAPI gethostname(char* name, int namelen);
struct servent* WSAAPI getservbyname(const char* name, const char* proto);
struct servent* WSAAPI getservbyport(int port, const char* proto);
struct protoent* WSAAPI getprotobyname(const char* name);
struct protoent* WSAAPI getprotobynumber(int number);

/* Initialization */
int WSAAPI WSAStartup(WORD wVersionRequested, LPWSADATA lpWSAData);
int WSAAPI WSACleanup(void);
int WSAAPI WSAGetLastError(void);
void WSAAPI WSASetLastError(int iError);

/* Winsock 2 API */
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

SOCKET WSAAPI WSAAccept(SOCKET s, struct sockaddr* addr, int* addrlen,
                        void* lpfnCondition, DWORD_PTR dwCallbackData);

int WSAAPI WSAConnect(SOCKET s, const struct sockaddr* name, int namelen,
                      LPWSABUF lpCallerData, LPWSABUF lpCalleeData,
                      LPQOS lpSQOS, LPQOS lpGQOS);

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

/* Address string conversion */
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

/* Protocol info */
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

/* Duplicate socket */
int WSAAPI WSADuplicateSocketA(SOCKET s, DWORD dwProcessId,
                               LPWSAPROTOCOL_INFOA lpProtocolInfo);
int WSAAPI WSADuplicateSocketW(SOCKET s, DWORD dwProcessId,
                               LPWSAPROTOCOL_INFOW lpProtocolInfo);

#ifdef UNICODE
#define WSADuplicateSocket WSADuplicateSocketW
#else
#define WSADuplicateSocket WSADuplicateSocketA
#endif

/* Other WSA functions */
BOOL WSAAPI WSAGetOverlappedResult(SOCKET s, LPWSAOVERLAPPED lpOverlapped,
                                   DWORD* lpcbTransfer, BOOL fWait,
                                   DWORD* lpdwFlags);
int WSAAPI WSAHtonl(SOCKET s, unsigned long hostlong, unsigned long* lpnetlong);
int WSAAPI WSAHtons(SOCKET s, unsigned short hostshort, unsigned short* lpnetshort);
int WSAAPI WSANtohl(SOCKET s, unsigned long netlong, unsigned long* lphostlong);
int WSAAPI WSANtohs(SOCKET s, unsigned short netshort, unsigned short* lphostshort);

/* Socket pair (not in Windows but useful) */
int WSAAPI WSASocketPair(int af, int type, int protocol, SOCKET socks[2]);

/* Name service provider */
typedef struct _WSANAMESPACE_INFOA {
    GUID NSProviderId;
    DWORD dwNameSpace;
    BOOL fActive;
    DWORD dwVersion;
    LPSTR lpszIdentifier;
} WSANAMESPACE_INFOA, *PWSANAMESPACE_INFOA, *LPWSANAMESPACE_INFOA;

typedef struct _WSANAMESPACE_INFOW {
    GUID NSProviderId;
    DWORD dwNameSpace;
    BOOL fActive;
    DWORD dwVersion;
    LPWSTR lpszIdentifier;
} WSANAMESPACE_INFOW, *PWSANAMESPACE_INFOW, *LPWSANAMESPACE_INFOW;

#ifdef UNICODE
typedef WSANAMESPACE_INFOW WSANAMESPACE_INFO;
typedef PWSANAMESPACE_INFOW PWSANAMESPACE_INFO;
typedef LPWSANAMESPACE_INFOW LPWSANAMESPACE_INFO;
#else
typedef WSANAMESPACE_INFOA WSANAMESPACE_INFO;
typedef PWSANAMESPACE_INFOA PWSANAMESPACE_INFO;
typedef LPWSANAMESPACE_INFOA LPWSANAMESPACE_INFO;
#endif

/* Namespace definitions */
#define NS_ALL      0
#define NS_DNS      12
#define NS_NETBT    13
#define NS_WINS     14
#define NS_NLA      15

/* Resolution flags */
#define RES_UNUSED_1    0x00000001
#define RES_FLUSH_CACHE 0x00000002
#define RES_SERVICE     0x00000004

/* IP multicast request structures */
struct ip_mreq {
    struct in_addr imr_multiaddr;
    struct in_addr imr_interface;
};

struct ip_mreq_source {
    struct in_addr imr_multiaddr;
    struct in_addr imr_sourceaddr;
    struct in_addr imr_interface;
};

struct ipv6_mreq {
    struct in6_addr ipv6mr_multiaddr;
    unsigned int ipv6mr_interface;
};

/* Compatibility defines */
#define h_errno WSAGetLastError()

#ifdef __cplusplus
}
#endif

#endif /* __linux__ */

#endif /* _WINSOCK2_H */
