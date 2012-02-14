#include "jmldarray.h"

JMLDArray* jm_ld_array_new(void) {
    JMLDArray *lda = (JMLDArray*)g_malloc(sizeof(JMLDArray));

    lda->ld_array = g_ptr_array_new_full(0, jm_live_data_free);
    lda->current_enabled_index = -1;
    lda->enabled_index_array = g_ptr_array_new();
    lda->mutex = g_mutex_new();

    return lda;
}

void jm_ld_array_free(JMLDArray *self) {
    size_t i;
    g_return_if_fail(self != NULL);

    g_ptr_array_free(self->ld_array, TRUE);
    g_ptr_array_free(self->enabled_index_array, TRUE);
    g_mutex_free(self->mutex);
}

gint32 jm_ld_array_next_enabled_index(JMLDArray *self) {
    gint32 ret = 0;
    g_return_val_if_fail(self != NULL, -1);
    g_mutex_lock(self->mutex);

    if (self->enabled_index_array->len == 0) {
        g_mutex_unlock(self->mutex);
        return -1;
    }

    ret = GPOINTER_TO_INT(g_ptr_array_index(self->enabled_index_array, self->current_enabled_index));
    self->current_enabled_index++;
    if (self->current_enabled_index > (gint32)(self->enabled_index_array->len - 1))
        self->current_enabled_index = 0;

    g_mutex_unlock(self->mutex);
    return ret;
}

void jm_ld_array_generate_enabled_index(JMLDArray *self) {
    size_t i;
    g_return_if_fail(self != NULL);

    g_mutex_lock(self->mutex);

    g_ptr_array_set_size(self->enabled_index_array, 0);

    for (i = 0; i < self->ld_array->len; i++) {
        JMLiveData *ld = jm_ld_array_index(self, i);
        g_ptr_array_add(self->enabled_index_array, GINT_TO_POINTER(i));
    }
}
