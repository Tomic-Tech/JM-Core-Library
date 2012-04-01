#ifndef __JM_LIB_H__
#define __JM_LIB_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <jm/jmtype.h>
#include <jm/jmerror.h>
#include <jm/jmutils.h>
#include <jm/jmbytearray.h>
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

JM_BEGIN_DECLS

/************************************************************************/
/* Auth                                                                     */
/************************************************************************/
void jm_auth_set_dat_path(const char *path);
void jm_auth_save_reg(const char *reg);
char* jm_auth_query_id_code(void);
char* jm_auth_decrypt_id_code();
char* jm_auth_decrypt_commbox_id();
char* jm_auth_decrypt_reg_time();
char* jm_auth_decrypt_expire_time();
char* jm_auth_decrypt_db_pw();
char* jm_auth_decrypt_lang();
char* jm_auth_decrypt_log_pw();
char* jm_auth_encrypt_log_text(const char *log);
boolean_t jm_auth_check_reg(void);

/************************************************************************/
/* Message Log                                                                     */
/************************************************************************/
void jm_log_write(const char *tag, const char *msg);
void jm_log_write_hex(const char *tag, const uint8_t *data, size_t count);

/************************************************************************/
/* Database                                                             */
/************************************************************************/
boolean_t jm_db_open(const char* file_path, const char* password);
boolean_t jm_db_close(void);
boolean_t jm_db_is_open(void);
void jm_db_set_tc_catalog(const char *catalog);
void jm_db_set_ld_catalog(const char *catalog);
void jm_db_set_cmd_catalog(const char *catalog);
char *jm_db_get_text(const char *name);
char *jm_db_get_trouble_code(const char *code);
JMByteArray *jm_db_get_command(const char *name);
JMByteArray *jm_db_get_command_by_id(int32_t id);

/************************************************************************/
/* System                                                               */
/************************************************************************/
void jm_sys_set_software_path(const char *software_path);
char* jm_sys_text(const char *name);

/************************************************************************/
/* Commbox Link                                                         */
/************************************************************************/
void jm_link_set_handler(pointer_t handle);
JMProtocolType jm_link_protocol_type(void);
pointer_t jm_link_get_protocol(void);
size_t jm_link_send_one_frame(const uint8_t *data, size_t count);
size_t jm_link_send_frames(const uint8_t *data, size_t count);
size_t jm_link_read_one_frame(uint8_t *data, size_t max_length);
size_t jm_link_read_frames(uint8_t *data, size_t max_length);
size_t jm_link_send_and_recv(const uint8_t *send, size_t send_count, uint8_t *recv, size_t max_length);
int32_t jm_link_start_keep_link(boolean_t run);
int32_t jm_link_set_keep_link(const uint8_t *data, size_t count);
int32_t jm_link_set_timeout(int64_t tx_b2b, int64_t rx_b2b, int64_t tx_f2f, int64_t rx_f2f, int64_t total);

/************************************************************************/
/* Commbox                                                              */
/************************************************************************/
void jm_commbox_set_handler(pointer_t handle);
int32_t jm_commbox_open(void);
int32_t jm_commbox_close(void);
int32_t jm_commbox_configure(JMProtocolType type);
int32_t jm_commbox_set_connector(JMConnector cnn);

/************************************************************************/
/* Commbox Port                                                         */
/************************************************************************/
// If we use SerialPort (or other need) we will need a pointer to operate.
//void jm_commbox_port_set_pointer(pointer_t p);
//pointer_t jm_commbox_port_get_pointer(void);
void jm_commbox_port_set_type(JMCommboxPortType type);
//JMCommboxPortType jm_commbox_port_get_type(void);
//int32_t jm_commbox_port_set_read_timeout(int64_t microseconds);
//int32_t jm_commbox_port_set_write_timeout(int64_t microseconds);
//size_t jm_commbox_port_bytes_available(void);
//int32_t jm_commbox_port_discard_out_buffer(void);
//int32_t jm_commbox_port_discard_in_buffer(void);
//size_t jm_commbox_port_read(uint8_t *data, size_t count);
//size_t jm_commbox_port_write(const uint8_t *data, size_t count);

void jm_commbox_port_push_in_deque(const uint8_t *data, size_t count);
boolean_t jm_commbox_port_out_deque_available(void);
boolean_t jm_commbox_port_pop_out_deque(uint8_t *data, size_t count);

/************************************************************************/
/* Commbox Factory                                                      */
/************************************************************************/
void jm_commbox_factory_set_commbox_version(JMCommboxVersion ver);

/************************************************************************/
/* Canbus                                                               */
/************************************************************************/
void jm_canbus_set_handler(pointer_t handle);
size_t jm_canbus_pack(const uint8_t *src, size_t src_length, uint8_t *tar, size_t tar_length);
size_t jm_canbus_unpack(const uint8_t *src, size_t src_length, uint8_t *tar, size_t tar_length);
int32_t jm_canbus_set_lines(int32_t high, int32_t low);
int32_t jm_canbus_set_filter(const int32_t *id_array, size_t count);
int32_t jm_canbus_set_options(int32_t id, JMCanbusBaud baud, JMCanbusIDMode id_mode, JMCanbusFilterMask mask, JMCanbusFrameType frame);
int32_t jm_canbus_init(void);

