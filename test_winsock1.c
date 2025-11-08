/*
 * test_winsock1.c - Test program for Winsock 1.1 API (wsock32)
 */

#include "winsock.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

void test_init_cleanup(void)
{
    WSADATA wsaData;
    int result;

    printf("\n[TEST] WSAStartup/WSACleanup (Winsock 1.1)\n");

    result = WSAStartup(MAKEWORD(1, 1), &wsaData);
    if (result != 0) {
        printf("  FAILED: WSAStartup returned %d\n", result);
        return;
    }

    printf("  SUCCESS: Winsock version %d.%d initialized\n",
           LOBYTE(wsaData.wVersion), HIBYTE(wsaData.wVersion));
    printf("  Description: %s\n", wsaData.szDescription);
    printf("  Status: %s\n", wsaData.szSystemStatus);

    result = WSACleanup();
    if (result != 0) {
        printf("  FAILED: WSACleanup returned %d\n", result);
        return;
    }

    printf("  SUCCESS: Winsock cleaned up\n");
}

void test_socket_creation(void)
{
    WSADATA wsaData;
    SOCKET sock;
    int result;

    printf("\n[TEST] socket creation and closesocket\n");

    WSAStartup(MAKEWORD(1, 1), &wsaData);

    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        printf("  FAILED: socket() returned INVALID_SOCKET, error: %d\n",
               WSAGetLastError());
        WSACleanup();
        return;
    }

    printf("  SUCCESS: TCP socket created (fd=%d)\n", (int)sock);

    result = closesocket(sock);
    if (result != 0) {
        printf("  FAILED: closesocket() returned %d, error: %d\n",
               result, WSAGetLastError());
        WSACleanup();
        return;
    }

    printf("  SUCCESS: Socket closed\n");

    WSACleanup();
}

void test_name_resolution(void)
{
    WSADATA wsaData;
    char hostname[256];
    struct hostent* host;
    struct in_addr addr;

    printf("\n[TEST] Name resolution (gethostbyname/gethostname)\n");

    WSAStartup(MAKEWORD(1, 1), &wsaData);

    if (gethostname(hostname, sizeof(hostname)) == 0) {
        printf("  SUCCESS: Hostname: %s\n", hostname);
    } else {
        printf("  WARNING: gethostname() failed\n");
    }

    host = gethostbyname("localhost");
    if (host != NULL) {
        printf("  SUCCESS: gethostbyname(\"localhost\") resolved:\n");
        printf("    Name: %s\n", host->h_name);
        if (host->h_addr_list[0] != NULL) {
            memcpy(&addr, host->h_addr_list[0], sizeof(addr));
            printf("    Address: %s\n", inet_ntoa(addr));
        }
    } else {
        printf("  WARNING: gethostbyname() failed\n");
    }

    WSACleanup();
}

void test_blocking_hooks(void)
{
    WSADATA wsaData;
    LPWSABLOCKINGHOOK old_hook;
    BOOL blocking;

    printf("\n[TEST] Blocking hook functions\n");

    WSAStartup(MAKEWORD(1, 1), &wsaData);

    blocking = WSAIsBlocking();
    printf("  Initial blocking state: %s\n", blocking ? "TRUE" : "FALSE");

    /* Note: WSASetBlockingHook returns default hook on first call */
    old_hook = WSASetBlockingHook((LPWSABLOCKINGHOOK)0x12345678);
    if (old_hook != NULL) {
        printf("  SUCCESS: WSASetBlockingHook() set new hook\n");
    } else {
        printf("  FAILED: WSASetBlockingHook() returned NULL\n");
    }

    if (WSAUnhookBlockingHook() == 0) {
        printf("  SUCCESS: WSAUnhookBlockingHook() succeeded\n");
    } else {
        printf("  FAILED: WSAUnhookBlockingHook() failed\n");
    }

    WSACleanup();
}

void test_async_service_lookup(void)
{
    WSADATA wsaData;
    HANDLE hAsync;
    char buffer[1024];

    printf("\n[TEST] Async service lookup\n");

    WSAStartup(MAKEWORD(1, 1), &wsaData);

    hAsync = WSAAsyncGetServByName((HANDLE)0, 0, "http", "tcp",
                                   buffer, sizeof(buffer));
    if (hAsync != NULL) {
        printf("  SUCCESS: WSAAsyncGetServByName() initiated\n");
        /* In a real app, we'd wait for the message */
    } else {
        printf("  WARNING: WSAAsyncGetServByName() failed, error: %d\n",
               WSAGetLastError());
    }

    hAsync = WSAAsyncGetProtoByName((HANDLE)0, 0, "tcp",
                                    buffer, sizeof(buffer));
    if (hAsync != NULL) {
        printf("  SUCCESS: WSAAsyncGetProtoByName() initiated\n");
    } else {
        printf("  WARNING: WSAAsyncGetProtoByName() failed, error: %d\n",
               WSAGetLastError());
    }

    WSACleanup();
}

