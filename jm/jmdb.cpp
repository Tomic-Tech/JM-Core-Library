#include "jmdb.hpp"

namespace JM
{
	Database::Database()
		: _tcCatalog()
		, _ldCatalog()
		, _cmdCatalog()
		, _handle(NULL)
		, _getTextStmt(NULL)
		, _getTroubleCodeStmt(NULL)
		, _getCommandStmt(NULL)
		, _getCommandByIDStmt(NULL)
		, _getLiveDataStmt(NULL)
	{
		sqlite3_enable_shared_cache(1);
	}

	Database::~Database()
	{
		close();
	}

	Database& Database::inst()
	{
		static Database instance;
		return instance;
	}

	bool Database::open(const std::string &filePath, const std::string &password)
	{
		close();

		if (sqlite3_open_v2(filePath.c_str(), &_handle, SQLITE_OPEN_READONLY, NULL) != SQLITE_OK)
		{
			_handle = NULL;
			return false;
		}

		sqlite3_busy_timeout(_handle, 5000);
		sqlite3_key(_handle, password.c_str(), password.size());
		return true;
	}

	bool Database::close()
	{
		int ret;
		sqlite3_stmt *stmt = NULL;
		_getTextStmt = NULL;
		_getTroubleCodeStmt = NULL;
		_getCommandStmt = NULL;
		_getCommandByIDStmt = NULL;
		_getLiveDataStmt = NULL;

		if (_handle == NULL)
		{
			return true;
		}
		while ((stmt = sqlite3_next_stmt(_handle, 0)) != NULL)
		{
			ret = sqlite3_finalize(stmt);
			if (ret != SQLITE_OK)
			{
				return false;
			}
		}
		ret = sqlite3_close(_handle);
		if (ret != SQLITE_OK)
		{
			return false;
		}
		_handle = NULL;
		return true;
	}

	bool Database::isOpen()
	{
		return _handle != NULL ? true : false;
	}

	void Database::setTCCatalog(const std::string &catalog)
	{
		_tcCatalog = catalog;
	}

	void Database::setLDCatalog(const std::string &catalog)
	{
		_ldCatalog = catalog;
	}

	void Database::setCMDCatalog(const std::string &catalog)
	{
		_cmdCatalog = catalog;
	}

	std::string Database::getText(const std::string &name)
	{
		std::string result(name);
		if (_getTextStmt == NULL)
		{
			std::string lang = JM::Auth::inst().decrypt(JM_AUTH_DE_LANG);
			int ret = 0;
			std::string text = "SELECT Content FROM [Text";
			text += lang;
			text += "] WHERE Name=:name";

			ret = sqlite3_prepare_v2(_handle, text.c_str(), text.size(), &_getTextStmt, NULL);
			if (ret != SQLITE_OK)
			{
				sqlite3_finalize(_getTextStmt);
				_getTextStmt = NULL;
				return result;
			}
		}

		if ((sqlite3_reset(_getTextStmt) == SQLITE_OK) && 
			(sqlite3_bind_text(_getTextStmt, 1, name.c_str(), name.size(), SQLITE_STATIC) == SQLITE_OK) && 
			(sqlite3_step(_getTextStmt) == SQLITE_ROW))
		{
			const char* text = (const char*)sqlite3_column_text(_getTextStmt, 0);
			if (text != NULL)
				result = text;
		}

		return result;
	}

	std::string Database::getTroubleCode(const std::string code)
	{
		std::string result(code);

		if (_getTroubleCodeStmt == NULL)
		{
			std::string lang = JM::Auth::inst().decrypt(JM_AUTH_DE_LANG);
			int ret = 0;
			std::string text = "SELECT Content FROM [TroubleCode";
			text += lang;
			text += "] WHERE Code=:code AND Catalog=:catalog";

			ret = sqlite3_prepare_v2(_handle, text.c_str(), text.size(), &_getTroubleCodeStmt, NULL);
			if (ret != SQLITE_OK)
			{
				sqlite3_finalize(_getTroubleCodeStmt);
				_getTroubleCodeStmt = NULL;
				return result;
			}
		}

		if ((sqlite3_reset(_getTroubleCodeStmt) == SQLITE_OK) &&
			(sqlite3_bind_text(_getTroubleCodeStmt, 1, code.c_str(), code.size(), SQLITE_STATIC) == SQLITE_OK) && 
			(sqlite3_bind_text(_getTroubleCodeStmt, 2, _tcCatalog.c_str(), _tcCatalog.size(), SQLITE_STATIC) == SQLITE_OK))
		{
			int rc = sqlite3_step(_getTroubleCodeStmt);
			if (rc == SQLITE_ROW)
			{
				const char* text = (const char*)sqlite3_column_text(_getTroubleCodeStmt, 0);
				result = text ? text : JM::System::inst().text("Undefined");
			}
			else if (rc == SQLITE_DONE)
			{
				result = JM::System::inst().text("Undefined");
			}
		}

		return result;
	}

