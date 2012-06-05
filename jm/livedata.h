#ifndef __JM_LIVEDATA_H__
#define __JM_LIVEDATA_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <glib.h>

G_BEGIN_DECLS

typedef struct _LiveData LiveData;

struct _LiveData {
    GString *shortName;
    GString *content;
    GString *unit;
    GString *defaultValue;
    GString *value;
    gint32 cmdID;
    gboolean enabled;
    gboolean showed;
    gint32 index;
};

LiveData * live_data_new(const gchar *shortName,
    const gchar *content,
    const gchar *unit,
    const gchar *defaultValue,
    const gchar *value,
    gint32 cmdID);
void live_data_dispose(LiveData *obj);
const gchar* live_data_short_name(LiveData *obj);
const gchar* live_data_content(LiveData *obj);
const gchar* live_data_unit(LiveData *obj);
const gchar* live_data_default_value(LiveData *obj);
const gchar* live_data_value(LiveData *obj);
void live_data_set_value(LiveData *obj, const gchar *value);
gint32 live_data_cmd_id(LiveData *obj);
gboolean live_data_enabled(LiveData *obj);
void live_data_set_enabled(LiveData *obj, gboolean enabled);
gboolean live_data_showed(LiveData *obj);
void live_data_set_showed(LiveData *obj, gboolean showed);
gint32 live_data_index(LiveData *obj);
void live_data_set_index(LiveData *obj, gint32 index);

G_END_DECLS

#endif /* __JM_LIVEDATA_H__ */
