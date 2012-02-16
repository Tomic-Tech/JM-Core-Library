#include "jmcommboxport.h"

static gint64 _jm_read_timeout;
static gint64 _jm_write_timeout;
static GQueue *_jm_in_deque = NULL;
static GQueue *_jm_out_deque = NULL;
static GMutex *_jm_in_mutex = NULL;
static GMutex *_jm_out_mutex = NULL;
static GCond *_jm_in_cond = NULL;

void jm_commbox_port_init(void) {
	_jm_read_timeout = 500000;
	_jm_write_timeout = 500000;
	_jm_in_deque = g_queue_new();
	_jm_out_deque = g_queue_new();
	_jm_in_mutex = g_mutex_new();
	_jm_out_mutex = g_mutex_new();
	_jm_in_cond = g_cond_new();
}

static void _jm_free_out_deque(gpointer data, gpointer user_data) {
	GByteArray *array = (GByteArray*)data;
	g_byte_array_free(array, TRUE);
}

void jm_commbox_port_destroy(void) {
	g_queue_free(_jm_in_deque);
	g_queue_foreach(_jm_out_deque, _jm_free_out_deque, NULL);
	g_queue_free(_jm_out_deque);
	g_mutex_free(_jm_in_mutex);
	g_mutex_free(_jm_out_mutex);
	g_cond_free(_jm_in_cond);
}

gint32 jm_commbox_port_set_read_timeout(gint64 microseconds) {
	_jm_read_timeout = microseconds;
	return JM_ERROR_SUCCESS;
}

gint32 jm_commbox_port_set_write_timeout(gint64 microseconds) {
	_jm_write_timeout = microseconds;
	return JM_ERROR_SUCCESS;
}

size_t jm_commbox_port_bytes_available(void) {
	return g_queue_get_length(_jm_in_deque);
}

gint32 jm_commbox_port_discard_out_buffer(void) {
	g_mutex_lock(_jm_out_mutex);
	g_queue_foreach(_jm_out_deque, _jm_free_out_deque, NULL);
	g_queue_clear(_jm_out_deque);
	g_mutex_unlock(_jm_out_mutex);
	return JM_ERROR_SUCCESS;
}

gint32 jm_commbox_port_discard_in_buffer(void) {
	g_mutex_lock(_jm_in_mutex);
	g_queue_clear(_jm_in_deque);
	g_mutex_unlock(_jm_in_mutex);
    return JM_ERROR_SUCCESS;
}

static size_t _jm_read_immediately(guint8 *data, size_t count) {
	size_t i;
	g_mutex_lock(_jm_in_mutex);
	for (i = 0; i < count; i++) {
		data[i] = (guint8)GPOINTER_TO_UINT(g_queue_pop_head(_jm_in_deque));
	}
	g_mutex_unlock(_jm_in_mutex);
	return count;
}

static size_t _jm_read_without_timeout(guint8 *data, size_t count) {
	size_t i;
	g_mutex_lock(_jm_in_mutex);
	while (g_queue_get_length(_jm_in_deque) < count) {
		g_cond_wait(_jm_in_cond, _jm_in_mutex);
	}
	for (i = 0; i < count; i++) {
		data[i] = (guint8)GPOINTER_TO_UINT(g_queue_pop_head(_jm_in_deque));
	}
	g_mutex_unlock(_jm_in_mutex);
	return count;
}

static size_t _jm_read_with_timeout(guint8 *data, size_t count) {
	size_t i;
	size_t len = 0;
	GTimeVal abs_time;

	g_mutex_lock(_jm_in_mutex);

	g_get_current_time(&abs_time);
	g_time_val_add(&abs_time, _jm_read_timeout);

	while (g_cond_timed_wait(_jm_in_cond, _jm_in_mutex, &abs_time)) {
		size_t size = g_queue_get_length(_jm_in_deque);
		if (size >= count) {
			for (i = 0; i < count; i++) {
				data[i + len] = (guint8)GPOINTER_TO_UINT(g_queue_pop_head(_jm_in_deque));
			}
			len += count;
			break;
		}
		for (i = 0; i < size; i++) {
			data[i + len] = (guint8)GPOINTER_TO_UINT(g_queue_pop_head(_jm_in_deque));
		}
		len += size;
		count -= size;
	}
	g_mutex_unlock(_jm_in_mutex);
	return len;
}

size_t jm_commbox_port_read(guint8 *data, size_t count) {
	if (count <= g_queue_get_length(_jm_in_deque))
		return _jm_read_immediately(data, count);

	if (_jm_read_timeout <= 0)
		return _jm_read_without_timeout(data, count);

	return _jm_read_with_timeout(data, count);
}

size_t jm_commbox_port_write(const guint8 *data, size_t count) {
	GByteArray *array = NULL;
	g_mutex_lock(_jm_out_mutex);
	array = g_byte_array_sized_new(count);
	array = g_byte_array_append(array, data, count);
    g_queue_push_tail(_jm_out_deque, array);
	g_mutex_unlock(_jm_out_mutex);
	return count;
}

void jm_commbox_port_push_in_deque(const guint8 *data, size_t count) {
	size_t i;
	g_mutex_lock(_jm_in_mutex);
	for (i = 0; i < count; i++) {
		g_queue_push_tail(_jm_in_deque, GUINT_TO_POINTER(data[i]));
	}
	g_cond_signal(_jm_in_cond);
	g_mutex_unlock(_jm_in_mutex);
}

gboolean jm_commbox_port_out_deque_available(void) {
	return !g_queue_is_empty(_jm_out_deque);
}

gboolean jm_commbox_port_pop_out_deque(GByteArray **data) {
	g_mutex_lock(_jm_out_mutex);
	if (g_queue_is_empty(_jm_out_deque))
		return FALSE;
	if (*data != NULL)
		g_byte_array_free(*data, TRUE);
	*data = (GByteArray*)g_queue_pop_head(_jm_out_deque);
	g_mutex_unlock(_jm_out_mutex);
	return TRUE;
}