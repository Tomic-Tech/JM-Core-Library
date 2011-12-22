#ifndef JM_MACROS_HPP
#define JM_MACROS_HPP

#ifdef _MSC_VER
#pragma once
#endif

#if defined(__CYGWIN__)
#define JM_OS_CYGWIN
#elif defined(MSDOS) || defined(_MSDOS)
#define JM_OS_MSDOS
#elif !defined(SAG_COM) && (defined(WIN64) || defined(_WIN64) || defined(__WIN64__))
#define JM_OS_WIN32
#define JM_OS_WIN64
#elif !defined(SAG_COM) && (defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__))
#if defined(WINCE) || defined(_WIN32_WCE)
#define JM_OS_WINCE
#else
#define JM_OS_WIN32
#endif
#elif defined(__MWERKS__) && defined(__INTEL__)
#define JM_OS_WIN32
#elif defined(__linux__) || defined(__linux)
#define JM_OS_LINUX
#ifdef ANDROID
#define JM_OS_ANDROID
#endif
#endif

#if defined(JM_OS_WIN32) || defined(JM_OS_WIN64) || defined(JM_OS_WINCE)
#define JM_OS_WIN
#endif

#if defined(JM_OS_MSDOS) || defined(JM_OS_OS2) || defined(JM_OS_WIN)
#undef JM_OS_UNIX
#elif !defined(JM_OS_UNIX)
#define JM_OS_UNIX
#endif

// Here we provide JM_GNUC_EXTENSION as an alias for __extension__,
// where this is valid. This allows for warningless compilation of
// "long long" types even in the presence of '-ansi -pedantic'.
#if __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 8)
#define JM_GNUC_EXTENSION __extension__
#else
#define JM_GNUC_EXTENSION
#endif

#ifdef JMLIB_BUILD
#ifdef JM_OS_WIN
#define JMLIB_API __declspec(dllexport)
#elif defined(JM_OS_UNIX)
#define JMLIB_API
#endif
#else
#ifdef JM_OS_WIN
#define JMLIB_API __declspec(dllimport)
#elif defined(JM_OS_UNIX)
#define JMLIB_API extern
#endif
#endif

#endif