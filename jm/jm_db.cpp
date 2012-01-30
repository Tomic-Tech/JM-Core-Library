#include "jm_db.hpp"
#include "jm_auth.hpp"

namespace jm {

	db& db::instance() {
		static db inst;
		return inst;
	}

	db::db() {
		_text_query = "SELECT TextContent FROM Text "
			"WHERE TextName=:textname AND Language=:language";
				_trouble_code_query = "SELECT Content FROM TroubleCode "
			"WHERE Code=:codename AND Catalog=:catalog AND Language=:language";
		_command_query = "SELECT Command FROM Command "
			"WHERE CommandName=:commandname AND Catalog=:catalog";
		_command_by_id_query = "SELECT Command FROM Command "
			"WHERE CommandID=:id";
		_live_data_query = "SELECT ShortName, Content, Unit, "
			"DefaultValue, CommandID, AlgorithmID FROM live_data "
			"WHERE Catalog=:catalog AND Language=:language";

		sqlite3_enable_shared_cache(true);
		_db_handle = NULL;
		_get_text_stmt = NULL;
		_get_trouble_code_stmt = NULL;
		_get_command_stmt = NULL;
		_get_command_by_id_stmt = NULL;
		_get_live_data_stmt = NULL;
	}

	db::~db() {
		close();
	}

	bool db::open(const std::string &file_path, const std::string &password) {
		close();

		if (sqlite3_open_v2(file_path.c_str(), &_db_handle, SQLITE_OPEN_READONLY, NULL) != SQLITE_OK) {
			_db_handle = NULL;
			return false;
		}
		sqlite3_busy_timeout(_db_handle, 5000);
		sqlite3_key(_db_handle, password.c_str(), password.length());
		if ((sqlite3_prepare_v2(_db_handle, _text_query, strlen(_text_query), &_get_text_stmt, NULL) != SQLITE_OK) ||
			(sqlite3_prepare_v2(_db_handle, _trouble_code_query, strlen(_trouble_code_query), &_get_trouble_code_stmt, NULL) != SQLITE_OK) ||
			(sqlite3_prepare_v2(_db_handle, _command_query, strlen(_command_query), &_get_command_stmt, NULL) != SQLITE_OK) ||
			(sqlite3_prepare_v2(_db_handle, _command_by_id_query, strlen(_command_by_id_query), &_get_command_by_id_stmt, NULL) != SQLITE_OK) ||
			(sqlite3_prepare_v2(_db_handle, _live_data_query, strlen(_live_data_query), &_get_live_data_stmt, NULL) != SQLITE_OK)) {
				sqlite3_finalize(_get_text_stmt);
				sqlite3_finalize(_get_trouble_code_stmt);
				sqlite3_finalize(_get_command_stmt);
				sqlite3_finalize(_get_command_by_id_stmt);
				sqlite3_finalize(_get_live_data_stmt);
				_get_text_stmt = NULL;
				_get_trouble_code_stmt = NULL;
				_get_command_stmt = NULL;
				_get_command_by_id_stmt = NULL;
				_get_live_data_stmt = NULL;
				sqlite3_close(_db_handle);
				_db_handle = NULL;
				return false;
		}
		return true;
	}

	bool db::close() {
		int ret;
		sqlite3_stmt *stmt = NULL;
		while ((stmt = sqlite3_next_stmt(_db_handle, 0)) != NULL) {
			ret = sqlite3_finalize(stmt);
			if (ret != SQLITE_OK) {
				return false;
			}
		}
		ret = sqlite3_close(_db_handle);
		if (ret != SQLITE_OK) {
			return false;
		}
		_db_handle = NULL;
		return true;
	}

	bool db::is_open() {
		return _db_handle != NULL ? true : false;
	}

	void db::set_tc_catalog(const std::string &catalog) {
		_tc_catalog = catalog;
	}

	void db::set_ld_catalog(const std::string &catalog) {
		_ld_catalog = catalog;
	}
	
	void db::set_cmd_catalog(const std::string &catalog) {
		_cmd_catalog = catalog;
	}

	std::string db::get_text(const std::string &name) {
		std::string lang = auth_inst().de_lang();
		if ((sqlite3_reset(_get_text_stmt) == SQLITE_OK) && 
			(sqlite3_bind_text(_get_text_stmt, 1, name.c_str(), name.length(), SQLITE_STATIC) == SQLITE_OK) && 
			(sqlite3_bind_text(_get_text_stmt, 2, lang.c_str(), lang.length(), SQLITE_STATIC) == SQLITE_OK) && 
			(sqlite3_step(_get_text_stmt) == SQLITE_ROW)) {
				const char* text = (const char*)sqlite3_column_text(_get_text_stmt, 0);
				std::string ret(text ? text : "");
				return ret;
		}
		return name;
	}

