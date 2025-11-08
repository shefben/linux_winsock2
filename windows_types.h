/*
 * Windows Types for Linux
 * Basic Windows type definitions for socket API compatibility
 */

#ifndef _WINDOWS_TYPES_H
#define _WINDOWS_TYPES_H

#ifdef __linux__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Basic Windows types that don't conflict with Linux */
#ifndef _WINDEF_
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

typedef int INT;
typedef long LONG;
typedef unsigned long ULONG;
typedef int64_t LONGLONG;
typedef uint64_t ULONGLONG;

typedef union _LARGE_INTEGER {
    struct {
        DWORD LowPart;
        LONG HighPart;
    };
    struct {
        DWORD LowPart;
        LONG HighPart;
    } u;
    LONGLONG QuadPart;
} LARGE_INTEGER, *PLARGE_INTEGER;

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
#define CALLBACK

#endif /* _WINDEF_ */

#ifdef __cplusplus
}
#endif

#endif /* __linux__ */

#endif /* _WINDOWS_TYPES_H */
