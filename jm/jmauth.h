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
GLIB_VAR void jm_auth_save_reg(const gchar *reg);
GLIB_VAR gchar* jm_auth_query_id_code(void);
gchar* jm_auth_decrypt(size_t index);
#define jm_auth_decrypt_id_code() jm_auth_decrypt(0)
#define jm_auth_decrypt_commbox_id() jm_auth_decrypt(1)
#define jm_auth_decrypt_reg_time() jm_auth_decrypt(2)
#define jm_auth_decrypt_expire_time() jm_auth_decrypt(3)
#define jm_auth_decrypt_db_pw() jm_auth_decrypt(4)
#define jm_auth_decrypt_lang() jm_auth_decrypt(5)
GLIB_VAR gboolean jm_auth_check_reg(void);

G_END_DECLS

#endif