	std::string db::get_trouble_code(const std::string &code) {
		std::string lang = auth_inst().de_lang();
		if ((sqlite3_reset(_get_trouble_code_stmt) == SQLITE_OK) &&
			(sqlite3_bind_text(_get_trouble_code_stmt, 1, code.c_str(), code.length(), SQLITE_STATIC) == SQLITE_OK) && 
			(sqlite3_bind_text(_get_trouble_code_stmt, 2, _tc_catalog.c_str(), _tc_catalog.length(), SQLITE_STATIC) == SQLITE_OK) && 
			(sqlite3_bind_text(_get_trouble_code_stmt, 3, lang.c_str(), lang.length(), SQLITE_STATIC) == SQLITE_OK)) {
				int rc = sqlite3_step(_get_trouble_code_stmt);
				if (rc == SQLITE_ROW) {
					const char* text = (const char*)sqlite3_column_text(_get_trouble_code_stmt, 0);
					std::string rs(text ? text : "");
					return rs;
				} else if (rc == SQLITE_DONE) {
					std::string rs("Undefined");
					return rs;
				}
		}
		return code;
	}

	byte_array db::get_command(const std::string &name) {
		if ((sqlite3_reset(_get_command_stmt) == SQLITE_OK) && 
			(sqlite3_bind_text(_get_command_stmt, 1, name.c_str(), name.length(), SQLITE_STATIC) == SQLITE_OK) &&
			(sqlite3_bind_text(_get_command_stmt, 2, _cmd_catalog.c_str(), _cmd_catalog.length(), SQLITE_STATIC) == SQLITE_OK) && 
			(sqlite3_step(_get_command_stmt) == SQLITE_ROW)) {
				const uint8* bytes = (const uint8*) sqlite3_column_blob(_get_command_stmt, 0);
				int size = sqlite3_column_bytes(_get_command_stmt, 0);
				byte_array ret;
				for (int i = 0; i < size; i++)
					ret.push_back(bytes[i]);
				return ret;
		}
		return byte_array();
	}

	byte_array db::get_command_id(int32 id) {
		if ((sqlite3_reset(_get_command_by_id_stmt) == SQLITE_OK) &&
			(sqlite3_bind_int(_get_command_by_id_stmt, 1, id) == SQLITE_OK) &&
			(sqlite3_step(_get_command_by_id_stmt) == SQLITE_ROW)) {
				const uint8 *bytes = (const uint8*) sqlite3_column_blob(_get_command_by_id_stmt, 0);
				int size = sqlite3_column_bytes(_get_command_by_id_stmt, 0);
				byte_array rb;
				for (int i  = 0; i < size; i++)
					rb.push_back(bytes[i]);
				return rb;
		}
		return byte_array();
	}

	live_data_vector_ptr db::get_live_data() {
		std::string lang = auth_inst().de_lang();
		live_data_vector_ptr ret = boost::make_shared<live_data_vector>();
		if ((sqlite3_reset(_get_live_data_stmt) == SQLITE_OK) &&
			(sqlite3_bind_text(_get_live_data_stmt, 1, _ld_catalog.c_str(), _ld_catalog.length(), SQLITE_STATIC) == SQLITE_OK) && 
			(sqlite3_bind_text(_get_live_data_stmt, 2, lang.c_str(), lang.length(), SQLITE_STATIC) == SQLITE_OK)) {
				while (sqlite3_step(_get_live_data_stmt) == SQLITE_ROW) {
					const char *short_name = (const char*)sqlite3_column_text(_get_live_data_stmt, 0);
					const char *content = (const char*)sqlite3_column_text(_get_live_data_stmt, 1);
					const char *unit = (const char*)sqlite3_column_text(_get_live_data_stmt, 2);
					const char *default_value = (const char*)sqlite3_column_text(_get_live_data_stmt, 3);
					int id = sqlite3_column_int(_get_live_data_stmt, 4);
					bool enabled = true;
					live_data *ptr = new live_data(short_name ? short_name : "", 
						content ? content : "",
						unit ? unit : "",
						default_value ? default_value : "",
						id,
						enabled);
					live_data_ptr ld_ptr(ptr);
					ret->push_back(ld_ptr);
				}
				return ret;
		}
		return ret;
	}

	db& db_inst() {
		return db::instance();
	}
}