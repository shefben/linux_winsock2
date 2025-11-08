/*
 * Address Conversion and Name Resolution Functions
 * Implements getaddrinfo, getnameinfo, inet_pton, inet_ntop, etc.
 */

#ifdef __linux__

#include "winsock2_api.h"
#include "ws2tcpip.h"
#include <wchar.h>
#include <locale.h>
#include <iconv.h>

extern __thread int g_wsa_last_error;

/* IPv6 address constants */
const struct in6_addr in6addr_any = IN6ADDR_ANY_INIT;
const struct in6_addr in6addr_loopback = IN6ADDR_LOOPBACK_INIT;

/* Note: inet_pton and inet_ntop are available as POSIX functions */

int WSAAPI InetPtonA(int Family, const char* pszAddrString, void* pAddrBuf)
{
    return inet_pton(Family, pszAddrString, pAddrBuf);
}

int WSAAPI InetPtonW(int Family, const wchar_t* pszAddrString, void* pAddrBuf)
{
    char buffer[INET6_ADDRSTRLEN];
    size_t len;

    /* Convert wide string to multibyte */
    len = wcstombs(buffer, pszAddrString, sizeof(buffer));
    if (len == (size_t)-1) {
        g_wsa_last_error = WSAEINVAL;
        return -1;
    }

    return inet_pton(Family, buffer, pAddrBuf);
}

const char* WSAAPI InetNtopA(int Family, const void* pAddr, char* pStringBuf,
                             size_t StringBufSize)
{
    return inet_ntop(Family, pAddr, pStringBuf, StringBufSize);
}

const wchar_t* WSAAPI InetNtopW(int Family, const void* pAddr, wchar_t* pStringBuf,
                                size_t StringBufSize)
{
    char buffer[INET6_ADDRSTRLEN];
    const char* result;

    result = inet_ntop(Family, pAddr, buffer, sizeof(buffer));
    if (result == NULL) {
        return NULL;
    }

    /* Convert multibyte to wide string */
    if (mbstowcs(pStringBuf, buffer, StringBufSize) == (size_t)-1) {
        g_wsa_last_error = WSAEINVAL;
        return NULL;
    }

    return pStringBuf;
}

/* Note: getaddrinfo and freeaddrinfo are available as POSIX functions */

int WSAAPI GetAddrInfoA(const char* pNodeName, const char* pServiceName,
                        const ADDRINFOA* pHints, ADDRINFOA** ppResult)
{
    return getaddrinfo(pNodeName, pServiceName, pHints, ppResult);
}

int WSAAPI GetAddrInfoW(const wchar_t* pNodeName, const wchar_t* pServiceName,
                        const ADDRINFOW* pHints, ADDRINFOW** ppResult)
{
    char node_buffer[256];
    char service_buffer[256];
    char* node_ptr;
    char* service_ptr;
    struct addrinfo hints;
    struct addrinfo* result;
    ADDRINFOW* wresult;
    ADDRINFOW* wcurrent;
    struct addrinfo* current;
    int ret;
    size_t len;

    node_ptr = NULL;
    service_ptr = NULL;

    /* Convert wide strings to multibyte */
    if (pNodeName != NULL) {
        len = wcstombs(node_buffer, pNodeName, sizeof(node_buffer));
        if (len == (size_t)-1) {
            g_wsa_last_error = WSAEINVAL;
            return WSAEINVAL;
        }
        node_ptr = node_buffer;
    }

    if (pServiceName != NULL) {
        len = wcstombs(service_buffer, pServiceName, sizeof(service_buffer));
        if (len == (size_t)-1) {
            g_wsa_last_error = WSAEINVAL;
            return WSAEINVAL;
        }
        service_ptr = service_buffer;
    }

    /* Convert hints */
    if (pHints != NULL) {
        hints.ai_flags = pHints->ai_flags;
        hints.ai_family = pHints->ai_family;
        hints.ai_socktype = pHints->ai_socktype;
        hints.ai_protocol = pHints->ai_protocol;
        hints.ai_addrlen = 0;
        hints.ai_addr = NULL;
        hints.ai_canonname = NULL;
        hints.ai_next = NULL;
    }

    ret = getaddrinfo(node_ptr, service_ptr, pHints != NULL ? &hints : NULL, &result);

    if (ret != 0) {
        return ret;
    }

    /* Convert result to wide character version */
    wresult = NULL;
    wcurrent = NULL;

    for (current = result; current != NULL; current = current->ai_next) {
        ADDRINFOW* witem;
        witem = (ADDRINFOW*)malloc(sizeof(ADDRINFOW));
        if (witem == NULL) {
            /* Clean up */
            while (wresult != NULL) {
                ADDRINFOW* next_item;
                next_item = wresult->ai_next;
                if (wresult->ai_addr != NULL) free(wresult->ai_addr);
                if (wresult->ai_canonname != NULL) free(wresult->ai_canonname);
                free(wresult);
                wresult = next_item;
            }
            freeaddrinfo(result);
            g_wsa_last_error = WSA_NOT_ENOUGH_MEMORY;
            return WSA_NOT_ENOUGH_MEMORY;
        }

        witem->ai_flags = current->ai_flags;
        witem->ai_family = current->ai_family;
        witem->ai_socktype = current->ai_socktype;
        witem->ai_protocol = current->ai_protocol;
        witem->ai_addrlen = current->ai_addrlen;

        /* Copy address */
        if (current->ai_addr != NULL) {
            witem->ai_addr = (struct sockaddr*)malloc(current->ai_addrlen);
            if (witem->ai_addr != NULL) {
                memcpy(witem->ai_addr, current->ai_addr, current->ai_addrlen);
            }
        } else {
            witem->ai_addr = NULL;
        }

        /* Convert canonical name to wide */
        if (current->ai_canonname != NULL) {
            size_t canon_len;
            canon_len = strlen(current->ai_canonname) + 1;
            witem->ai_canonname = (wchar_t*)malloc(canon_len * sizeof(wchar_t));
            if (witem->ai_canonname != NULL) {
                mbstowcs(witem->ai_canonname, current->ai_canonname, canon_len);
            }
        } else {
            witem->ai_canonname = NULL;
        }

        witem->ai_next = NULL;

        if (wresult == NULL) {
            wresult = witem;
            wcurrent = witem;
        } else {
            wcurrent->ai_next = witem;
            wcurrent = witem;
        }
    }

    freeaddrinfo(result);
    *ppResult = wresult;

    g_wsa_last_error = 0;
    return 0;
}

