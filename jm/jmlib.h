#ifndef __JM_LIB_H__
#define __JM_LIB_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <glib.h>
#include <jm/jmauth.h>
#include <jm/jmdb.h>
#include <jm/jmcommboxfactory.h>
#include <jm/jmcommboxport.h>
#include <jm/jmcanbus.h>
#include <jm/jmkwp1281.h>
#include <jm/jmkwp2000.h>
#include <jm/jmmikuni.h>

G_BEGIN_DECLS

GLIB_VAR void jm_lib_init(const gchar *software_path);
GLIB_VAR void jm_lib_destroy(void);
GLIB_VAR gboolean jm_load_vehicle_script(const gchar *name, const gchar *path, const gchar *db_name);
GLIB_VAR const gchar* jm_lib_vehicles_dir(void);

G_END_DECLS

#endif