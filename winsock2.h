/*
 * Winsock2.h - Windows Socket API 2.2 for Linux
 * Compatible header for compiling Windows socket applications on Linux
 */

#ifndef _WINSOCK2API_
#define _WINSOCK2API_

#ifdef __linux__

/* Include our clean API definitions */
#include "winsock2_api.h"

/* Note: Standard POSIX socket functions are available directly:
 * - socket(), bind(), listen(), accept(), connect()
 * - send(), recv(), sendto(), recvfrom()
 * - getsockname(), getpeername()
 * - getsockopt(), setsockopt()
 * - shutdown(), select()
 * - gethostbyname(), gethostbyaddr(), gethostname()
 * - getservbyname(), getservbyport()
 * - getprotobyname(), getprotobynumber()
 *
 * Use them directly or through the WinSock API.
 */

#else
/* On Windows, include the real winsock2.h */
#include <winsock2.h>
#endif

#endif /* _WINSOCK2API_ */