void WSAAPI FreeAddrInfoA(ADDRINFOA* pAddrInfo)
{
    freeaddrinfo(pAddrInfo);
}

void WSAAPI FreeAddrInfoW(ADDRINFOW* pAddrInfo)
{
    ADDRINFOW* current;
    ADDRINFOW* next_item;

    current = pAddrInfo;
    while (current != NULL) {
        next_item = current->ai_next;
        if (current->ai_addr != NULL) {
            free(current->ai_addr);
        }
        if (current->ai_canonname != NULL) {
            free(current->ai_canonname);
        }
        free(current);
        current = next_item;
    }
}

/* Note: getnameinfo is available as a POSIX function */

int WSAAPI GetNameInfoA(const SOCKADDR* pSockaddr, int SockaddrLength,
                        char* pNodeBuffer, DWORD NodeBufferSize,
                        char* pServiceBuffer, DWORD ServiceBufferSize,
                        INT Flags)
{
    return getnameinfo(pSockaddr, SockaddrLength, pNodeBuffer, NodeBufferSize,
                      pServiceBuffer, ServiceBufferSize, Flags);
}

int WSAAPI GetNameInfoW(const SOCKADDR* pSockaddr, int SockaddrLength,
                        wchar_t* pNodeBuffer, DWORD NodeBufferSize,
                        wchar_t* pServiceBuffer, DWORD ServiceBufferSize,
                        INT Flags)
{
    char node_buffer[NI_MAXHOST];
    char service_buffer[NI_MAXSERV];
    int result;

    result = getnameinfo(pSockaddr, SockaddrLength,
                        pNodeBuffer != NULL ? node_buffer : NULL, NI_MAXHOST,
                        pServiceBuffer != NULL ? service_buffer : NULL, NI_MAXSERV,
                        Flags);

    if (result != 0) {
        return result;
    }

    /* Convert to wide strings */
    if (pNodeBuffer != NULL) {
        if (mbstowcs(pNodeBuffer, node_buffer, NodeBufferSize) == (size_t)-1) {
            g_wsa_last_error = WSAEINVAL;
            return WSAEINVAL;
        }
    }

    if (pServiceBuffer != NULL) {
        if (mbstowcs(pServiceBuffer, service_buffer, ServiceBufferSize) == (size_t)-1) {
            g_wsa_last_error = WSAEINVAL;
            return WSAEINVAL;
        }
    }

    g_wsa_last_error = 0;
    return 0;
}

/* ============================================================================
 * WSAAddressToString / WSAStringToAddress Functions
 * ============================================================================ */

