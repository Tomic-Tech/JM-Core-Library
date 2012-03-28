#ifndef __JM_LIB_H__
#define __JM_LIB_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <glib.h>
#include <jm/jmerror.h>
#include <jm/jmutils.h>
#include <jm/jmcommboxversion.h>
#include <jm/jmcommboxporttype.h>
#include <jm/jmprotocoltype.h>
#include <jm/jmconnector.h>
#include <jm/jmcanbusbaud.h>
#include <jm/jmcanbusfiltermask.h>
#include <jm/jmcanbusframetype.h>
#include <jm/jmcanbusidmode.h>
#include <jm/jmkwpmode.h>
#include <jm/jmui.h>
#include <jm/jmvehicle.h>

G_BEGIN_DECLS

/************************************************************************/
/* Auth                                                                     */
/************************************************************************/
void jm_auth_set_dat_path(const gchar *path);
void jm_auth_save_reg(const gchar *reg);
gchar* jm_auth_query_id_code(void);
gchar* jm_auth_decrypt_id_code();
gchar* jm_auth_decrypt_commbox_id();
gchar* jm_auth_decrypt_reg_time();
gchar* jm_auth_decrypt_expire_time();
gchar* jm_auth_decrypt_db_pw();
gchar* jm_auth_decrypt_lang();
gchar* jm_auth_decrypt_log_pw();
gchar* jm_auth_encrypt_log_text(const gchar *log);
gboolean jm_auth_check_reg(void);

/************************************************************************/
/* Message Log                                                                     */
/************************************************************************/
void jm_log_write(const gchar *tag, const gchar *msg);
void jm_log_write_hex(const gchar *tag, const guint8 *data, size_t count);

/************************************************************************/
/* Database                                                             */
/************************************************************************/
gboolean jm_db_open(const gchar* file_path, const gchar* password);
gboolean jm_db_close(void);
gboolean jm_db_is_open(void);
void jm_db_set_tc_catalog(const gchar *catalog);
void jm_db_set_ld_catalog(const gchar *catalog);
void jm_db_set_cmd_catalog(const gchar *catalog);
gchar *jm_db_get_text(const gchar *name);
gchar *jm_db_get_trouble_code(const gchar *code);
GByteArray *jm_db_get_command(const gchar *name);
GByteArray *jm_db_get_command_by_id(gint32 id);

/************************************************************************/
/* System                                                               */
/************************************************************************/
void jm_sys_set_software_path(const gchar *software_path);
gchar* jm_sys_text(const gchar *name);

/************************************************************************/
/* Commbox Link                                                         */
/************************************************************************/
void jm_link_set_handler(gpointer handle);
JMProtocolType jm_link_protocol_type(void);
gpointer jm_link_get_protocol(void);
size_t jm_link_send_one_frame(const guint8 *data, size_t count);
size_t jm_link_send_frames(const guint8 *data, size_t count);
size_t jm_link_read_one_frame(guint8 *data, size_t max_length);
size_t jm_link_read_frames(guint8 *data, size_t max_length);
size_t jm_link_send_and_recv(const guint8 *send, size_t send_count, guint8 *recv, size_t max_length);
gint32 jm_link_start_keep_link(gboolean run);
gint32 jm_link_set_keep_link(const guint8 *data, size_t count);
gint32 jm_link_set_timeout(gint64 tx_b2b, gint64 rx_b2b, gint64 tx_f2f, gint64 rx_f2f, gint64 total);

/************************************************************************/
/* Commbox                                                              */
/************************************************************************/
void jm_commbox_set_handler(gpointer handle);
gint32 jm_commbox_open(void);
gint32 jm_commbox_close(void);
gint32 jm_commbox_configure(JMProtocolType type);
gint32 jm_commbox_set_connector(JMConnector cnn);

/************************************************************************/
/* Commbox Port                                                         */
/************************************************************************/
// If we use SerialPort (or other need) we will need a pointer to operate.
void jm_commbox_port_set_pointer(gpointer p);
gpointer jm_commbox_port_get_pointer(void);
void jm_commbox_port_set_type(JMCommboxPortType type);
JMCommboxPortType jm_commbox_port_get_type(void);
gint32 jm_commbox_port_set_read_timeout(gint64 microseconds);
gint32 jm_commbox_port_set_write_timeout(gint64 microseconds);
size_t jm_commbox_port_bytes_available(void);
gint32 jm_commbox_port_discard_out_buffer(void);
gint32 jm_commbox_port_discard_in_buffer(void);
size_t jm_commbox_port_read(guint8 *data, size_t count);
size_t jm_commbox_port_write(const guint8 *data, size_t count);

void jm_commbox_port_push_in_deque(const guint8 *data, size_t count);
gboolean jm_commbox_port_out_deque_available(void);
gboolean jm_commbox_port_pop_out_deque(guint8 *data, size_t count);

/************************************************************************/
/* Commbox Factory                                                      */
/************************************************************************/
void jm_commbox_factory_set_commbox_version(JMCommboxVersion ver);

/************************************************************************/
/* Canbus                                                               */
/************************************************************************/
void jm_canbus_set_handler(gpointer handle);
size_t jm_canbus_pack(const guint8 *src, size_t src_length, guint8 *tar, size_t tar_length);
size_t jm_canbus_unpack(const guint8 *src, size_t src_length, guint8 *tar, size_t tar_length);
gint32 jm_canbus_set_lines(gint32 high, gint32 low);
gint32 jm_canbus_set_filter(const gint32 *id_array, size_t count);
gint32 jm_canbus_set_options(gint32 id, JMCanbusBaud baud, JMCanbusIDMode id_mode, JMCanbusFilterMask mask, JMCanbusFrameType frame);

