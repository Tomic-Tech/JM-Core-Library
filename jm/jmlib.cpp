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
#include "jmdiag.hpp"

static char* _jm_std_string_to_char(const std::string &text)
{
	if (text.empty())
		return NULL;

	char *ret = (char*)malloc(text.length() + 1);
	strcpy(ret, text.c_str());
	return ret;
}

extern "C"
{
    /************************************************************************/
    /* Auth                                                                 */
    /************************************************************************/
    void jm_auth_set_dat_path(const char *path)
    {
        JM::Auth::inst().setPath(std::string(path));
    }

    void jm_auth_save_reg(const char *reg)
    {
        JM::Auth::inst().saveReg(std::string(reg));
    }

    char* jm_auth_query_id_code(void)
    {
		std::string idCode = JM::Auth::inst().queryIDCode();
		return _jm_std_string_to_char(idCode);
    }

    char* jm_auth_decrypt_id_code()
    {
		std::string idCode = JM::Auth::inst().decrypt(JM_AUTH_DE_ID_CODE);
		return _jm_std_string_to_char(idCode);
    }

    char* jm_auth_decrypt_commbox_id()
    {
		std::string commboxID = JM::Auth::inst().decrypt(JM_AUTH_DE_COMMBOX_ID);
		return _jm_std_string_to_char(commboxID);
    }

    char* jm_auth_decrypt_reg_time()
    {
		std::string regTime = JM::Auth::inst().decrypt(JM_AUTH_DE_REG_TIME);
		return _jm_std_string_to_char(regTime);
    }

    char* jm_auth_decrypt_expire_time()
    {
		std::string expireTime = JM::Auth::inst().decrypt(JM_AUTH_DE_EXPIRE_TIME);
		return _jm_std_string_to_char(expireTime);
    }

    char* jm_auth_decrypt_db_pw()
    {
		std::string dbPW = JM::Auth::inst().decrypt(JM_AUTH_DE_DB_PW);
		return _jm_std_string_to_char(dbPW);
    }

    char* jm_auth_decrypt_lang()
    {
		std::string lang = JM::Auth::inst().decrypt(JM_AUTH_DE_LANG);
		return _jm_std_string_to_char(lang);
    }

    char* jm_auth_decrypt_log_pw()
    {
		std::string logPW = JM::Auth::inst().decrypt(JM_AUTH_DE_LOG_PW);
		return _jm_std_string_to_char(logPW);
    }

    char* jm_auth_encrypt_log_text(const char *log)
    {
		std::string text = JM::Auth::inst().encryptLogText(std::string(log));
		return _jm_std_string_to_char(text);
    }

    boolean_t jm_auth_check_reg(void)
    {
        return JM::Auth::inst().checkReg() ? TRUE : FALSE;
    }

    /************************************************************************/
    /* Message Log                                                          */
    /************************************************************************/
    void jm_log_write(const char *tag, const char *msg)
    {
        JM::Log::inst().write(std::string(tag), std::string(msg));
    }

    void jm_log_write_hex(const char *tag, const uint8_t *data, size_t count)
    {
        JM::Log::inst().write(std::string(tag), data, count);
    }

    /************************************************************************/
    /* Database                                                             */
    /************************************************************************/
    boolean_t jm_db_open(const char* file_path, const char* password)
    {
        return JM::Database::inst().open(std::string(file_path), std::string(password));
    }

    boolean_t jm_db_close(void)
    {
        return JM::Database::inst().close();
    }

    boolean_t jm_db_is_open(void)
    {
        return JM::Database::inst().isOpen();
    }

    void jm_db_set_tc_catalog(const char *catalog)
    {
        JM::Database::inst().setTCCatalog(std::string(catalog));
    }

    void jm_db_set_ld_catalog(const char *catalog)
    {
        JM::Database::inst().setLDCatalog(std::string(catalog));
    }

    void jm_db_set_cmd_catalog(const char *catalog)
    {
        JM::Database::inst().setCMDCatalog(std::string(catalog));
    }

    char *jm_db_get_text(const char *name)
    {
		std::string text = JM::Database::inst().getText(std::string(name));
		return _jm_std_string_to_char(text);
    }

    char *jm_db_get_trouble_code(const char *code)
    {
		std::string content = JM::Database::inst().getTroubleCode(std::string(code));
		return _jm_std_string_to_char(content);
    }

    JMByteArray *jm_db_get_command(const char *name)
    {
		JMByteArray *result;
		boost::asio::const_buffer buff = JM::Database::inst().getCommand<boost::asio::const_buffer>(std::string(name));
		result = jm_byte_array_new();
		jm_byte_array_append(result, boost::asio::buffer_cast<const uint8_t*>(buff), boost::asio::buffer_size(buff));
		return result;
    }

    JMByteArray *jm_db_get_command_by_id(int32_t id)
    {
		JMByteArray *result;
		boost::asio::const_buffer buff = JM::Database::inst().getCommandByID<boost::asio::const_buffer>(id);
		result = jm_byte_array_new();
		jm_byte_array_append(result, boost::asio::buffer_cast<const uint8_t*>(buff), boost::asio::buffer_size(buff));
		return result;
    }

    /************************************************************************/
    /* System                                                               */
    /************************************************************************/
    void jm_sys_set_software_path(const char *software_path)
    {
        JM::System::inst().setPath(std::string(software_path));
    }

    char* jm_sys_text(const char *name)
    {
		std::string text = JM::System::inst().text(std::string(name));
		return _jm_std_string_to_char(text);
    }

    /************************************************************************/
    /* Commbox Link                                                         */
    /************************************************************************/
    static JM::Link *_link_handle = NULL;

    void jm_link_set_handler(pointer_t handle)
    {
        _link_handle = (JM::Link*)handle;
    }

    JMProtocolType jm_link_protocol_type(void)
    {
		if (_link_handle == NULL)
			return JM_PRC_UNKNOW;
        return ((JM::Link*)(_link_handle))->protocolType();
    }

    pointer_t jm_link_get_protocol(void)
    {
		if (_link_handle == NULL)
			return NULL;
        return ((JM::Link*)(_link_handle))->protocol();
    }

    size_t jm_link_send_one_frame(const uint8_t *data, size_t count)
    {
		if (_link_handle == NULL)
			return 0;
        return ((JM::Link*)(_link_handle))->sendOneFrame(data, count);
    }

    size_t jm_link_send_frames(const uint8_t *data, size_t count)
    {
		if (_link_handle == NULL)
			return 0;
        return ((JM::Link*)(_link_handle))->sendFrames(data, count);
    }

    size_t jm_link_read_one_frame(uint8_t *data, size_t max_length)
    {
		if (_link_handle == NULL)
			return 0;
        return ((JM::Link*)(_link_handle))->readOneFrame(boost::asio::mutable_buffer(data, max_length));
    }

    size_t jm_link_read_frames(uint8_t *data, size_t max_length)
    {
		if (_link_handle == NULL)
			return 0;
        return ((JM::Link*)(_link_handle))->readFrames(boost::asio::mutable_buffer(data, max_length));
    }

    size_t jm_link_send_and_recv(const uint8_t *send, size_t send_count, uint8_t *recv, size_t max_length)
    {
		if (_link_handle == NULL)
			return 0;
        return ((JM::Link*)(_link_handle))->sendAndRecv(send, send_count, recv, max_length);
    }

    int32_t jm_link_start_keep_link(boolean_t run)
    {
		if (_link_handle == NULL)
			return JM_ERROR_GENERIC;
        return ((JM::Link*)(_link_handle))->startKeepLink(run ? true : false);
    }

    int32_t jm_link_set_keep_link(const uint8_t *data, size_t count)
    {
		if (_link_handle == NULL)
			return JM_ERROR_GENERIC;
        return ((JM::Link*)(_link_handle))->setKeepLink(data, count);
    }

    int32_t jm_link_set_timeout(int64_t tx_b2b, int64_t rx_b2b, int64_t tx_f2f, int64_t rx_f2f, int64_t total)
    {
		if (_link_handle == NULL)
			return JM_ERROR_GENERIC;
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

    void jm_commbox_set_handler(const pointer_t handle)
    {
        _commbox_handle = (JM::Commbox*)handle;
    }

    int32_t jm_commbox_open(void)
    {
		if (_commbox_handle == NULL)
			return JM_ERROR_GENERIC;
        return ((JM::Commbox*)(_commbox_handle))->open();
    }

    int32_t jm_commbox_close(void)
    {
		if (_commbox_handle == NULL)
			return JM_ERROR_GENERIC;
        return ((JM::Commbox*)(_commbox_handle))->close();
    }

    int32_t jm_commbox_configure(JMProtocolType type)
    {
		if (_commbox_handle == NULL)
			return JM_ERROR_GENERIC;
        JM::Link *link = ((JM::Commbox*)(_commbox_handle))->configure(type);
        if (link != NULL)
        {
            jm_link_set_handler(link);
            return JM_ERROR_SUCCESS;
        }
        return JM_ERROR_GENERIC;
    }

    int32_t jm_commbox_set_connector(JMConnector cn)
    {
		if (_commbox_handle == NULL)
			return JM_ERROR_GENERIC;
        return ((JM::Commbox*)(_commbox_handle))->setConnector(cn);
    }

    /************************************************************************/
    /* Commbox Port                                                         */
    /************************************************************************/
    void jm_commbox_port_set_type(JMCommboxPortType type)
    {
        JM::CommboxPort::inst().setType(type);
    }

    //JMCommboxPortType jm_commbox_port_get_type(void)
    //{
    //    return JM::CommboxPort::inst().type();
    //}

    //void jm_commbox_port_set_pointer(pointer_t p)
    //{
    //    JM::CommboxPort::inst().setPointer(p);
    //}

    //pointer_t jm_commbox_port_get_pointer(void)
    //{
    //    return JM::CommboxPort::inst().pointer();
    //}

    //int32_t jm_commbox_port_set_read_timeout(int64_t microseconds) 
    //{
    //    return JM::CommboxPort::inst().setReadTimeout(boost::posix_time::microseconds(microseconds));
    //}

    //int32_t jm_commbox_port_set_write_timeout(int64_t microseconds)
    //{
    //    return JM::CommboxPort::inst().setWriteTimeout(boost::posix_time::microseconds(microseconds));
    //}

    //size_t jm_commbox_port_bytes_available(void)
    //{
    //    return JM::CommboxPort::inst().bytesAvailable();
    //}

    //int32_t jm_commbox_port_discard_out_buffer(void)
    //{
    //    return JM::CommboxPort::inst().discardOutBuffer();
    //}

    //int32_t jm_commbox_port_discard_in_buffer(void)
    //{
    //    return JM::CommboxPort::inst().discardInBuffer();
    //}

    //size_t jm_commbox_port_read(uint8_t *data, size_t count)
    //{
    //    return JM::CommboxPort::inst().read(boost::asio::mutable_buffer(data, count));
    //}

    //size_t jm_commbox_port_write(const uint8_t *data, size_t count)
    //{
    //    return JM::CommboxPort::inst().write(boost::asio::const_buffer(data, count));
    //}

    //void jm_commbox_port_push_in_deque(const uint8_t *data, size_t count)
    //{
    //    JM::CommboxPort::inst().pushInDeque(boost::asio::const_buffer(data, count));
    //}

    //boolean_t jm_commbox_port_out_deque_available(void)
    //{
    //    return JM::CommboxPort::inst().outDequeAvailable();
    //}

    //boolean_t jm_commbox_port_pop_out_deque(uint8_t *data, size_t count)
    //{
    //    return JM::CommboxPort::inst().popOutDeque(boost::asio::mutable_buffer(data, count));
    //}

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
    void jm_canbus_set_handler(pointer_t handle)
    {
        _canbus_handle = (JM::Canbus*)handle;
    }

    size_t jm_canbus_pack(const uint8_t *src, size_t src_length, uint8_t *tar, size_t tar_length)
    {
		if (_canbus_handle == NULL)
			return 0;
        return ((JM::Canbus*)(_canbus_handle))->pack(src, src_length, tar, tar_length);
    }

    size_t jm_canbus_unpack(const uint8_t *src, size_t src_length, uint8_t *tar, size_t tar_length)
    {
		if (_canbus_handle == NULL)
			return 0;
        return ((JM::Canbus*)(_canbus_handle))->unpack(src, src_length, tar, tar_length);
    }

    int32_t jm_canbus_set_lines(int32_t high, int32_t low)
    {
		if (_canbus_handle == NULL)
			return JM_ERROR_GENERIC;
        return ((JM::Canbus*)(_canbus_handle))->setLines(high, low);
    }

    int32_t jm_canbus_set_filter(const int32_t *id_array, size_t count)
    {
		if (_canbus_handle == NULL)
			return JM_ERROR_GENERIC;
		std::vector<boost::int32_t> id_vector(id_array, id_array + count);
        return ((JM::Canbus*)(_canbus_handle))->setFilter(id_vector);
    }

    int32_t jm_canbus_set_options(int32_t id, JMCanbusBaud baud, JMCanbusIDMode id_mode, JMCanbusFilterMask mask, JMCanbusFrameType frame)
    {
		if (_canbus_handle == NULL)
			return JM_ERROR_GENERIC;
        return ((JM::Canbus*)(_canbus_handle))->setOptions(id, baud, id_mode, mask, frame);
    }

	int32_t jm_canbus_init(void)
	{
		if (_canbus_handle == NULL)
			return JM_ERROR_GENERIC;
		return ((JM::Canbus*)(_canbus_handle))->init();
	}
    
    /************************************************************************/
    /* KWP2000                                                              */
    /************************************************************************/
    static JM::KWP2000 *_kwp2000_handle = NULL;

    void jm_kwp2000_set_handler(pointer_t handle)
    {
        _kwp2000_handle = (JM::KWP2000*)handle;
    }

    size_t jm_kwp2000_pack(const uint8_t *src, size_t src_length, uint8_t *tar, size_t tar_length)
    {
		if (_kwp2000_handle == NULL)
			return 0;
        return ((JM::KWP2000*)(_kwp2000_handle))->pack(src, src_length, tar, tar_length);
    }

    size_t jm_kwp2000_unpack(const uint8_t *src, size_t src_length, uint8_t *tar, size_t tar_length)
    {
		if (_kwp2000_handle == NULL)
			return 0;

        return ((JM::KWP2000*)(_kwp2000_handle))->unpack(src, src_length, tar, tar_length);
    }

    int32_t jm_kwp2000_addr_init(uint8_t addr_code)
    {
		if (_kwp2000_handle == NULL)
			return JM_ERROR_GENERIC;

        return ((JM::KWP2000*)(_kwp2000_handle))->addrInit(addr_code);
    }

    int32_t jm_kwp2000_fast_init(const uint8_t *data, size_t count)
    {
		if (_kwp2000_handle == NULL)
			return JM_ERROR_GENERIC;

        return ((JM::KWP2000*)(_kwp2000_handle))->fastInit(data, count);
    }

    int32_t jm_kwp2000_set_lines(int32_t com_line, boolean_t l_line)
    {
		if (_kwp2000_handle == NULL)
			return JM_ERROR_GENERIC;

        return ((JM::KWP2000*)(_kwp2000_handle))->setLines(com_line, l_line ? true : false);
    }

    int32_t jm_kwp2000_set_options(JMKWPMode msg_mode, JMKWPMode link_mode, int32_t baud)
    {
		if (_kwp2000_handle == NULL)
			return JM_ERROR_GENERIC;

        return ((JM::KWP2000*)(_kwp2000_handle))->setOptions(msg_mode, link_mode, baud);
    }

    int32_t jm_kwp2000_set_address(uint8_t target, uint8_t source)
    {
		if (_kwp2000_handle == NULL)
			return JM_ERROR_GENERIC;

        return ((JM::KWP2000*)(_kwp2000_handle))->setAddress(target, source);
    }

    /************************************************************************/
    /* KWP1281                                                              */
    /************************************************************************/
    static JM::KWP1281 *_kwp1281_handle = NULL;
    
    void jm_kwp1281_set_handler(pointer_t handle)
    {
        _kwp1281_handle = (JM::KWP1281*)handle;
    }

    size_t jm_kwp1281_pack(const uint8_t *src, size_t src_length, uint8_t *tar, size_t tar_length)
    {
		if (_kwp1281_handle == NULL)
			return 0;
        return ((JM::KWP1281*)(_kwp1281_handle))->pack(src, src_length, tar, tar_length);
    }

    size_t jm_kwp1281_unpack(const uint8_t *src, size_t src_length, uint8_t *tar, size_t tar_length)
    {
		if (_kwp1281_handle == NULL)
			return 0;

        return ((JM::KWP1281*)(_kwp1281_handle))->unpack(src, src_length, tar, tar_length);
    }

    int32_t jm_kwp1281_addr_init(uint8_t addr_code)
    {
		if (_kwp1281_handle == NULL)
			return JM_ERROR_GENERIC;

        return ((JM::KWP1281*)(_kwp1281_handle))->addrInit(addr_code);
    }

    int32_t jm_kwp1281_set_lines(int32_t com_line, boolean_t l_line)
    {
		if (_kwp1281_handle == NULL)
			return JM_ERROR_GENERIC;

        return ((JM::KWP1281*)(_kwp1281_handle))->setLines(com_line, l_line ? true : false);
    }

    /************************************************************************/
    /* Mikuni Protocol                                                      */
    /************************************************************************/
    static JM::Mikuni *_mikuni_handle = NULL;

    void jm_mikuni_set_handler(pointer_t handle)
    {
        _mikuni_handle = (JM::Mikuni*)handle;
    }

    size_t jm_mikuni_pack(const uint8_t *src, size_t src_length, uint8_t *tar, size_t tar_length)
    {
		if (_mikuni_handle == NULL)
			return 0;

        return ((JM::Mikuni*)(_mikuni_handle))->pack(src, src_length, tar, tar_length);
    }

    size_t jm_mikuni_unpack(const uint8_t *src, size_t src_length, uint8_t *tar, size_t tar_length)
    {
		if (_mikuni_handle == NULL)
			return 0;

        return ((JM::Mikuni*)(_mikuni_handle))->unpack(src, src_length, tar, tar_length);
    }

    int32_t jm_mikuni_init(void)
    {
		if (_mikuni_handle == NULL)
			return JM_ERROR_GENERIC;

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

    void jm_ui_msg_box_add_btn(const char *text)
    {
        JM::UserInterface::inst().msgBoxAddBtn(text);
    }

    void jm_ui_msg_box_set_msg(const char *text)
    {
        JM::UserInterface::inst().msgBoxSetMsg(text);
    }

    void jm_ui_msg_box_set_title(const char *text)
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

    void jm_ui_list_box_add_btn(const char *text)
    {
        JM::UserInterface::inst().listBoxAddBtn(text);
    }

    void jm_ui_list_box_add_item(const char *caption, const char *item)
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

    void jm_ui_menu_add_item(const char *text)
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

    void jm_ui_tc_add_item(const char *code, const char *text)
    {
        JM::UserInterface::inst().tcAddItem(code, text);
    }

    void jm_ui_tc_add_btn(const char *text)
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

    void jm_ui_ld_add_btn(const char *text)
    {
        JM::UserInterface::inst().ldAddBtn(text);
    }

    void jm_ui_ld_set_value(int32_t index, const char *value)
    {
        JM::UserInterface::inst().ldSetValue(index, value);
    }

    char* jm_ui_get_btn_clicked(boolean_t is_blocked)
    {
        return _jm_std_string_to_char(JM::UserInterface::inst().btnClicked(is_blocked ? true : false));
    }

    void jm_ui_set_btn_clicked(const char *text)
    {
        JM::UserInterface::inst().setBtnClicked(text);
    }

    char* jm_ui_get_menu_selected(void)
    {
        return _jm_std_string_to_char(JM::UserInterface::inst().menuSelected());
    }

    void jm_ui_set_menu_selected(const char *text)
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
	void jm_ld_array_update_global_array(boolean_t showed)
	{
		JM::LiveDataVector::updateGlobalArray(showed ? true : false);
	}

	int32_t jm_ld_array_next_showed_index(void)
	{
		return JM::LiveDataVector::nextShowedIndex();
	}

	int32_t jm_ld_array_get_showed_index(int32_t index)
	{
		return JM::LiveDataVector::getShowedIndex(index);
	}

	int32_t jm_ld_array_query_showed_position(int32_t index)
	{
		return JM::LiveDataVector::queryShowedPosition(index);
	}

	int32_t jm_ld_array_get_enabled_index(int32_t index)
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

	void jm_ld_array_set_short_name(int32_t index, const char *short_name)
	{
		JM::LiveDataVector::globalAt(index)->setShortName(std::string(short_name));
	}

	const char* jm_ld_array_get_short_name(int32_t index)
	{
		static std::string ret;
		ret = JM::LiveDataVector::globalAt(index)->shortName().c_str();
		return ret.c_str();
	}

	void jm_ld_array_set_content(int32_t index, const char *content)
	{
		JM::LiveDataVector::globalAt(index)->setContent(std::string(content));
	}

	const char* jm_ld_array_get_content(int32_t index)
	{
		static std::string ret;
		ret = JM::LiveDataVector::globalAt(index)->content().c_str();
		return ret.c_str();
	}

	void jm_ld_array_set_unit(int32_t index, const char *unit)
	{
		JM::LiveDataVector::globalAt(index)->setUnit(std::string(unit));
	}

	const char* jm_ld_array_get_unit(int32_t index)
	{
		static std::string ret;
		ret = JM::LiveDataVector::globalAt(index)->unit().c_str();
		return ret.c_str();
	}

	void jm_ld_array_set_default_value(int32_t index, const char *value)
	{
		JM::LiveDataVector::globalAt(index)->setDefaultValue(std::string(value));
	}

	const char* jm_ld_array_get_default_value(int32_t index)
	{
		static std::string ret;
		ret = JM::LiveDataVector::globalAt(index)->defaultValue().c_str();
		return ret.c_str();
	}

	void jm_ld_array_set_value(int32_t index, const char *value)
	{
		JM::LiveDataVector::globalAt(index)->setValue(std::string(value));
	}

	const char* jm_ld_array_get_value(int32_t index)
	{
		static std::string ret;
		ret = JM::LiveDataVector::globalAt(index)->value().c_str();
		return ret.c_str();
	}

	void jm_ld_array_set_command_id(int32_t index, int32_t id)
	{
		JM::LiveDataVector::globalAt(index)->setCmdID(id);
	}

	int32_t jm_ld_array_get_command_id(int32_t index)
	{
		return JM::LiveDataVector::globalAt(index)->cmdID();
	}

	void jm_ld_array_set_enabled(int32_t index, boolean_t enabled)
	{
		JM::LiveDataVector::setEnabled(index, enabled ? true : false);
	}

	boolean_t jm_ld_array_get_enabled(int32_t index)
	{
		return JM::LiveDataVector::globalAt(index)->enabled();
	}

	void jm_ld_array_set_showed(int32_t index, boolean_t show)
	{
		return JM::LiveDataVector::setShowed(index, show ? true : false);
	}

	boolean_t jm_ld_array_get_showed(int32_t index)
	{
		return JM::LiveDataVector::globalAt(index)->showed();
	}

	/************************************************************************/
	/* JM Diagnose                                                          */
	/************************************************************************/
	char* jm_diag_calc_std_obd_dtc(const uint8_t *buff, size_t buff_length, int32_t pos, int32_t factor, int32_t offset)
	{
		std::string code = JM::Diag::inst().calcStdOBDTroubleCode(boost::asio::const_buffer(buff, buff_length), pos, factor, offset);
		return _jm_std_string_to_char(code);
	}

    /************************************************************************/
    /* JM Lib                                                               */
    /************************************************************************/
    void jm_lib_init(const char *software_path)
    {
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