void test_socket_options(void)
{
    WSADATA wsaData;
    SOCKET sock;
    int optval;
    int optlen;
    struct linger linger_opt;

    printf("\n[TEST] Socket options (getsockopt/setsockopt)\n");

    WSAStartup(MAKEWORD(1, 1), &wsaData);

    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        printf("  FAILED: socket() failed\n");
        WSACleanup();
        return;
    }

    /* Set SO_REUSEADDR */
    optval = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR,
                   (char*)&optval, sizeof(optval)) == 0) {
        printf("  SUCCESS: SO_REUSEADDR set\n");
    } else {
        printf("  WARNING: setsockopt(SO_REUSEADDR) failed\n");
    }

    /* Get socket type */
    optlen = sizeof(optval);
    if (getsockopt(sock, SOL_SOCKET, SO_TYPE,
                   (char*)&optval, (socklen_t*)&optlen) == 0) {
        printf("  SUCCESS: Socket type: %d (SOCK_STREAM=%d)\n",
               optval, SOCK_STREAM);
    } else {
        printf("  WARNING: getsockopt(SO_TYPE) failed\n");
    }

    /* Set linger option */
    linger_opt.l_onoff = 1;
    linger_opt.l_linger = 10;
    if (setsockopt(sock, SOL_SOCKET, SO_LINGER,
                   (char*)&linger_opt, sizeof(linger_opt)) == 0) {
        printf("  SUCCESS: SO_LINGER set (timeout: %d seconds)\n",
               linger_opt.l_linger);
    } else {
        printf("  WARNING: setsockopt(SO_LINGER) failed\n");
    }

    closesocket(sock);
    WSACleanup();
}

void test_ioctlsocket(void)
{
    WSADATA wsaData;
    SOCKET sock;
    unsigned long mode;

    printf("\n[TEST] ioctlsocket (non-blocking mode)\n");

    WSAStartup(MAKEWORD(1, 1), &wsaData);

    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        printf("  FAILED: socket() failed\n");
        WSACleanup();
        return;
    }

    /* Set non-blocking mode */
    mode = 1;
    if (ioctlsocket(sock, FIONBIO, &mode) == 0) {
        printf("  SUCCESS: Socket set to non-blocking mode\n");
    } else {
        printf("  FAILED: ioctlsocket(FIONBIO) failed, error: %d\n",
               WSAGetLastError());
    }

    /* Set back to blocking mode */
    mode = 0;
    if (ioctlsocket(sock, FIONBIO, &mode) == 0) {
        printf("  SUCCESS: Socket set back to blocking mode\n");
    } else {
        printf("  FAILED: ioctlsocket(FIONBIO) failed\n");
    }

    closesocket(sock);
    WSACleanup();
}

void test_select(void)
{
    WSADATA wsaData;
    SOCKET sock;
    fd_set write_fds;
    struct timeval timeout;
    int result;

    printf("\n[TEST] select() function\n");

    WSAStartup(MAKEWORD(1, 1), &wsaData);

    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        printf("  FAILED: socket() failed\n");
        WSACleanup();
        return;
    }

    FD_ZERO(&write_fds);
    FD_SET(sock, &write_fds);

    timeout.tv_sec = 0;
    timeout.tv_usec = 100000;  /* 100ms */

    result = select(sock + 1, NULL, &write_fds, NULL, &timeout);
    printf("  SUCCESS: select() returned: %d\n", result);

    if (FD_ISSET(sock, &write_fds)) {
        printf("  Socket is writable\n");
    }

    closesocket(sock);
    WSACleanup();
}

int main(void)
{
    printf("=======================================================\n");
    printf("Linux Winsock 1.1 (wsock32) Test Program\n");
    printf("=======================================================\n");

    test_init_cleanup();
    test_socket_creation();
    test_name_resolution();
    test_blocking_hooks();
    test_async_service_lookup();
    test_socket_options();
    test_ioctlsocket();
    test_select();

    printf("\n\n=======================================================\n");
    printf("All Winsock 1.1 tests completed!\n");
    printf("=======================================================\n");

    return 0;
}
