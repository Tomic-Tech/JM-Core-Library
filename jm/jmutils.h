#ifndef __JM_UTILS_H__
#define __JM_UTILS_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <glib.h>

G_BEGIN_DECLS

#define JM_HIGH_BYTE(value) (guint8)(((guint16)(value) >> 8) & 0xFF)
#define JM_LOW_BYTE(value) (guint8)((guint16)(value) & 0xFF)
#define JM_HIGH_WORD(value) (guint16)(((guint32)(value) >> 16) & 0xFFF)
#define JM_LOW_WORD(value) (guint16)((guint32)(value) & 0xFFFF)

G_END_DECLS

#endif