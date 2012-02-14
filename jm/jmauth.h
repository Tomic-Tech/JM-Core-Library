#ifndef __JM_AUTH_H__
#define __JM_AUTH_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <glib.h>

G_BEGIN_DECLS

void jm_auth_init(void);
void jm_auth_destroy(void);
void jm_auth_set_dat_path(const gchar *path);
void jm_auth_write_register(const gchar *reg);
gchar* jm_auth_de(size_t index);
#define jm_auth_de_id_code() jm_auth_de(0)
#define jm_auth_de_commbox_id() jm_auth_de(1)
#define jm_auth_de_first_time() jm_auth_de(2)
#define jm_auth_de_expire_time() jm_auth_de(3)
#define jm_auth_de_db() jm_auth_de(4)
#define jm_auth_de_lang() jm_auth_de(5)

G_END_DECLS

#endif