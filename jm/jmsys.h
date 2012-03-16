#ifndef __JM_SYS_H__
#define __JM_SYS_H__

#include <glib.h>

G_BEGIN_DECLS

void jm_sys_init(void);
void jm_sys_destroy(void);
void jm_sys_set_software_path(const gchar *software_path);
gchar* jm_sys_text(const gchar *name);

G_END_DECLS

#endif