int WSAAPI WSAAddressToStringA(LPSOCKADDR lpsaAddress, DWORD dwAddressLength,
                               LPWSAPROTOCOL_INFOA lpProtocolInfo,
                               LPSTR lpszAddressString,
                               DWORD* lpdwAddressStringLength)
{
    char buffer[INET6_ADDRSTRLEN + 16]; /* Address + port */
    int af;
    const char* result;
    size_t len;

    (void)lpProtocolInfo;

    if (lpsaAddress == NULL || lpszAddressString == NULL || lpdwAddressStringLength == NULL) {
        g_wsa_last_error = WSAEFAULT;
        return SOCKET_ERROR;
    }

    af = lpsaAddress->sa_family;

    if (af == AF_INET) {
        struct sockaddr_in* addr;
        addr = (struct sockaddr_in*)lpsaAddress;
        result = inet_ntop(AF_INET, &addr->sin_addr, buffer, sizeof(buffer));
        if (result != NULL) {
            len = strlen(buffer);
            snprintf(buffer + len, sizeof(buffer) - len, ":%d", ntohs(addr->sin_port));
        }
    } else if (af == AF_INET6) {
        struct sockaddr_in6* addr6;
        addr6 = (struct sockaddr_in6*)lpsaAddress;
        result = inet_ntop(AF_INET6, &addr6->sin6_addr, buffer, sizeof(buffer));
        if (result != NULL) {
            len = strlen(buffer);
            snprintf(buffer + len, sizeof(buffer) - len, ":%d", ntohs(addr6->sin6_port));
        }
    } else {
        g_wsa_last_error = WSAEAFNOSUPPORT;
        return SOCKET_ERROR;
    }

    if (result == NULL) {
        g_wsa_last_error = WSAEFAULT;
        return SOCKET_ERROR;
    }

    len = strlen(buffer) + 1;
    if (*lpdwAddressStringLength < len) {
        *lpdwAddressStringLength = (DWORD)len;
        g_wsa_last_error = WSAEFAULT;
        return SOCKET_ERROR;
    }

    strcpy(lpszAddressString, buffer);
    *lpdwAddressStringLength = (DWORD)len;

    g_wsa_last_error = 0;
    return 0;
}

int WSAAPI WSAAddressToStringW(LPSOCKADDR lpsaAddress, DWORD dwAddressLength,
                               LPWSAPROTOCOL_INFOW lpProtocolInfo,
                               LPWSTR lpszAddressString,
                               DWORD* lpdwAddressStringLength)
{
    char buffer[INET6_ADDRSTRLEN + 16];
    LPSTR temp_string;
    DWORD temp_length;
    int result;

    temp_length = sizeof(buffer);
    temp_string = buffer;

    result = WSAAddressToStringA(lpsaAddress, dwAddressLength,
                                 (LPWSAPROTOCOL_INFOA)lpProtocolInfo,
                                 temp_string, &temp_length);

    if (result != 0) {
        return result;
    }

    /* Convert to wide string */
    if (mbstowcs(lpszAddressString, buffer, *lpdwAddressStringLength) == (size_t)-1) {
        g_wsa_last_error = WSAEINVAL;
        return SOCKET_ERROR;
    }

    *lpdwAddressStringLength = (DWORD)(wcslen(lpszAddressString) + 1);

    g_wsa_last_error = 0;
    return 0;
}

