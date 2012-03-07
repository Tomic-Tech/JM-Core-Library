#include <string.h>
#include "jmv1box.h"


JMV1Box* jm_v1_box_new(JMV1Shared *shared)
{
    JMV1Box *obj = (JMV1Box*)g_malloc(sizeof(JMV1Box));
    memset(obj, 0, sizeof(JMV1Box));

    obj->box_type = -1;
    obj->shared = shared;

    return obj;
}

void jm_v1_box_free(JMV1Box *self)
{
    g_return_if_fail(self != NULL);

    g_free(self);
}

gboolean jm_v1_box_open_comm(JMV1Box *self)
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(self->open_comm != NULL, FALSE);

    return self->open_comm(self);
}

gboolean jm_v1_box_close_comm(JMV1Box *self)
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(self->close_comm != NULL, FALSE);

    return self->close_comm(self);
}

size_t jm_v1_box_read_bytes(JMV1Box *self, guint8 *buff, size_t count)
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(self->read_bytes != NULL, FALSE);

    return self->read_bytes(self, buff, count);
}

gboolean jm_v1_box_set_comm_ctrl(JMV1Box *self, guint8 value_open, 
    guint8 value_close)
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(self->set_comm_ctrl != NULL, FALSE);

    return self->set_comm_ctrl(self, value_open, value_close);
}

gboolean jm_v1_box_set_comm_line(JMV1Box *self, guint8 send_line, 
    guint8 recv_line)
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(self->set_comm_line != NULL, FALSE);

    return self->set_comm_line(self, send_line, recv_line);
}

gboolean jm_v1_box_set_comm_link(JMV1Box *self, guint8 ctrl_word1, 
    guint8 ctrl_word2, guint8 ctrl_word3)
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(self->set_comm_link != NULL, FALSE);

    return self->set_comm_link(self, ctrl_word1, ctrl_word2, ctrl_word3);
}

gboolean jm_v1_box_set_comm_baud(JMV1Box *self, gdouble baud)
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(self->set_comm_baud != NULL, FALSE);

    return self->set_comm_baud(self, baud);
}

gboolean jm_v1_box_set_line_level(JMV1Box *self, guint8 value_low, 
    guint8 value_high)
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(self->set_line_level != NULL, FALSE);

    return self->set_line_level(self, value_low, value_high);
}

gboolean jm_v1_box_commbox_delay(JMV1Box *self, guint32 time)
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(self->commbox_delay != NULL, FALSE);

    return self->commbox_delay(self, time);
}

gboolean jm_v1_box_turn_over_one_by_one(JMV1Box *self)
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(self->turn_over_one_by_one != NULL, FALSE);

    return self->turn_over_one_by_one(self);
}

gboolean jm_v1_box_stop_now(JMV1Box *self, gboolean is_stop_execute)
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(self->stop_now != NULL, FALSE);

    return self->stop_now(self, is_stop_execute);
}

gboolean jm_v1_box_new_batch(JMV1Box *self, guint8 buff_id)
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(self->new_batch != NULL, FALSE);

    return self->new_batch(self, buff_id);
}

gboolean jm_v1_box_del_batch(JMV1Box *self, guint8 buff_id)
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(self->del_batch != NULL, FALSE);

    return self->del_batch(self, buff_id);
}

gboolean jm_v1_box_check_result(JMV1Box *self, gint64 microseconds)
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(self->check_result != NULL, FALSE);

    return self->check_result(self, microseconds);
}

gboolean jm_v1_box_keep_link(JMV1Box *self, gboolean is_run_link)
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(self->keep_link != NULL, FALSE);

    return self->keep_link(self, is_run_link);
}

gboolean jm_v1_box_send_out_data(JMV1Box *self, const guint8 *buff, 
    size_t count)
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(self->send_out_data != NULL, FALSE);

    return self->send_out_data(self, buff, count);
}

gboolean jm_v1_run_receive(JMV1Box *self, guint8 type)
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(self->run_receive != NULL, FALSE);

    return self->run_receive(self, type);
}

gboolean jm_v1_box_end_batch(JMV1Box *self)
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(self->end_batch != NULL, FALSE);

    return self->end_batch(self);
}

gboolean jm_v1_box_run_batch(JMV1Box *self, guint8 *buff_id, size_t count, 
    gboolean is_execute_many)
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(self->run_batch != NULL, FALSE);

    return self->run_batch(self, buff_id, count, is_execute_many);
}

size_t jm_v1_box_read_data(JMV1Box *self, guint8 *buff, size_t count, 
    gint64 microseconds)
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(self->read_data != NULL, FALSE);

    return self->read_data(self, buff, count, microseconds);
}

gboolean jm_v1_box_update_buff(JMV1Box *self, guint8 type, guint8 *buff)
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(self->update_buff != NULL, FALSE);

    return self->update_buff(self, type, buff);
}

guint8 jm_v1_box_get_abs_add(JMV1Box *self, guint8 buff_id, guint8 add)
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(self->get_abs_add != NULL, FALSE);

    return self->get_abs_add(self, buff_id, add);
}

gint32 jm_v1_box_serial_port_baud(JMV1Box *self)
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(self->serial_port_baud != NULL, FALSE);

    return self->serial_port_baud(self);
}

guint8 jm_v1_box_serial_port_databits(JMV1Box *self)
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(self->serial_port_databits != NULL, FALSE);

    return self->serial_port_databits(self);
}

gint32 jm_v1_box_serial_port_parity(JMV1Box *self)
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(self->serial_port_parity != NULL, FALSE);

    return self->serial_port_parity(self);
}

gint32 jm_v1_box_serial_port_flow_control(JMV1Box *self)
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(self->serial_port_flow_control != NULL, FALSE);

    return self->serial_port_flow_control(self);
}

gboolean jm_v1_box_serial_port_change_config(JMV1Box *self)
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(self->serial_port_change_config != NULL, FALSE);

    return self->serial_port_change_config(self);
}

gboolean jm_v1_box_check_serial_port_change_config(JMV1Box *self)
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(self->serial_port_change_config != NULL, FALSE);

    return self->serial_port_change_config(self);
}