/************************************************************************/
/* KWP2000                                                              */
/************************************************************************/
void jm_kwp2000_set_handler(gpointer handle);
size_t jm_kwp2000_pack(const guint8 *src, size_t src_length, guint8 *tar, size_t tar_length);
size_t jm_kwp2000_unpack(const guint8 *src, size_t src_length, guint8 *tar, size_t tar_length);
gint32 jm_kwp2000_addr_init(guint8 addr_code);
gint32 jm_kwp2000_fast_init(const guint8 *data, size_t count);
gint32 jm_kwp2000_set_lines(gint32 com_line, gboolean l_line);
gint32 jm_kwp2000_set_options(JMKWPMode msg_mode, JMKWPMode link_mode, gint32 baud);
gint32 jm_kwp2000_set_address(guint8 target, guint8 source);


/************************************************************************/
/* KWP1281                                                              */
/************************************************************************/
void jm_kwp1281_set_handler(gpointer handle);
size_t jm_kwp1281_pack(const guint8 *src, size_t src_length, guint8 *tar, size_t tar_length);
size_t jm_kwp1281_unpack(const guint8 *src, size_t src_length, guint8 *tar, size_t tar_length);
gint32 jm_kwp1281_addr_init(guint8 addr_code);
gint32 jm_kwp1281_set_lines(gint32 com_line, gboolean l_line);

/************************************************************************/
/* Mikuni Protocol                                                      */
/************************************************************************/
void jm_mikuni_set_handler(gpointer handle);
size_t jm_mikuni_pack(const guint8 *src, size_t src_length, guint8 *tar, size_t tar_length);
size_t jm_mikuni_unpack(const guint8 *src, size_t src_length, guint8 *tar, size_t tar_length);
gint32 jm_mikuni_init(void);

/************************************************************************/
/* User Interface                                                       */
/************************************************************************/
void jm_ui_msg_free(JMUIMessage *obj);
void jm_ui_main_show(void);
void jm_ui_msg_box_btn_clr(void);
void jm_ui_msg_box_add_btn(const gchar *text);
void jm_ui_msg_box_set_msg(const gchar *text);
void jm_ui_msg_box_set_title(const gchar *text);
void jm_ui_msg_box_show(void);
void jm_ui_msg_box_hide(void);
void jm_ui_list_box_btn_clr(void);
void jm_ui_list_box_add_btn(const gchar *text);
void jm_ui_list_box_add_item(const gchar *caption, const gchar *item);
void jm_ui_list_box_item_clr(void);
void jm_ui_list_box_show(void);
void jm_ui_list_box_hide(void);
void jm_ui_menu_item_clr(void);
void jm_ui_menu_add_item(const gchar *text);
void jm_ui_menu_show(void);
void jm_ui_tc_item_clr(void);
void jm_ui_tc_add_item(const gchar *code, const gchar *text);
void jm_ui_tc_add_btn(const gchar *text);
void jm_ui_tc_btn_clr(void);
void jm_ui_tc_show(void);
void jm_ui_ld_prepare_show(void);
void jm_ui_ld_show(void);
void jm_ui_ld_btn_clr(void);
void jm_ui_ld_add_btn(const gchar *text);
void jm_ui_ld_set_value(gint32 index, const gchar *value);
gchar* jm_ui_get_btn_clicked(gboolean is_blocked);
void jm_ui_set_btn_clicked(const gchar *text);
gchar* jm_ui_get_menu_selected(void);
void jm_ui_set_menu_selected(const gchar *text);
JMUIMessage* jm_ui_pop_msg(void);
size_t jm_ui_msg_count(void);

/************************************************************************/
/* Live Data Array                                                      */
/************************************************************************/
void jm_ld_array_update_global_array(gboolean showed);
gint32 jm_ld_array_next_showed_index(void);
gint32 jm_ld_array_get_showed_index(gint32 index);
gint32 jm_ld_array_query_showed_position(gint32 index);
gint32 jm_ld_array_get_enabled_index(gint32 index);
void jm_ld_array_deploy_enabled_index(void);
void jm_ld_array_deploy_showed_index(void);
size_t jm_ld_array_size(void);
size_t jm_ld_array_enabled_size(void);
size_t jm_ld_array_showed_size(void);
void jm_ld_array_set_short_name(gint32 index, const gchar *short_name);
const gchar* jm_ld_array_get_short_name(gint32 index);
void jm_ld_array_set_content(gint32 index, const gchar *content);
const gchar* jm_ld_array_get_content(gint32 index);
void jm_ld_array_set_unit(gint32 index, const gchar *unit);
const gchar* jm_ld_array_get_unit(gint32 index);
void jm_ld_array_set_default_value(gint32 index, const gchar *value);
const gchar* jm_ld_array_get_default_value(gint32 index);
void jm_ld_array_set_value(gint32 index, const gchar *value);
const gchar* jm_ld_array_get_value(gint32 index);
void jm_ld_array_set_command_id(gint32 index, gint32 id);
gint32 jm_ld_array_get_command_id(gint32 index);
void jm_ld_array_set_enabled(gint32 index, gboolean enabled);
gboolean jm_ld_array_get_enabled(gint32 index);
void jm_ld_array_set_showed(gint32 index, gboolean show);
gboolean jm_ld_array_get_showed(gint32 index);

/************************************************************************/
/* JM Lib                                                               */
/************************************************************************/
void jm_lib_init(const gchar *software_path);
void jm_lib_destroy(void);

G_END_DECLS

#endif