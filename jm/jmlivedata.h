#ifndef __JM_LIVE_DATA_H__
#define __JM_LIVE_DATA_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <glib.h>

G_BEGIN_DECLS

typedef struct _JMLiveData JMLiveData;

struct _JMLiveData
{
    GString *short_name;
    GString *content;
    GString *unit;
    GString *default_value;
    GString *value;
    gint32 command_id;
    gboolean enabled; // If the value is FALSE, user can't choose this live data.
    gboolean show; // User can set this TRUE or FALSE to determine this live data show or not.
};

JMLiveData *jm_live_data_new(const gchar *short_name, 
    const gchar* content, 
    const gchar *unit, 
    const gchar *default_value, 
    gint32 command_id);
void jm_live_data_free(JMLiveData *self);

G_END_DECLS

#endif