/************************************************************************/
/* KWP2000                                                              */
/************************************************************************/
void jm_kwp2000_set_handler(pointer_t handle);
size_t jm_kwp2000_pack(const uint8_t *src, size_t src_length, uint8_t *tar, size_t tar_length);
size_t jm_kwp2000_unpack(const uint8_t *src, size_t src_length, uint8_t *tar, size_t tar_length);
int32_t jm_kwp2000_addr_init(uint8_t addr_code);
int32_t jm_kwp2000_fast_init(const uint8_t *data, size_t count);
int32_t jm_kwp2000_set_lines(int32_t com_line, boolean_t l_line);
int32_t jm_kwp2000_set_options(JMKWPMode msg_mode, JMKWPMode link_mode, int32_t baud);
int32_t jm_kwp2000_set_address(uint8_t target, uint8_t source);


/************************************************************************/
/* KWP1281                                                              */
/************************************************************************/
void jm_kwp1281_set_handler(pointer_t handle);
size_t jm_kwp1281_pack(const uint8_t *src, size_t src_length, uint8_t *tar, size_t tar_length);
size_t jm_kwp1281_unpack(const uint8_t *src, size_t src_length, uint8_t *tar, size_t tar_length);
int32_t jm_kwp1281_addr_init(uint8_t addr_code);
int32_t jm_kwp1281_set_lines(int32_t com_line, boolean_t l_line);

/************************************************************************/
/* Mikuni Protocol                                                      */
/************************************************************************/
void jm_mikuni_set_handler(pointer_t handle);
size_t jm_mikuni_pack(const uint8_t *src, size_t src_length, uint8_t *tar, size_t tar_length);
size_t jm_mikuni_unpack(const uint8_t *src, size_t src_length, uint8_t *tar, size_t tar_length);
int32_t jm_mikuni_init(void);

/************************************************************************/
/* User Interface                                                       */
/************************************************************************/
void jm_ui_msg_free(JMUIMessage *obj);
void jm_ui_main_show(void);
void jm_ui_msg_box_btn_clr(void);
void jm_ui_msg_box_add_btn(const char *text);
void jm_ui_msg_box_set_msg(const char *text);
void jm_ui_msg_box_set_title(const char *text);
void jm_ui_msg_box_show(void);
void jm_ui_msg_box_hide(void);
void jm_ui_list_box_btn_clr(void);
void jm_ui_list_box_add_btn(const char *text);
void jm_ui_list_box_add_item(const char *caption, const char *item);
void jm_ui_list_box_item_clr(void);
void jm_ui_list_box_show(void);
void jm_ui_list_box_hide(void);
void jm_ui_menu_item_clr(void);
void jm_ui_menu_add_item(const char *text);
void jm_ui_menu_show(void);
void jm_ui_tc_item_clr(void);
void jm_ui_tc_add_item(const char *code, const char *text);
void jm_ui_tc_add_btn(const char *text);
void jm_ui_tc_btn_clr(void);
void jm_ui_tc_show(void);
void jm_ui_ld_prepare_show(void);
void jm_ui_ld_show(void);
void jm_ui_ld_btn_clr(void);
void jm_ui_ld_add_btn(const char *text);
void jm_ui_ld_set_value(int32_t index, const char *value);
char* jm_ui_get_btn_clicked(boolean_t is_blocked);
void jm_ui_set_btn_clicked(const char *text);
char* jm_ui_get_menu_selected(void);
void jm_ui_set_menu_selected(const char *text);
JMUIMessage* jm_ui_pop_msg(void);
size_t jm_ui_msg_count(void);

/************************************************************************/
/* Live Data Array                                                      */
/************************************************************************/
void jm_ld_array_update_global_array(boolean_t showed);
int32_t jm_ld_array_next_showed_index(void);
int32_t jm_ld_array_get_showed_index(int32_t index);
int32_t jm_ld_array_query_showed_position(int32_t index);
int32_t jm_ld_array_get_enabled_index(int32_t index);
void jm_ld_array_deploy_enabled_index(void);
void jm_ld_array_deploy_showed_index(void);
size_t jm_ld_array_size(void);
size_t jm_ld_array_enabled_size(void);
size_t jm_ld_array_showed_size(void);
void jm_ld_array_set_short_name(int32_t index, const char *short_name);
const char* jm_ld_array_get_short_name(int32_t index);
void jm_ld_array_set_content(int32_t index, const char *content);
const char* jm_ld_array_get_content(int32_t index);
void jm_ld_array_set_unit(int32_t index, const char *unit);
const char* jm_ld_array_get_unit(int32_t index);
void jm_ld_array_set_default_value(int32_t index, const char *value);
const char* jm_ld_array_get_default_value(int32_t index);
void jm_ld_array_set_value(int32_t index, const char *value);
const char* jm_ld_array_get_value(int32_t index);
void jm_ld_array_set_command_id(int32_t index, int32_t id);
int32_t jm_ld_array_get_command_id(int32_t index);
void jm_ld_array_set_enabled(int32_t index, boolean_t enabled);
boolean_t jm_ld_array_get_enabled(int32_t index);
void jm_ld_array_set_showed(int32_t index, boolean_t show);
boolean_t jm_ld_array_get_showed(int32_t index);

/************************************************************************/
/* JM Diagnose                                                          */
/************************************************************************/
char* jm_diag_calc_std_obd_dtc(const uint8_t *buff, size_t buff_length, int32_t pos, int32_t factor, int32_t offset);

/************************************************************************/
/* JM Lib                                                               */
/************************************************************************/
void jm_lib_init(const char *software_path);
void jm_lib_destroy(void);

JM_END_DECLS

#endif