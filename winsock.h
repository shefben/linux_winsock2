/*
 * winsock.h - Windows Socket API 1.1 for Linux
 * Compatible header for compiling Winsock 1.1 applications on Linux
 *
 * This header provides Winsock 1.1 compatibility by including Winsock 2.2
 * which is backward compatible with Winsock 1.1
 */

#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_

#ifdef __linux__

/* Include Winsock2 - it's backward compatible with Winsock 1.1 */
#include "winsock2.h"

/* Winsock 1.1 used MAKEWORD(1,1) */
#define WINSOCK1_VERSION MAKEWORD(1,1)

#else
/* On Windows, include the real winsock.h */
#include <winsock.h>
#endif

#endif /* _WINSOCKAPI_ */
