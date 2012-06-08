#include "livedatavector.h"

LiveDataVector *live_data_vector_new(void)
{
    LiveDataVector *obj = (LiveDataVector*)g_malloc(sizeof(LiveDataVector));
    obj->arr = g_ptr_array_new();
    return obj;
}

void live_data_vector_dispose(LiveDataVector *obj)
{
  size_t i;
  g_return_if_fail(obj != NULL);
  for (i = 0; i < obj->arr->len; i++)
    live_data_dispose(live_data_vector_index(obj, i));

  g_ptr_array_free(obj->arr, TRUE);
  g_free(obj);
}

LiveDataVector* live_data_vector_push_back(LiveDataVector *obj, LiveData *ld)
{
    g_return_val_if_fail(obj != NULL, NULL);
    live_data_set_index(ld, obj->arr->len);
    g_ptr_array_add(obj->arr, ld);
    return obj;
}

LiveData* live_data_vector_index(LiveDataVector *obj, size_t index)
{
    g_return_val_if_fail(obj != NULL, NULL);
    g_return_val_if_fail(index < obj->arr->len, NULL);
    return (LiveData*)g_ptr_array_index(obj->arr, index);
}

size_t live_data_vector_size(LiveDataVector *obj)
{
  g_return_val_if_fail(obj != NULL, 0);
  return obj->arr->len;
}
