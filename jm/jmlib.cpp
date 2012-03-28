#include "jmlib.h"
#include "jmui.h"
#include "jmauth.hpp"
#include "jmvehicle.h"
#include "jmlog.hpp"
#include "jmdb.hpp"
#include "jmsys.hpp"
#include "jmlink.hpp"
#include "jmcommbox.hpp"
#include "jmcommboxport.hpp"
#include "jmcommboxfactory.hpp"
#include "jmcanbus.hpp"
#include "jmkwp1281.hpp"
#include "jmkwp2000.hpp"
#include "jmmikuni.hpp"
#include "jmui.hpp"

static gchar* _jm_std_string_to_gchar(const std::string &text)
{
	if (text.empty())
		return NULL;

	gchar* ret = (gchar*)g_malloc(text.length() + 1);
	strcpy(ret, text.c_str());
	return ret;
}

extern "C"
{
    /************************************************************************/
    /* Auth                                                                 */
    /************************************************************************/
    void jm_auth_set_dat_path(const gchar *path)
    {
        JM::Auth::inst().setPath(std::string(path));
    }

    void jm_auth_save_reg(const gchar *reg)
    {
        JM::Auth::inst().saveReg(std::string(reg));
    }

    gchar* jm_auth_query_id_code(void)
    {
		std::string idCode = JM::Auth::inst().queryIDCode();
		return _jm_std_string_to_gchar(idCode);
    }

    gchar* jm_auth_decrypt_id_code()
    {
		std::string idCode = JM::Auth::inst().decrypt(JM_AUTH_DE_ID_CODE);
		return _jm_std_string_to_gchar(idCode);
    }

    gchar* jm_auth_decrypt_commbox_id()
    {
		std::string commboxID = JM::Auth::inst().decrypt(JM_AUTH_DE_COMMBOX_ID);
		return _jm_std_string_to_gchar(commboxID);
    }

    gchar* jm_auth_decrypt_reg_time()
    {
		std::string regTime = JM::Auth::inst().decrypt(JM_AUTH_DE_REG_TIME);
		return _jm_std_string_to_gchar(regTime);
    }

    gchar* jm_auth_decrypt_expire_time()
    {
		std::string expireTime = JM::Auth::inst().decrypt(JM_AUTH_DE_EXPIRE_TIME);
		return _jm_std_string_to_gchar(expireTime);
    }

    gchar* jm_auth_decrypt_db_pw()
    {
		std::string dbPW = JM::Auth::inst().decrypt(JM_AUTH_DE_DB_PW);
		return _jm_std_string_to_gchar(dbPW);
    }

    gchar* jm_auth_decrypt_lang()
    {
		std::string lang = JM::Auth::inst().decrypt(JM_AUTH_DE_LANG);
		return _jm_std_string_to_gchar(lang);
    }

    gchar* jm_auth_decrypt_log_pw()
    {
		std::string logPW = JM::Auth::inst().decrypt(JM_AUTH_DE_LOG_PW);
		return _jm_std_string_to_gchar(logPW);
    }

    gchar* jm_auth_encrypt_log_text(const gchar *log)
    {
		std::string text = JM::Auth::inst().encryptLogText(std::string(log));
		return _jm_std_string_to_gchar(text);
    }

    gboolean jm_auth_check_reg(void)
    {
        return JM::Auth::inst().checkReg() ? TRUE : FALSE;
    }

    /************************************************************************/
    /* Message Log                                                          */
    /************************************************************************/
    void jm_log_write(const gchar *tag, const gchar *msg)
    {
        JM::Log::inst().write(std::string(tag), std::string(msg));
    }

    void jm_log_write_hex(const gchar *tag, const guint8 *data, size_t count)
    {
        JM::Log::inst().write(std::string(tag), data, count);
    }

    /************************************************************************/
    /* Database                                                             */
    /************************************************************************/
    gboolean jm_db_open(const gchar* file_path, const gchar* password)
    {
        return JM::Database::inst().open(std::string(file_path), std::string(password));
    }

    gboolean jm_db_close(void)
    {
        return JM::Database::inst().close();
    }

    gboolean jm_db_is_open(void)
    {
        return JM::Database::inst().isOpen();
    }

    void jm_db_set_tc_catalog(const gchar *catalog)
    {
        JM::Database::inst().setTCCatalog(std::string(catalog));
    }

    void jm_db_set_ld_catalog(const gchar *catalog)
    {
        JM::Database::inst().setLDCatalog(std::string(catalog));
    }

    void jm_db_set_cmd_catalog(const gchar *catalog)
    {
        JM::Database::inst().setCMDCatalog(std::string(catalog));
    }

    gchar *jm_db_get_text(const gchar *name)
    {
		std::string text = JM::Database::inst().getText(std::string(name));
		return _jm_std_string_to_gchar(text);
    }

    gchar *jm_db_get_trouble_code(const gchar *code)
    {
		std::string content = JM::Database::inst().getTroubleCode(std::string(code));
		return _jm_std_string_to_gchar(content);
    }

    GByteArray *jm_db_get_command(const gchar *name)
    {
		GByteArray *result;
		boost::asio::const_buffer buff = JM::Database::inst().getCommand<boost::asio::const_buffer>(std::string(name));
		result = g_byte_array_new();
		result = g_byte_array_append(result, boost::asio::buffer_cast<const guint8*>(buff), boost::asio::buffer_size(buff));
		return result;
    }

    GByteArray *jm_db_get_command_by_id(gint32 id)
    {
		GByteArray *result;
		boost::asio::const_buffer buff = JM::Database::inst().getCommandByID<boost::asio::const_buffer>(id);
		result = g_byte_array_new();
		result = g_byte_array_append(result, boost::asio::buffer_cast<const guint8*>(buff), boost::asio::buffer_size(buff));
		return result;
    }

    /************************************************************************/
    /* System                                                               */
    /************************************************************************/
    void jm_sys_set_software_path(const gchar *software_path)
    {
        JM::System::inst().setPath(std::string(software_path));
    }

    gchar* jm_sys_text(const gchar *name)
    {
		GString *ret = g_string_new(JM::System::inst().text(std::string(name)).c_str());
		return g_string_free(ret, FALSE);
    }

    /************************************************************************/
    /* Commbox Link                                                         */
    /************************************************************************/
    static JM::Link *_link_handle = NULL;

    void jm_link_set_handler(gpointer handle)
    {
        _link_handle = (JM::Link*)handle;
    }

    JMProtocolType jm_link_protocol_type(void)
    {
        g_return_val_if_fail(_link_handle != NULL, JM_PRC_UNKNOW);
        return ((JM::Link*)(_link_handle))->protocolType();
    }

    gpointer jm_link_get_protocol(void)
    {
        g_return_val_if_fail(((JM::Link*)(_link_handle)) != NULL, NULL);
        return ((JM::Link*)(_link_handle))->protocol();
    }

    size_t jm_link_send_one_frame(const guint8 *data, size_t count)
    {

        g_return_val_if_fail(((JM::Link*)(_link_handle)) != NULL, 0);
        return ((JM::Link*)(_link_handle))->sendOneFrame(data, count);
    }

    size_t jm_link_send_frames(const guint8 *data, size_t count)
    {

        g_return_val_if_fail(((JM::Link*)(_link_handle)) != NULL, 0);
        return ((JM::Link*)(_link_handle))->sendFrames(data, count);
    }

    size_t jm_link_read_one_frame(guint8 *data, size_t max_length)
    {

        g_return_val_if_fail(((JM::Link*)(_link_handle)) != NULL, 0);
        return ((JM::Link*)(_link_handle))->readOneFrame(boost::asio::mutable_buffer(data, max_length));
    }

    size_t jm_link_read_frames(guint8 *data, size_t max_length)
    {

        g_return_val_if_fail(((JM::Link*)(_link_handle)) != NULL, 0);
        return ((JM::Link*)(_link_handle))->readFrames(boost::asio::mutable_buffer(data, max_length));
    }

    size_t jm_link_send_and_recv(const guint8 *send, size_t send_count, guint8 *recv, size_t max_length)
    {

        g_return_val_if_fail(((JM::Link*)(_link_handle)) != NULL, 0);
        return ((JM::Link*)(_link_handle))->sendAndRecv(send, send_count, recv, max_length);
    }

    gint32 jm_link_start_keep_link(gboolean run)
    {

        g_return_val_if_fail(((JM::Link*)(_link_handle)) != NULL, JM_ERROR_GENERIC);
        return ((JM::Link*)(_link_handle))->startKeepLink(run ? true : false);
    }

    gint32 jm_link_set_keep_link(const guint8 *data, size_t count)
    {

        g_return_val_if_fail(((JM::Link*)(_link_handle)) != NULL, JM_ERROR_GENERIC);
        return ((JM::Link*)(_link_handle))->setKeepLink(data, count);
    }

    gint32 jm_link_set_timeout(gint64 tx_b2b, gint64 rx_b2b, gint64 tx_f2f, gint64 rx_f2f, gint64 total)
    {

        g_return_val_if_fail(((JM::Link*)(_link_handle)) != NULL, JM_ERROR_GENERIC);
        return ((JM::Link*)(_link_handle))->setTimeout(boost::posix_time::microseconds(tx_b2b), 
			boost::posix_time::microseconds(rx_b2b), 
			boost::posix_time::microseconds(tx_f2f), 
			boost::posix_time::microseconds(rx_f2f), 
			boost::posix_time::microseconds(total));
    }

    /************************************************************************/
    /* Commbox                                                              */
    /************************************************************************/
    static JM::Commbox* _commbox_handle = NULL;

    void jm_commbox_set_handler(gpointer handle)
    {
        _commbox_handle = (JM::Commbox*)handle;
    }

    gint32 jm_commbox_open(void)
    {
        g_return_val_if_fail(_commbox_handle != NULL, JM_ERROR_GENERIC);
        return ((JM::Commbox*)(_commbox_handle))->open();
    }

    gint32 jm_commbox_close(void)
    {
        g_return_val_if_fail(((JM::Commbox*)(_commbox_handle)) != NULL, JM_ERROR_GENERIC);
        return ((JM::Commbox*)(_commbox_handle))->close();
    }

    gint32 jm_commbox_configure(JMProtocolType type)
    {
        g_return_val_if_fail(((JM::Commbox*)(_commbox_handle)) != NULL, NULL);
        JM::Link *link = ((JM::Commbox*)(_commbox_handle))->configure(type);
        if (link != NULL)
        {
            jm_link_set_handler(link);
            return JM_ERROR_SUCCESS;
        }
        return JM_ERROR_GENERIC;
    }

    gint32 jm_commbox_set_connector(JMConnector cn)
    {

        g_return_val_if_fail(((JM::Commbox*)(_commbox_handle)) != NULL, JM_ERROR_GENERIC);
        return ((JM::Commbox*)(_commbox_handle))->setConnector(cn);
    }

    /************************************************************************/
    /* Commbox Port                                                         */
    /************************************************************************/
    void jm_commbox_port_set_type(JMCommboxPortType type)
    {
        JM::CommboxPort::inst().setType(type);
    }

    JMCommboxPortType jm_commbox_port_get_type(void)
    {
        return JM::CommboxPort::inst().type();
    }

    void jm_commbox_port_set_pointer(gpointer p)
    {
        JM::CommboxPort::inst().setPointer(p);
    }

    gpointer jm_commbox_port_get_pointer(void)
    {
        return JM::CommboxPort::inst().pointer();
    }

    gint32 jm_commbox_port_set_read_timeout(gint64 microseconds) 
    {
        return JM::CommboxPort::inst().setReadTimeout(boost::posix_time::microseconds(microseconds));
    }

    gint32 jm_commbox_port_set_write_timeout(gint64 microseconds)
    {
        return JM::CommboxPort::inst().setWriteTimeout(boost::posix_time::microseconds(microseconds));
    }

    size_t jm_commbox_port_bytes_available(void)
    {
        return JM::CommboxPort::inst().bytesAvailable();
    }

    gint32 jm_commbox_port_discard_out_buffer(void)
    {
        return JM::CommboxPort::inst().discardOutBuffer();
    }

    gint32 jm_commbox_port_discard_in_buffer(void)
    {
        return JM::CommboxPort::inst().discardInBuffer();
    }

    size_t jm_commbox_port_read(guint8 *data, size_t count)
    {
        return JM::CommboxPort::inst().read(boost::asio::mutable_buffer(data, count));
    }

    size_t jm_commbox_port_write(const guint8 *data, size_t count)
    {
        return JM::CommboxPort::inst().write(boost::asio::const_buffer(data, count));
    }

    void jm_commbox_port_push_in_deque(const guint8 *data, size_t count)
    {
        JM::CommboxPort::inst().pushInDeque(boost::asio::const_buffer(data, count));
    }

    gboolean jm_commbox_port_out_deque_available(void)
    {
        return JM::CommboxPort::inst().outDequeAvailable();
    }

    gboolean jm_commbox_port_pop_out_deque(guint8 *data, size_t count)
    {
        return JM::CommboxPort::inst().popOutDeque(boost::asio::mutable_buffer(data, count));
    }

    /************************************************************************/
    /* Commbox Factory                                                      */
    /************************************************************************/
    static JM::Canbus *_canbus_handle = NULL;

    void jm_commbox_factory_set_commbox_version(JMCommboxVersion ver)
    {
        JM::CommboxFactory::inst().setVersion(ver);
    }

    /************************************************************************/
    /* Canbus                                                               */
    /************************************************************************/
    void jm_canbus_set_handler(gpointer handle)
    {
        _canbus_handle = (JM::Canbus*)handle;
    }

    size_t jm_canbus_pack(const guint8 *src, size_t src_length, guint8 *tar, size_t tar_length)
    {
        g_return_val_if_fail(((JM::Canbus*)(_canbus_handle)) != NULL, 0);
        return ((JM::Canbus*)(_canbus_handle))->pack(src, src_length, tar, tar_length);
    }

    size_t jm_canbus_unpack(const guint8 *src, size_t src_length, guint8 *tar, size_t tar_length)
    {
        g_return_val_if_fail(((JM::Canbus*)(_canbus_handle)) != NULL, 0);
        return ((JM::Canbus*)(_canbus_handle))->unpack(src, src_length, tar, tar_length);
    }

    gint32 jm_canbus_set_lines(gint32 high, gint32 low)
    {
        g_return_val_if_fail(((JM::Canbus*)(_canbus_handle)) != NULL, JM_ERROR_GENERIC);
        return ((JM::Canbus*)(_canbus_handle))->setLines(high, low);
    }

    gint32 jm_canbus_set_filter(const gint32 *id_array, size_t count)
    {
        g_return_val_if_fail(((JM::Canbus*)(_canbus_handle)) != NULL, JM_ERROR_GENERIC);
		std::vector<boost::int32_t> id_vector(id_array, id_array + count);
        return ((JM::Canbus*)(_canbus_handle))->setFilter(id_vector);
    }

    gint32 jm_canbus_set_options(gint32 id, JMCanbusBaud baud, JMCanbusIDMode id_mode, JMCanbusFilterMask mask, JMCanbusFrameType frame)
    {
        g_return_val_if_fail(((JM::Canbus*)(_canbus_handle)) != NULL, JM_ERROR_GENERIC);
        return ((JM::Canbus*)(_canbus_handle))->setOptions(id, baud, id_mode, mask, frame);
    }
    
    /************************************************************************/
    /* KWP2000                                                              */
    /************************************************************************/
    static JM::KWP2000 *_kwp2000_handle = NULL;

    void jm_kwp2000_set_handler(gpointer handle)
    {
        _kwp2000_handle = (JM::KWP2000*)handle;
    }

    size_t jm_kwp2000_pack(const guint8 *src, size_t src_length, guint8 *tar, size_t tar_length)
    {
        g_return_val_if_fail(((JM::KWP2000*)(_kwp2000_handle)) != NULL, 0);

        return ((JM::KWP2000*)(_kwp2000_handle))->pack(src, src_length, tar, tar_length);
    }

    size_t jm_kwp2000_unpack(const guint8 *src, size_t src_length, guint8 *tar, size_t tar_length)
    {
        g_return_val_if_fail(((JM::KWP2000*)(_kwp2000_handle)) != NULL, 0);

        return ((JM::KWP2000*)(_kwp2000_handle))->unpack(src, src_length, tar, tar_length);
    }

    gint32 jm_kwp2000_addr_init(guint8 addr_code)
    {
        g_return_val_if_fail(((JM::KWP2000*)(_kwp2000_handle)) != NULL, JM_ERROR_GENERIC);

        return ((JM::KWP2000*)(_kwp2000_handle))->addrInit(addr_code);
    }

    gint32 jm_kwp2000_fast_init(const guint8 *data, size_t count)
    {
        g_return_val_if_fail(((JM::KWP2000*)(_kwp2000_handle)) != NULL, JM_ERROR_GENERIC);

        return ((JM::KWP2000*)(_kwp2000_handle))->fastInit(data, count);
    }

    gint32 jm_kwp2000_set_lines(gint32 com_line, gboolean l_line)
    {
        g_return_val_if_fail(((JM::KWP2000*)(_kwp2000_handle)) != NULL, JM_ERROR_GENERIC);

        return ((JM::KWP2000*)(_kwp2000_handle))->setLines(com_line, l_line ? true : false);
    }

    gint32 jm_kwp2000_set_options(JMKWPMode msg_mode, JMKWPMode link_mode, gint32 baud)
    {
        g_return_val_if_fail(((JM::KWP2000*)(_kwp2000_handle)) != NULL, JM_ERROR_GENERIC);

        return ((JM::KWP2000*)(_kwp2000_handle))->setOptions(msg_mode, link_mode, baud);
    }

    gint32 jm_kwp2000_set_address(guint8 target, guint8 source)
    {
        g_return_val_if_fail(((JM::KWP2000*)(_kwp2000_handle)) != NULL, JM_ERROR_GENERIC);

        return ((JM::KWP2000*)(_kwp2000_handle))->setAddress(target, source);
    }

    /************************************************************************/
    /* KWP1281                                                              */
    /************************************************************************/
    static JM::KWP1281 *_kwp1281_handle = NULL;
    
    void jm_kwp1281_set_handler(gpointer handle)
    {
        _kwp1281_handle = (JM::KWP1281*)handle;
    }

    size_t jm_kwp1281_pack(const guint8 *src, size_t src_length, guint8 *tar, size_t tar_length)
    {
        g_return_val_if_fail(((JM::KWP1281*)(_kwp1281_handle)) != NULL, 0);
        return ((JM::KWP1281*)(_kwp1281_handle))->pack(src, src_length, tar, tar_length);
    }

    size_t jm_kwp1281_unpack(const guint8 *src, size_t src_length, guint8 *tar, size_t tar_length)
    {
        g_return_val_if_fail(((JM::KWP1281*)(_kwp1281_handle)) != NULL, 0);

        return ((JM::KWP1281*)(_kwp1281_handle))->unpack(src, src_length, tar, tar_length);
    }

    gint32 jm_kwp1281_addr_init(guint8 addr_code)
    {
        g_return_val_if_fail(((JM::KWP1281*)(_kwp1281_handle)) != NULL, JM_ERROR_GENERIC);

        return ((JM::KWP1281*)(_kwp1281_handle))->addrInit(addr_code);
    }

    gint32 jm_kwp1281_set_lines(gint32 com_line, gboolean l_line)
    {
        g_return_val_if_fail(((JM::KWP1281*)(_kwp1281_handle)) != NULL, JM_ERROR_GENERIC);

        return ((JM::KWP1281*)(_kwp1281_handle))->setLines(com_line, l_line ? true : false);
    }

    /************************************************************************/
    /* Mikuni Protocol                                                      */
    /************************************************************************/
    static JM::Mikuni *_mikuni_handle = NULL;

    void jm_mikuni_set_handler(gpointer handle)
    {
        _mikuni_handle = (JM::Mikuni*)handle;
    }

    size_t jm_mikuni_pack(const guint8 *src, size_t src_length, guint8 *tar, size_t tar_length)
    {
        g_return_val_if_fail(((JM::Mikuni*)(_mikuni_handle)) != NULL, 0);

        return ((JM::Mikuni*)(_mikuni_handle))->pack(src, src_length, tar, tar_length);
    }

    size_t jm_mikuni_unpack(const guint8 *src, size_t src_length, guint8 *tar, size_t tar_length)
    {
        g_return_val_if_fail(((JM::Mikuni*)(_mikuni_handle)) != NULL, 0);

        return ((JM::Mikuni*)(_mikuni_handle))->unpack(src, src_length, tar, tar_length);
    }

    gint32 jm_mikuni_init(void)
    {
        g_return_val_if_fail(((JM::Mikuni*)(_mikuni_handle)) != NULL, JM_ERROR_GENERIC);

        return ((JM::Mikuni*)(_mikuni_handle))->init();
    }

    /************************************************************************/
    /* User Interface                                                       */
    /************************************************************************/
    void jm_ui_msg_free(JMUIMessage *obj)
    {
        JM::UserInterface::inst().msgFree(obj);
    }

    void jm_ui_main_show(void)
    {
        JM::UserInterface::inst().mainShow();
    }

    void jm_ui_msg_box_btn_clr(void)
    {
        JM::UserInterface::inst().msgBoxBtnClr();
    }

    void jm_ui_msg_box_add_btn(const gchar *text)
    {
        JM::UserInterface::inst().msgBoxAddBtn(text);
    }

    void jm_ui_msg_box_set_msg(const gchar *text)
    {
        JM::UserInterface::inst().msgBoxSetMsg(text);
    }

    void jm_ui_msg_box_set_title(const gchar *text)
    {
        JM::UserInterface::inst().msgBoxSetTitle(text);
    }

    void jm_ui_msg_box_show(void)
    {
        JM::UserInterface::inst().msgBoxShow();
    }

    void jm_ui_msg_box_hide(void)
    {
        JM::UserInterface::inst().msgBoxHide();
    }

    void jm_ui_list_box_btn_clr(void)
    {
        JM::UserInterface::inst().listBoxBtnClr();
    }

    void jm_ui_list_box_add_btn(const gchar *text)
    {
        JM::UserInterface::inst().listBoxAddBtn(text);
    }

    void jm_ui_list_box_add_item(const gchar *caption, const gchar *item)
    {
        JM::UserInterface::inst().listBoxAddItem(caption, item);
    }

    void jm_ui_list_box_item_clr(void)
    {
        JM::UserInterface::inst().listBoxItemClr();
    }

    void jm_ui_list_box_show(void)
    {
        JM::UserInterface::inst().listBoxShow();
    }

    void jm_ui_list_box_hide(void)
    {
        JM::UserInterface::inst().listBoxHide();
    }

    void jm_ui_menu_item_clr(void)
    {
        JM::UserInterface::inst().menuItemClr();
    }

    void jm_ui_menu_add_item(const gchar *text)
    {
        JM::UserInterface::inst().menuAddItem(text);
    }

    void jm_ui_menu_show(void)
    {
        JM::UserInterface::inst().menuShow();
    }

    void jm_ui_tc_item_clr(void)
    {
        JM::UserInterface::inst().tcItemClr();
    }

    void jm_ui_tc_add_item(const gchar *code, const gchar *text)
    {
        JM::UserInterface::inst().tcAddItem(code, text);
    }

    void jm_ui_tc_add_btn(const gchar *text)
    {
        JM::UserInterface::inst().tcAddBtn(text);
    }

    void jm_ui_tc_btn_clr(void)
    {
        JM::UserInterface::inst().tcBtnClr();
    }

    void jm_ui_tc_show(void)
    {
        JM::UserInterface::inst().tcShow();
    }

    void jm_ui_ld_prepare_show(void)
    {
        JM::UserInterface::inst().ldPrepareShow();
    }

    void jm_ui_ld_show(void)
    {
        JM::UserInterface::inst().ldShow();
    }

    void jm_ui_ld_btn_clr(void)
    {
        JM::UserInterface::inst().ldBtnClr();
    }

    void jm_ui_ld_add_btn(const gchar *text)
    {
        JM::UserInterface::inst().ldAddBtn(text);
    }

    void jm_ui_ld_set_value(gint32 index, const gchar *value)
    {
        JM::UserInterface::inst().ldSetValue(index, value);
    }

    gchar* jm_ui_get_btn_clicked(gboolean is_blocked)
    {
        return JM::UserInterface::inst().btnClicked(is_blocked);
    }

    void jm_ui_set_btn_clicked(const gchar *text)
    {
        JM::UserInterface::inst().setBtnClicked(text);
    }

    gchar* jm_ui_get_menu_selected(void)
    {
        return JM::UserInterface::inst().menuSelected();
    }

    void jm_ui_set_menu_selected(const gchar *text)
    {
        JM::UserInterface::inst().setMenuSelected(text);
    }

    JMUIMessage* jm_ui_pop_msg(void)
    {
        return JM::UserInterface::inst().popMsg();
    }

    size_t jm_ui_msg_count(void)
    {
        return JM::UserInterface::inst().msgCount();
    }

	/************************************************************************/
	/* Live Data Array                                                      */
	/************************************************************************/
	void jm_ld_array_update_global_array(gboolean showed)
	{
		JM::LiveDataVector::updateGlobalArray(showed ? true : false);
	}

	gint32 jm_ld_array_next_showed_index(void)
	{
		return JM::LiveDataVector::nextShowedIndex();
	}

	gint32 jm_ld_array_get_showed_index(gint32 index)
	{
		return JM::LiveDataVector::getShowedIndex(index);
	}

	gint32 jm_ld_array_query_showed_position(gint32 index)
	{
		return JM::LiveDataVector::queryShowedPosition(index);
	}

	gint32 jm_ld_array_get_enabled_index(gint32 index)
	{
		return JM::LiveDataVector::getEnabledIndex(index);
	}

	void jm_ld_array_deploy_enabled_index(void)
	{
		JM::LiveDataVector::deployEnabledIndex();
	}

	void jm_ld_array_deploy_showed_index(void)
	{
		JM::LiveDataVector::deployShowedIndex();
	}

	size_t jm_ld_array_size(void)
	{
		return JM::LiveDataVector::globalSize();
	}

	size_t jm_ld_array_enabled_size(void)
	{
		return JM::LiveDataVector::enabledSize();
	}

	size_t jm_ld_array_showed_size(void)
	{
		return JM::LiveDataVector::showedSize();
	}

	void jm_ld_array_set_short_name(gint32 index, const gchar *short_name)
	{
		JM::LiveDataVector::globalAt(index)->setShortName(std::string(short_name));
	}

	const gchar* jm_ld_array_get_short_name(gint32 index)
	{
		return JM::LiveDataVector::globalAt(index)->shortName().c_str();
	}

	void jm_ld_array_set_content(gint32 index, const gchar *content)
	{
		JM::LiveDataVector::globalAt(index)->setContent(std::string(content));
	}

	const gchar* jm_ld_array_get_content(gint32 index)
	{
		return JM::LiveDataVector::globalAt(index)->content().c_str();
	}

	void jm_ld_array_set_unit(gint32 index, const gchar *unit)
	{
		JM::LiveDataVector::globalAt(index)->setUnit(std::string(unit));
	}

	const gchar* jm_ld_array_get_unit(gint32 index)
	{
		return JM::LiveDataVector::globalAt(index)->unit().c_str();
	}

	void jm_ld_array_set_default_value(gint32 index, const gchar *value)
	{
		JM::LiveDataVector::globalAt(index)->setDefaultValue(std::string(value));
	}

	const gchar* jm_ld_array_get_default_value(gint32 index)
	{
		return JM::LiveDataVector::globalAt(index)->defaultValue().c_str();
	}

	void jm_ld_array_set_value(gint32 index, const gchar *value)
	{
		JM::LiveDataVector::globalAt(index)->setValue(std::string(value));
	}

	const gchar* jm_ld_array_get_value(gint32 index)
	{
		return JM::LiveDataVector::globalAt(index)->value().c_str();
	}

	void jm_ld_array_set_command_id(gint32 index, gint32 id)
	{
		JM::LiveDataVector::globalAt(index)->setCmdID(id);
	}

	gint32 jm_ld_array_get_command_id(gint32 index)
	{
		return JM::LiveDataVector::globalAt(index)->cmdID();
	}

	void jm_ld_array_set_enabled(gint32 index, gboolean enabled)
	{
		JM::LiveDataVector::globalAt(index)->setEnabled(enabled ? true : false);
	}

	gboolean jm_ld_array_get_enabled(gint32 index)
	{
		return JM::LiveDataVector::globalAt(index)->enabled();
	}

	void jm_ld_array_set_showed(gint32 index, gboolean show)
	{
		JM::LiveDataVector::globalAt(index)->setShowed(show ? true : false);
	}

	gboolean jm_ld_array_get_showed(gint32 index)
	{
		return JM::LiveDataVector::globalAt(index)->showed();
	}

    /************************************************************************/
    /* JM Lib                                                               */
    /************************************************************************/
    void jm_lib_init(const gchar *software_path)
    {
        g_thread_init(NULL);
        jm_sys_set_software_path(software_path);
        jm_auth_set_dat_path(software_path);
        jm_vehicle_init();
        jm_vehicle_set_path(software_path);
    }

    void jm_lib_destroy(void)
    {
        jm_vehicle_destroy();
    }
};

