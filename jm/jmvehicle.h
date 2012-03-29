#ifndef __JM_VEHICLE_H__
#define __JM_VEHICLE_H__

#include <jm/jmtype.h>

JM_BEGIN_DECLS

void jm_vehicle_init(void);
void jm_vehicle_destroy(void);
void jm_vehicle_set_path(const char *software_path);
boolean_t jm_vehicle_load_script(const char *name, const char *path, const char *db_name);
const char* jm_vehicles_dir(void);

JM_END_DECLS

#endif