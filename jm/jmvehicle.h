#ifndef __JM_VEHICLE_H__
#define __JM_VEHICLE_H__

#include <glib.h>

G_BEGIN_DECLS

void jm_vehicle_init(void);
void jm_vehicle_destroy(void);
void jm_vehicle_set_path(const gchar *software_path);
gboolean jm_vehicle_load_script(const gchar *name, const gchar *path, const gchar *db_name);
const gchar* jm_vehicles_dir(void);

G_END_DECLS

#endif