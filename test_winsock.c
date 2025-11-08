/*
 * Winsock2 Linux Wrapper Test Program
 * Demonstrates usage of all major Winsock2 API functions
 */

#include "winsock2.h"
#include "ws2tcpip.h"
#include <stdio.h>
#include <string.h>

/* Test function declarations */
void test_initialization(void);
void test_socket_creation(void);
void test_address_conversion(void);
void test_name_resolution(void);
void test_server_client(void);
void test_select(void);
void test_socket_options(void);

int main(void)
{
    printf("=======================================================\n");
    printf("Linux Winsock2 Wrapper Test Program\n");
    printf("=======================================================\n\n");

    test_initialization();
    test_socket_creation();
    test_address_conversion();
    test_name_resolution();
    test_socket_options();
    test_select();
    test_server_client();

    printf("\n=======================================================\n");
    printf("All tests completed!\n");
    printf("=======================================================\n");

    return 0;
}

/* Test WSAStartup and WSACleanup */
void test_initialization(void)
{
    WSADATA wsaData;
    int result;

    printf("[TEST] WSAStartup/WSACleanup\n");

    result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        printf("  FAILED: WSAStartup failed with error: %d\n", result);
        return;
    }

    printf("  SUCCESS: Winsock version %d.%d initialized\n",
           LOBYTE(wsaData.wVersion), HIBYTE(wsaData.wVersion));
    printf("  Description: %s\n", wsaData.szDescription);
    printf("  Status: %s\n", wsaData.szSystemStatus);

    result = WSACleanup();
    if (result != 0) {
        printf("  FAILED: WSACleanup failed with error: %d\n", WSAGetLastError());
        return;
    }

    printf("  SUCCESS: Winsock cleaned up\n\n");

    /* Re-initialize for other tests */
    WSAStartup(MAKEWORD(2, 2), &wsaData);
}

/* Test socket creation and closure */
void test_socket_creation(void)
{
    SOCKET sock;

    printf("[TEST] socket creation and closesocket\n");

    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        printf("  FAILED: socket() returned INVALID_SOCKET, error: %d\n",
               WSAGetLastError());
        return;
    }

    printf("  SUCCESS: TCP socket created (fd=%d)\n", (int)sock);

    if (closesocket(sock) == SOCKET_ERROR) {
        printf("  FAILED: closesocket() failed, error: %d\n", WSAGetLastError());
        return;
    }

    printf("  SUCCESS: Socket closed\n\n");
}

/* Test address conversion functions */
void test_address_conversion(void)
{
    struct sockaddr_in addr;
    char ip_str[INET_ADDRSTRLEN];
    int result;

    printf("[TEST] Address conversion (inet_pton/inet_ntop)\n");

    /* Test inet_pton */
    result = inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
    if (result != 1) {
        printf("  FAILED: inet_pton() failed\n");
        return;
    }

    printf("  SUCCESS: inet_pton(\"127.0.0.1\") converted successfully\n");

    /* Test inet_ntop */
    if (inet_ntop(AF_INET, &addr.sin_addr, ip_str, sizeof(ip_str)) == NULL) {
        printf("  FAILED: inet_ntop() failed\n");
        return;
    }

    printf("  SUCCESS: inet_ntop() returned: %s\n\n", ip_str);
}

/* Test name resolution */
void test_name_resolution(void)
{
    struct addrinfo hints;
    struct addrinfo *result;
    struct addrinfo *ptr;
    char hostname[256];
    int ret;

    printf("[TEST] Name resolution (getaddrinfo/gethostname)\n");

    /* Test gethostname */
    ret = gethostname(hostname, sizeof(hostname));
    if (ret == SOCKET_ERROR) {
        printf("  WARNING: gethostname() failed, error: %d\n", WSAGetLastError());
    } else {
        printf("  SUCCESS: Hostname: %s\n", hostname);
    }

    /* Test getaddrinfo */
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    ret = getaddrinfo("localhost", "80", &hints, &result);
    if (ret != 0) {
        printf("  WARNING: getaddrinfo() failed, error: %d\n", ret);
        return;
    }

    printf("  SUCCESS: getaddrinfo() resolved localhost:\n");

    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
        char addrstr[INET6_ADDRSTRLEN];
        void *addr_ptr;

        if (ptr->ai_family == AF_INET) {
            struct sockaddr_in *ipv4;
            ipv4 = (struct sockaddr_in *)ptr->ai_addr;
            addr_ptr = &(ipv4->sin_addr);
            inet_ntop(AF_INET, addr_ptr, addrstr, sizeof(addrstr));
            printf("    IPv4: %s\n", addrstr);
        } else if (ptr->ai_family == AF_INET6) {
            struct sockaddr_in6 *ipv6;
            ipv6 = (struct sockaddr_in6 *)ptr->ai_addr;
            addr_ptr = &(ipv6->sin6_addr);
            inet_ntop(AF_INET6, addr_ptr, addrstr, sizeof(addrstr));
            printf("    IPv6: %s\n", addrstr);
        }
    }

    freeaddrinfo(result);
    printf("\n");
}

