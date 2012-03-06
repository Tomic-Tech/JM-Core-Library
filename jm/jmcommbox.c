#include "jmcommbox.h"

static gint32 _jm_commbox_serial_port_baud(JMCommbox *self)
{
	return 0;
}

static guint8 _jm_commbox_serial_port_databits(JMCommbox *self)
{
	return 0;
}

static gint32 _jm_commbox_serial_port_parity(JMCommbox *self)
{
	return 0;
}

static gint32 _jm_commbox_serial_port_stopbits(JMCommbox *self)
{
	return 0;
}

static gint32 _jm_commbox_serial_port_flow_control(JMCommbox *self)
{
	return 0;
}

static gboolean _jm_commbox_serial_port_change_config(JMCommbox *self)
{
	return FALSE;
}

static gboolean _jm_commbox_check_serial_port_change_config(JMCommbox *self)
{
	return FALSE;
}

JMCommbox *jm_commbox_new(void)
{
	JMCommbox *obj = (JMCommbox*)g_malloc(sizeof(JMCommbox));
	obj->open = NULL;
	obj->close = NULL;
	obj->configure = NULL;
	obj->set_connector = NULL;
	obj->serial_port_baud = _jm_commbox_serial_port_baud;
	obj->serial_port_databits = _jm_commbox_serial_port_databits;
	obj->serial_port_parity = _jm_commbox_serial_port_parity;
	obj->serial_port_stopbits = _jm_commbox_serial_port_stopbits;
	obj->serial_port_flow_control = _jm_commbox_serial_port_flow_control;
	obj->serial_port_change_config  = _jm_commbox_serial_port_change_config;
	obj->check_serial_port_change_config = _jm_commbox_check_serial_port_change_config;
	obj->version = JM_COMMBOX_UNKNOW;
	obj->user_data = NULL;
	return obj;
}

void jm_commbox_free(JMCommbox *self)
{
	g_return_if_fail(self != NULL);
	g_free(self);
}

gint32 jm_commbox_open(JMCommbox *self)
{
	g_return_val_if_fail(self != NULL, JM_ERROR_GENERIC);
    g_return_val_if_fail(self->open != NULL, JM_ERROR_GENERIC);
	return self->open(self);
}

gint32 jm_commbox_close(JMCommbox *self)
{
	g_return_val_if_fail(self != NULL, JM_ERROR_GENERIC);
    g_return_val_if_fail(self->close != NULL, JM_ERROR_GENERIC);
	return self->close(self);
}

JMComm* jm_commbox_configure(JMCommbox *self, JMProtocolType type)
{
	g_return_val_if_fail(self != NULL, NULL);
    g_return_val_if_fail(self->configure != NULL, NULL);
	return self->configure(self, type);
}

gint32 jm_commbox_set_connector(JMCommbox *self, JMConnector cn)
{
	g_return_val_if_fail(self != NULL, JM_ERROR_GENERIC);
    g_return_val_if_fail(self->set_connector != NULL, JM_ERROR_GENERIC);
	return self->set_connector(self, cn);
}

gint32 jm_commbox_serial_port_baud(JMCommbox *self)
{
	g_return_val_if_fail(self != NULL, 0);
    g_return_val_if_fail(self->serial_port_baud != NULL, 0);
	return self->serial_port_baud(self);
}

guint8 jm_commbox_serial_port_databits(JMCommbox *self)
{
	g_return_val_if_fail(self != NULL, 0);
    g_return_val_if_fail(self->serial_port_databits != NULL, 0);
	return self->serial_port_databits(self);
}

gint32 jm_commbox_serial_port_parity(JMCommbox *self)
{
	g_return_val_if_fail(self != NULL, 0);
    g_return_val_if_fail(self->serial_port_parity != NULL, 0);
	return self->serial_port_parity(self);
}

gint32 jm_commbox_serial_port_stopbits(JMCommbox *self)
{
	g_return_val_if_fail(self != NULL, 0);
    g_return_val_if_fail(self->serial_port_stopbits != NULL, 0);
	return self->serial_port_stopbits(self);
}

gint32 jm_commbox_serial_port_flow_control(JMCommbox *self)
{
	g_return_val_if_fail(self != NULL, 0);
    g_return_val_if_fail(self->serial_port_flow_control != NULL, 0);
	return self->serial_port_flow_control(self);
}

gboolean jm_commbox_serial_port_change_config(JMCommbox *self)
{
	g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(self->serial_port_change_config != NULL, FALSE);
	return self->serial_port_change_config(self);
}

gboolean jm_commbox_check_serial_port_change_config(JMCommbox *self)
{
	g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(self->check_serial_port_change_config != NULL, FALSE);
	return self->check_serial_port_change_config(self);
}