#ifndef __JM_COMMBOX_H__
#define __JM_COMMBOX_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <glib.h>
#include <jm/jmprotocoltype.h>
#include <jm/jmconnector.h>
#include <jm/jmerror.h>
#include <jm/jmcommboxversion.h>
#include <jm/jmcomm.h>

G_BEGIN_DECLS

typedef struct _JMCommbox JMCommbox;

struct _JMCommbox {
	gint32 (*open)(JMCommbox *);
	gint32 (*close)(JMCommbox *);
	JMComm* (*configure)(JMCommbox *, JMProtocolType);
	gint32 (*set_connector)(JMCommbox *, JMConnector);
	/* this only for serial port configuration */
	gint32 (*serial_port_baud)(JMCommbox *);
	guint8 (*serial_port_databits)(JMCommbox *);
	gint32 (*serial_port_parity)(JMCommbox *);
	gint32 (*serial_port_stopbits)(JMCommbox *);
	gint32 (*serial_port_flow_control)(JMCommbox *);
	gboolean (*serial_port_change_config)(JMCommbox *);
	gboolean (*check_serial_port_change_config)(JMCommbox *);

	JMCommboxVersion version;
    JMComm *comm;
	gpointer user_data;
};

JMCommbox *jm_commbox_new(void);
void jm_commbox_free(JMCommbox *self);
gint32 jm_commbox_open(JMCommbox *self);
gint32 jm_commbox_close(JMCommbox *self);
JMComm* jm_commbox_configure(JMCommbox *self, JMProtocolType type);
gint32 jm_commbox_set_connector(JMCommbox *self, JMConnector);
gint32 jm_commbox_serial_port_baud(JMCommbox *self);
guint8 jm_commbox_serial_port_databits(JMCommbox *self);
gint32 jm_commbox_serial_port_parity(JMCommbox *self);
gint32 jm_commbox_serial_port_stopbits(JMCommbox *self);
gint32 jm_commbox_serial_port_flow_control(JMCommbox *self);
gboolean jm_commbox_serial_port_change_config(JMCommbox *self);
gboolean jm_commbox_check_serial_port_change_config(JMCommbox *self);

G_END_DECLS


#endif