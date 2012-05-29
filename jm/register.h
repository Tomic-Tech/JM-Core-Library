#ifndef __JM_REGISTER_H__
#define __JM_REGISTER_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <glib.h>

G_BEGIN_DECLS


typedef enum _DecryptType {
    IDCode = 0,
    CommboxID,
    RegisterTime,
    ExpireTime,
    DatabasePassword,
    Language,
    LogPassword,
    MessageLines
} DecryptType;

void register_init(const gchar *path);
void register_dispose(void);
void register_save(const char *path);
gboolean register_query_id_code(char *id_code);
gboolean register_check(void);
gboolean register_decrypt(DecryptType index, char *result);

G_END_DECLS

#endif /* __JM_REGISTER_H__ */