	boost::asio::const_buffer Database::getCommand(const std::string &name)
	{
		if (_getCommandStmt == NULL)
		{
			static char *text = "SELECT Command FROM [Command] WHERE Name=:name AND Catalog=:catalog";
			int ret;
			ret = sqlite3_prepare_v2(_handle, text, strlen(text), &_getCommandStmt, NULL);
			if (ret != SQLITE_OK)
			{
				sqlite3_finalize(_getCommandStmt);
				_getCommandStmt = NULL;
				return boost::asio::const_buffer();
			}
		}
		if ((sqlite3_reset(_getCommandStmt) == SQLITE_OK) && 
			(sqlite3_bind_text(_getCommandStmt, 1, name.c_str(), name.size(), SQLITE_STATIC) == SQLITE_OK) &&
			(sqlite3_bind_text(_getCommandStmt, 2, _cmdCatalog.c_str(), _cmdCatalog.size(), SQLITE_STATIC) == SQLITE_OK) && 
			(sqlite3_step(_getCommandStmt) == SQLITE_ROW))
		{
			const boost::uint8_t *bytes = (const boost::uint8_t*)sqlite3_column_blob(_getCommandStmt, 0);
			boost::int32_t size = sqlite3_column_bytes(_getCommandStmt, 0);
			return boost::asio::const_buffer(bytes, size);
		}
		return boost::asio::const_buffer();
	}

	boost::asio::const_buffer Database::getCommandByID(boost::int32_t id)
	{
		if (_getCommandByIDStmt == NULL)
		{
			static char *text = "SELECT Command FROM [Command] WHERE ID=:id";
			int ret = sqlite3_prepare_v2(_handle, text, strlen(text), &_getCommandByIDStmt, NULL);
			if (ret != SQLITE_OK)
			{
				sqlite3_finalize(_getCommandByIDStmt);
				_getCommandByIDStmt = NULL;
				return boost::asio::const_buffer();
			}
		}

		if ((sqlite3_reset(_getCommandByIDStmt) == SQLITE_OK) &&
			(sqlite3_bind_int(_getCommandByIDStmt, 1, id) == SQLITE_OK) && 
			(sqlite3_step(_getCommandByIDStmt) == SQLITE_ROW))
		{
			const boost::uint8_t *bytes = (const boost::uint8_t*) sqlite3_column_blob(_getCommandByIDStmt, 0);
			boost::int32_t size = sqlite3_column_bytes(_getCommandByIDStmt, 0);
			return boost::asio::const_buffer(bytes, size);
		}
		return boost::asio::const_buffer();
	}

	LiveDataVectorPtr Database::getLiveData(bool showed)
	{
		if (_getLiveDataStmt == NULL)
		{
			std::string lang = JM::Auth::inst().decrypt(JM_AUTH_DE_LANG);
			std::string text = "SELECT ShortName, Content, Unit, DefaultValue, CommandID, AlgorithmID FROM [LiveData";
			text += lang;
			text += "] WHERE Catalog=:catalog";
			
			int ret = sqlite3_prepare_v2(_handle, text.c_str(), text.size(), &_getLiveDataStmt, NULL);

			if (ret != SQLITE_OK)
			{
				sqlite3_finalize(_getLiveDataStmt);
				_getLiveDataStmt = NULL;
				return boost::make_shared<LiveDataVector>();
			}
		}

		if ((sqlite3_reset(_getLiveDataStmt) == SQLITE_OK) &&
			(sqlite3_bind_text(_getLiveDataStmt, 1, _ldCatalog.c_str(), _ldCatalog.size(), SQLITE_STATIC) == SQLITE_OK))
		{
			LiveDataVectorPtr result = boost::make_shared<LiveDataVector>();

			while (sqlite3_step(_getLiveDataStmt) == SQLITE_ROW)
			{
				const char *shortName = (const char*)sqlite3_column_text(_getLiveDataStmt, 0);
				const char *content = (const char*)sqlite3_column_text(_getLiveDataStmt, 1);
				const char *unit = (const char*)sqlite3_column_text(_getLiveDataStmt, 2);
				const char *defaultValue = (const char*)sqlite3_column_text(_getLiveDataStmt, 3);
				boost::int32_t id = sqlite3_column_int(_getLiveDataStmt, 4);
				bool enabled = true;

				LiveDataPtr ld = boost::make_shared<LiveData>(shortName ? shortName : "",
					content ? content : "",
					unit ? unit : "",
					defaultValue ? defaultValue : "",
					id,
					showed);
				result->push_back(ld);
			}
			return result;
		}

		return boost::make_shared<LiveDataVector>();
	}
}