int WSAAPI WSAStringToAddressA(LPSTR AddressString, INT AddressFamily,
                               LPWSAPROTOCOL_INFOA lpProtocolInfo,
                               LPSOCKADDR lpAddress, INT* lpAddressLength)
{
    char* colon_pos;
    char addr_only[INET6_ADDRSTRLEN];
    int port;
    int result;

    (void)lpProtocolInfo;

    if (AddressString == NULL || lpAddress == NULL || lpAddressLength == NULL) {
        g_wsa_last_error = WSAEFAULT;
        return SOCKET_ERROR;
    }

    /* Find port separator */
    colon_pos = strrchr(AddressString, ':');
    port = 0;

    if (colon_pos != NULL) {
        size_t addr_len;
        addr_len = colon_pos - AddressString;
        if (addr_len >= sizeof(addr_only)) {
            g_wsa_last_error = WSAEINVAL;
            return SOCKET_ERROR;
        }
        memcpy(addr_only, AddressString, addr_len);
        addr_only[addr_len] = '\0';
        port = atoi(colon_pos + 1);
    } else {
        strncpy(addr_only, AddressString, sizeof(addr_only) - 1);
        addr_only[sizeof(addr_only) - 1] = '\0';
    }

    if (AddressFamily == AF_INET) {
        struct sockaddr_in* addr;
        if (*lpAddressLength < (INT)sizeof(struct sockaddr_in)) {
            *lpAddressLength = (INT)sizeof(struct sockaddr_in);
            g_wsa_last_error = WSAEFAULT;
            return SOCKET_ERROR;
        }
        addr = (struct sockaddr_in*)lpAddress;
        memset(addr, 0, sizeof(*addr));
        addr->sin_family = AF_INET;
        addr->sin_port = htons((uint16_t)port);
        result = inet_pton(AF_INET, addr_only, &addr->sin_addr);
        *lpAddressLength = (INT)sizeof(struct sockaddr_in);
    } else if (AddressFamily == AF_INET6) {
        struct sockaddr_in6* addr6;
        if (*lpAddressLength < (INT)sizeof(struct sockaddr_in6)) {
            *lpAddressLength = (INT)sizeof(struct sockaddr_in6);
            g_wsa_last_error = WSAEFAULT;
            return SOCKET_ERROR;
        }
        addr6 = (struct sockaddr_in6*)lpAddress;
        memset(addr6, 0, sizeof(*addr6));
        addr6->sin6_family = AF_INET6;
        addr6->sin6_port = htons((uint16_t)port);
        result = inet_pton(AF_INET6, addr_only, &addr6->sin6_addr);
        *lpAddressLength = (INT)sizeof(struct sockaddr_in6);
    } else {
        g_wsa_last_error = WSAEAFNOSUPPORT;
        return SOCKET_ERROR;
    }

    if (result <= 0) {
        g_wsa_last_error = WSAEINVAL;
        return SOCKET_ERROR;
    }

    g_wsa_last_error = 0;
    return 0;
}

int WSAAPI WSAStringToAddressW(LPWSTR AddressString, INT AddressFamily,
                               LPWSAPROTOCOL_INFOW lpProtocolInfo,
                               LPSOCKADDR lpAddress, INT* lpAddressLength)
{
    char buffer[INET6_ADDRSTRLEN + 16];

    /* Convert to multibyte */
    if (wcstombs(buffer, AddressString, sizeof(buffer)) == (size_t)-1) {
        g_wsa_last_error = WSAEINVAL;
        return SOCKET_ERROR;
    }

    return WSAStringToAddressA(buffer, AddressFamily,
                              (LPWSAPROTOCOL_INFOA)lpProtocolInfo,
                              lpAddress, lpAddressLength);
}

/* ============================================================================
 * Protocol Enumeration Functions
 * ============================================================================ */

int WSAAPI WSAEnumProtocolsA(INT* lpiProtocols,
                             LPWSAPROTOCOL_INFOA lpProtocolBuffer,
                             DWORD* lpdwBufferLength)
{
    (void)lpiProtocols;
    (void)lpProtocolBuffer;

    /* Return required buffer size for common protocols */
    if (lpdwBufferLength != NULL) {
        *lpdwBufferLength = 10 * sizeof(WSAPROTOCOL_INFOA);
    }

    g_wsa_last_error = WSAENOBUFS;
    return SOCKET_ERROR;
}

int WSAAPI WSAEnumProtocolsW(INT* lpiProtocols,
                             LPWSAPROTOCOL_INFOW lpProtocolBuffer,
                             DWORD* lpdwBufferLength)
{
    (void)lpiProtocols;
    (void)lpProtocolBuffer;

    if (lpdwBufferLength != NULL) {
        *lpdwBufferLength = 10 * sizeof(WSAPROTOCOL_INFOW);
    }

    g_wsa_last_error = WSAENOBUFS;
    return SOCKET_ERROR;
}

/* ============================================================================
 * Duplicate Socket Functions
 * ============================================================================ */

int WSAAPI WSADuplicateSocketA(SOCKET s, DWORD dwProcessId,
                               LPWSAPROTOCOL_INFOA lpProtocolInfo)
{
    (void)s;
    (void)dwProcessId;
    (void)lpProtocolInfo;

    /* Socket duplication across processes not supported */
    g_wsa_last_error = WSAEINVAL;
    return SOCKET_ERROR;
}

int WSAAPI WSADuplicateSocketW(SOCKET s, DWORD dwProcessId,
                               LPWSAPROTOCOL_INFOW lpProtocolInfo)
{
    (void)s;
    (void)dwProcessId;
    (void)lpProtocolInfo;

    g_wsa_last_error = WSAEINVAL;
    return SOCKET_ERROR;
}

/* Note: if_indextoname and if_nametoindex are available as POSIX functions */

#endif /* __linux__ */
