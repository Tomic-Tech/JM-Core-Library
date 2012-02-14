#ifndef __JM_V1_BOX_H__
#define __JM_V1_BOX_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <glib.h>
#include "jmv1shared.h"
#include "jmv1boxw80d.h"
#include "jmv1boxc168d.h"

G_BEGIN_DECLS

typedef struct _JMV1Box JMV1Box;

#define JM_V1_BOX_C168 0
#define JM_V1_BOX_W80  1

#define JM_V1_BOX_C(box, name) ((box)->box_type == JM_V1_BOX_C168 ? JM_V1_BOX_C168_##name : JM_V1_BOX_W80_##name)

struct _JMV1Box {
    gboolean (*open_comm)(JMV1Box *self);
    gboolean (*close_comm)(JMV1Box *self);
    size_t (*read_bytes)(JMV1Box *self, guint8 *buff, size_t count);
    gboolean (*set_comm_ctrl)(JMV1Box *self, guint8 value_open, guint8 value_close);
    gboolean (*set_comm_line)(JMV1Box *self, guint8 send_line, guint8 recv_line);
    gboolean (*set_comm_link)(JMV1Box *self, guint8 ctrl_word1, guint8 ctrl_word2, guint8 ctrl_word3);
    gboolean (*set_comm_baud)(JMV1Box *self, gdouble baud);
    gboolean (*set_comm_time)(JMV1Box *self, guint8 type, guint32 time);
    gboolean (*set_line_level)(JMV1Box *self, guint8 value_low, guint8 value_high);
    gboolean (*commbox_delay)(JMV1Box *self, guint32 time);
    gboolean (*turn_over_one_by_one)(JMV1Box *self);
    gboolean (*stop_now)(JMV1Box *self, gboolean is_stop_execute);
    gboolean (*new_batch)(JMV1Box *self, guint8 buff_id);
    gboolean (*del_batch)(JMV1Box *self, guint8 buff_id);
    gboolean (*check_result)(JMV1Box *self, gint64 microseconds);
    gboolean (*keep_link)(JMV1Box *self, gboolean is_run_link);
    gboolean (*send_out_data)(JMV1Box *self, const guint8 *buff, size_t count);
    gboolean (*run_receive)(JMV1Box *self, guint8 type);
    gboolean (*end_batch)(JMV1Box *self);
    gboolean (*run_batch)(JMV1Box *self, guint8 *buff_id, size_t count, gboolean is_execute_many);
    size_t (*read_data)(JMV1Box *self, guint8 *buff, size_t count, gint64 microseconds);
    gboolean (*update_buff)(JMV1Box *self, guint8 type, guint8 *buff);
    guint8 (*get_abs_add)(JMV1Box *self, guint8 buff_id, guint8 add);
    gint32 (*serial_port_baud)(JMV1Box *self);
    guint8 (*serial_port_databits)(JMV1Box *self);
    gint32 (*serial_port_parity)(JMV1Box *self);
    gint32 (*serial_port_stopbits)(JMV1Box *self);
    gint32 (*serial_port_flow_control)(JMV1Box *self);
    gboolean (*serial_port_change_config)(JMV1Box *self);
    gboolean (*check_serial_port_change_config)(JMV1Box *self);
    
    guint32 box_type;
    JMV1Shared *shared;
    gpointer user_data;
};

JMV1Box* jm_v1_box_new(JMV1Shared *shared);
void jm_v1_box_free(JMV1Box *self);
gboolean jm_v1_box_open_comm(JMV1Box *self);
gboolean jm_v1_box_close_comm(JMV1Box *self);
size_t jm_v1_box_read_bytes(JMV1Box *self, guint8 *buff, size_t count);
gboolean jm_v1_box_set_comm_ctrl(JMV1Box *self, guint8 value_open, guint8 value_close);
gboolean jm_v1_box_set_comm_line(JMV1Box *self, guint8 send_line, guint8 recv_line);
gboolean jm_v1_box_set_comm_link(JMV1Box *self, guint8 ctrl_word1, guint8 ctrl_word2, guint8 ctrl_word3);
gboolean jm_v1_box_set_comm_baud(JMV1Box *self, gdouble baud);
gboolean jm_v1_box_set_line_level(JMV1Box *self, guint8 value_low, guint8 value_high);
gboolean jm_v1_box_commbox_delay(JMV1Box *self, guint32 time);
gboolean jm_v1_box_turn_over_one_by_one(JMV1Box *self);
gboolean jm_v1_box_stop_now(JMV1Box *self, gboolean is_stop_execute);
gboolean jm_v1_box_new_batch(JMV1Box *self, guint8 buff_id);
gboolean jm_v1_box_del_batch(JMV1Box *self, guint8 buff_id);
gboolean jm_v1_box_check_result(JMV1Box *self, gint64 microseconds);
gboolean jm_v1_box_keep_link(JMV1Box *self, gboolean is_run_link);
gboolean jm_v1_box_send_out_data(JMV1Box *self, const guint8 *buff, size_t count);
gboolean jm_v1_run_receive(JMV1Box *self, guint8 type);
gboolean jm_v1_box_end_batch(JMV1Box *self);
gboolean jm_v1_box_run_batch(JMV1Box *self, guint8 *buff_id, size_t count, gboolean is_execute_many);
size_t jm_v1_box_read_data(JMV1Box *self, guint8 *buff, size_t count, gint64 microseconds);
gboolean jm_v1_box_update_buff(JMV1Box *self, guint8 type, guint8 *buff);
guint8 jm_v1_box_get_abs_add(JMV1Box *self, guint8 buff_id, guint8 add);
gint32 jm_v1_box_serial_port_baud(JMV1Box *self);
guint8 jm_v1_box_serial_port_databits(JMV1Box *self);
gint32 jm_v1_box_serial_port_parity(JMV1Box *self);
gint32 jm_v1_box_serial_port_stopbits(JMV1Box *self);
gint32 jm_v1_box_serial_port_flow_control(JMV1Box *self);
gboolean jm_v1_box_serial_port_change_config(JMV1Box *self);
gboolean jm_v1_box_check_serial_port_change_config(JMV1Box *self);


G_END_DECLS

#endif