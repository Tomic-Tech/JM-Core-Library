#ifndef JM_DB_HPP
#define JM_DB_HPP

#ifdef _MSC_VER
#pragma once
#endif

#include <sqlite/sqlite3.h>
#include <string>
#include <jm/jm_types.hpp>
#include <jm/jm_byte_array.hpp>
#include <jm/jm_live_data_vector.hpp>

namespace jm {
	class db {
	public:
		static db& instance();
		~db();
		bool open(const std::string &file_path, const std::string &password);
		bool close();
		bool is_open();
		void set_tc_catalog(const std::string &catalog);
		void set_ld_catalog(const std::string &catalog);
		void set_cmd_catalog(const std::string &catalog);
		std::string get_text(const std::string &name);
		std::string get_trouble_code(const std::string &code);
		byte_array get_command(const std::string &name);
		byte_array get_command_id(int32 id);
		live_data_vector_ptr get_live_data();
	private:
		db();
	private:
		const char* _text_query;
		const char* _trouble_code_query;
		const char* _command_query;
		const char* _command_by_id_query;
		const char* _live_data_query;
		std::string _tc_catalog;
		std::string _ld_catalog;
		std::string _cmd_catalog;
		sqlite3 *_db_handle;
		sqlite3_stmt *_get_text_stmt;
		sqlite3_stmt *_get_trouble_code_stmt;
		sqlite3_stmt *_get_command_stmt;
		sqlite3_stmt *_get_command_by_id_stmt;
		sqlite3_stmt *_get_live_data_stmt;
	};
	db& db_inst();
}


#endif