#ifndef __JM_MACROS_H__
#define __JM_MACROS_H__

#ifdef _MSC_VER
#pragma once
#endif

#if defined(__CYGWIN__)
#define OS_CYGWIN
#elif defined(MSDOS) || defined(_MSDOS)
#define OS_MSDOS
#elif !defined(SAG_COM) && (defined(WIN64) || defined(_WIN64) || defined(__WIN64__))
#define OS_WIN32
#define OS_WIN64
#elif !defined(SAG_COM) && (defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__))
#if defined(WINCE) || defined(_WIN32_WCE)
#define OS_WINCE
#else
#define OS_WIN32
#endif
#elif defined(__MWERKS__) && defined(__INTEL__)
#define OS_WIN32
#elif defined(__linux__) || defined(__linux)
#define OS_LINUX
#ifdef ANDROID
#define OS_ANDROID
#endif
#endif

#if defined(OS_WIN32) || defined(OS_WIN64) || defined(OS_WINCE)
#define OS_WIN
#endif

#if defined(OS_MSDOS) || defined(OS_OS2) || defined(OS_WIN)
#undef OS_UNIX
#elif !defined(OS_UNIX)
#define OS_UNIX
#endif


#endif /* __JM_MACROS_H__ */