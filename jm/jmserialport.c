#include "jmserialportprivate.h"

static void _jm_serial_port_initialize(JMSerialPort *self) {
	self->port_name = NULL;
	self->read_timeout = 500;
	self->write_timeout = 500;
	self->parity = JM_SP_PAR_NONE;
	self->stopbits = JM_SP_SB_ONE;
	self->flow_control = JM_SP_FC_NONE;
	self->baudrate = 57600;
	self->databits = 8;
	self->is_multi_setting = FALSE;
    self->mutex = (GStaticRecMutex *)g_malloc(sizeof(GStaticRecMutex));
    g_static_rec_mutex_init(self->mutex);
}

JMSerialPort* jm_serial_port_new(void) {
	JMSerialPort *obj = (JMSerialPort*)g_malloc(sizeof(JMSerialPort));
	_jm_serial_port_initialize(obj);
	_jm_serial_port_platform_initialize(obj);
	return obj;
}

JMSerialPort* jm_serial_port_new_with_name(const gchar *name) {
	JMSerialPort *obj = jm_serial_port_new();
	jm_serial_port_set_port_name(obj, name);
	return obj;
}

void jm_serial_port_free(JMSerialPort *self) {
	g_return_if_fail(self != NULL);

	if (!jm_serial_port_is_open(self)) {
		jm_serial_port_close(self);
	}

	_jm_serial_port_platform_destroy(self);
	g_free(self->port_name);
    g_static_rec_mutex_free(self->mutex);
    g_free(self->mutex);
	g_free(self);
}

const gchar* jm_serial_port_get_port_name(JMSerialPort *self) {
	g_return_val_if_fail(self, NULL);
	return self->port_name;
}

gint32 jm_serial_port_get_baudrate(JMSerialPort *self) {
	g_return_val_if_fail(self, 0);
	return self->baudrate;
}

guint8 jm_serial_port_get_databits(JMSerialPort *self) {
	g_return_val_if_fail(self, 0);
	return self->databits;
}

gint32 jm_serial_port_get_stopbits(JMSerialPort *self) {
	g_return_val_if_fail(self, 0);
	return self->stopbits;
}

gint32 jm_serial_port_get_parity(JMSerialPort *self) {
	g_return_val_if_fail(self, 0);
	return self->parity;
}

gint32 jm_serial_port_get_flow_control(JMSerialPort *self) {
	g_return_val_if_fail(self, 0);
	return self->flow_control;
}

gint64 jm_serial_port_get_read_timeout(JMSerialPort *self) {
	g_return_val_if_fail(self, 0);
	return self->read_timeout;
}

gint64 jm_serial_port_get_write_timeout(JMSerialPort *self) {
	g_return_val_if_fail(self, 0);
	return self->write_timeout;
}
