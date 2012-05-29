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
gint32 live_data_cmd_id(LiveData *obj);

G_END_DECLS

#endif /* __JM_LIVEDATA_H__ */