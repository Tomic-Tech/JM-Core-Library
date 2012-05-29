#ifndef __JM_LIVEDATAVECTOR_H__
#define __JM_LIVEDATAVECTOR_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <glib.h>
#include <jm/livedata.h>

G_BEGIN_DECLS
    
typedef struct _LiveDataVector LiveDataVector;

struct _LiveDataVector {
    GPtrArray *arr;
};

LiveDataVector * live_data_vector_new(void);
void live_data_vector_dispose(LiveDataVector *obj);
LiveDataVector* live_data_vector_push_back(LiveDataVector *obj, LiveData *ld);
LiveData* live_data_vector_index(LiveDataVector *obj, size_t index);
    
G_END_DECLS

#endif /* __JM_LIVEDATAVECTOR_H__ */