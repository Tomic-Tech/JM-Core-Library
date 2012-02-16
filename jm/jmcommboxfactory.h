#ifndef __JM_COMMBOX_FACTORY_H__
#define __JM_COMMBOX_FACTORY_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <glib.h>
#include <jm/jmcommboxversion.h>
#include <jm/jmcommbox.h>

G_BEGIN_DECLS

GLIB_VAR void jm_commbox_factory_init(void);
GLIB_VAR void jm_commbox_factory_destroy(void);
GLIB_VAR void jm_commbox_factory_set_commbox_version(JMCommboxVersion ver);
GLIB_VAR JMCommbox* jm_commbox_factory_create_commbox();

G_END_DECLS

#endif
