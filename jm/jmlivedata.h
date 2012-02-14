#ifndef __JM_LIVE_DATA_H__
#define __JM_LIVE_DATA_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <glib.h>

G_BEGIN_DECLS

typedef struct _JMLiveData JMLiveData;

struct _JMLiveData {
    gchar *short_name;
    gchar *content;
    gchar *unit;
    gchar *default_value;
    gint32 command_id;
    gboolean enabled;
};

JMLiveData *jm_live_data_new(const gchar *short_name, 
    const gchar* content, 
    const gchar *unit, 
    const gchar *default_value, 
    gint32 command_id, 
    gboolean enabled);
void jm_live_data_free(JMLiveData *self);

G_END_DECLS

#endif