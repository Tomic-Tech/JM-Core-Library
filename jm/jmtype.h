#ifndef __JM_TYPE_H__
#define __JM_TYPE_H__

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
#define JM_BEGIN_DECLS extern "C" {
#define JM_END_DECLS }
#else
#define JM_BEGIN_DECLS
#define JM_END_DECLS
#endif

typedef int32_t boolean_t;
typedef void* pointer_t;

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE (!FALSE)
#endif

#endif