/* Test socket options */
void test_socket_options(void)
{
    SOCKET sock;
    int optval;
    int optlen;
    struct linger linger_opt;

    printf("[TEST] Socket options (getsockopt/setsockopt)\n");

    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        printf("  FAILED: Could not create socket\n");
        return;
    }

    /* Test SO_REUSEADDR */
    optval = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR,
                   (char *)&optval, sizeof(optval)) == SOCKET_ERROR) {
        printf("  WARNING: setsockopt(SO_REUSEADDR) failed, error: %d\n",
               WSAGetLastError());
    } else {
        printf("  SUCCESS: SO_REUSEADDR set\n");
    }

    /* Test getting socket type */
    optlen = sizeof(optval);
    if (getsockopt(sock, SOL_SOCKET, SO_TYPE,
                   (char *)&optval, &optlen) == SOCKET_ERROR) {
        printf("  WARNING: getsockopt(SO_TYPE) failed, error: %d\n",
               WSAGetLastError());
    } else {
        printf("  SUCCESS: Socket type: %d (SOCK_STREAM=%d)\n",
               optval, SOCK_STREAM);
    }

    /* Test SO_LINGER */
    linger_opt.l_onoff = 1;
    linger_opt.l_linger = 10;
    if (setsockopt(sock, SOL_SOCKET, SO_LINGER,
                   (char *)&linger_opt, sizeof(linger_opt)) == SOCKET_ERROR) {
        printf("  WARNING: setsockopt(SO_LINGER) failed, error: %d\n",
               WSAGetLastError());
    } else {
        printf("  SUCCESS: SO_LINGER set (timeout: %d seconds)\n",
               linger_opt.l_linger);
    }

    closesocket(sock);
    printf("\n");
}

/* Test select function */
void test_select(void)
{
    SOCKET sock;
    fd_set readfds;
    fd_set writefds;
    struct timeval timeout;
    int result;

    printf("[TEST] select() function\n");

    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        printf("  FAILED: Could not create socket\n");
        return;
    }

    FD_ZERO(&readfds);
    FD_ZERO(&writefds);
    FD_SET(sock, &writefds);

    timeout.tv_sec = 0;
    timeout.tv_usec = 100000; /* 100ms */

    result = select(0, &readfds, &writefds, NULL, &timeout);
    if (result == SOCKET_ERROR) {
        printf("  WARNING: select() failed, error: %d\n", WSAGetLastError());
    } else {
        printf("  SUCCESS: select() returned: %d\n", result);
        if (FD_ISSET(sock, &writefds)) {
            printf("  Socket is writable\n");
        }
    }

    closesocket(sock);
    printf("\n");
}

/* Test basic server/client functionality */
void test_server_client(void)
{
    SOCKET listen_sock;
    SOCKET client_sock;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    int addr_len;
    char buffer[256];
    int result;
    int optval;

    printf("[TEST] Basic server/client communication\n");

    /* Create listening socket */
    listen_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listen_sock == INVALID_SOCKET) {
        printf("  FAILED: Could not create listening socket\n");
        return;
    }

    /* Set SO_REUSEADDR */
    optval = 1;
    setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR,
               (char *)&optval, sizeof(optval));

    /* Bind to localhost */
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    server_addr.sin_port = htons(12345);

    if (bind(listen_sock, (struct sockaddr *)&server_addr,
             sizeof(server_addr)) == SOCKET_ERROR) {
        printf("  FAILED: bind() failed, error: %d\n", WSAGetLastError());
        closesocket(listen_sock);
        return;
    }

    printf("  SUCCESS: Socket bound to 127.0.0.1:12345\n");

    /* Listen */
    if (listen(listen_sock, SOMAXCONN) == SOCKET_ERROR) {
        printf("  FAILED: listen() failed, error: %d\n", WSAGetLastError());
        closesocket(listen_sock);
        return;
    }

    printf("  SUCCESS: Socket listening\n");

    /* Create client socket */
    client_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (client_sock == INVALID_SOCKET) {
        printf("  FAILED: Could not create client socket\n");
        closesocket(listen_sock);
        return;
    }

    /* Set client socket to non-blocking for quick test */
    {
        unsigned long mode;
        mode = 1;
        ioctlsocket(client_sock, FIONBIO, &mode);
    }

    /* Try to connect (will be non-blocking) */
    result = connect(client_sock, (struct sockaddr *)&server_addr,
                    sizeof(server_addr));
    if (result == SOCKET_ERROR && WSAGetLastError() != WSAEWOULDBLOCK &&
        WSAGetLastError() != WSAEINPROGRESS) {
        printf("  WARNING: connect() failed unexpectedly, error: %d\n",
               WSAGetLastError());
    } else {
        printf("  SUCCESS: Connection initiated\n");
    }

    /* Test getsockname */
    addr_len = sizeof(client_addr);
    if (getsockname(listen_sock, (struct sockaddr *)&client_addr,
                    &addr_len) == 0) {
        char addr_str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, addr_str, sizeof(addr_str));
        printf("  SUCCESS: getsockname() - %s:%d\n",
               addr_str, ntohs(client_addr.sin_port));
    }

    /* Clean up */
    closesocket(client_sock);
    closesocket(listen_sock);

    printf("  Test completed\n\n");
}
