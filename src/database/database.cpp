#include "database.h"

#include <cstdarg>

namespace Database
{
	DatabaseHelper& DatabaseHelper::GetInstance() noexcept
	{
		static DatabaseHelper instance;

		return instance;
	}

	sql::ResultSet* DatabaseHelper::ExecuteQuery(const std::string query, ...)
	{
		char request[255];

		va_list args;
		va_start(args, query);
		vsnprintf(request, 255, query.c_str(), args);
		va_end(args);

		try
		{
			_statement = _connection->createStatement();
			return _statement->executeQuery(request);
		}
		catch (const sql::SQLException& e)
		{
			std::cout << e.what() << std::endl;
		}

		return nullptr;
	}

	bool DatabaseHelper::ExecuteUpdate(const std::string query, ...)
	{
		char request[255];

		va_list args;
		va_start(args, query);
		vsnprintf(request, 255, query.c_str(), args);
		va_end(args);

		try
		{
			_statement = _connection->createStatement();
			_statement->executeUpdate(request);

			return true;
		}
		catch (const sql::SQLException& e)
		{
			std::cout << e.what() << std::endl;
		}
		
		return false;
	}

	DatabaseHelper::~DatabaseHelper()
	{
		if (_currentState != DatabaseState::kDatabaseDisconnected)
		{
			DatabaseHelper::Disconnect();
		}
	}

	DatabaseHelper::DatabaseHelper()
	{
		if (_currentState != DatabaseState::kDatabaseConnected)
		{
			DatabaseHelper::Connect();
		}
	}

	bool DatabaseHelper::Connect() noexcept
	{
		try
		{
			_driver = get_driver_instance();
			_connection = _driver->connect(_hostName, _userName, _password);
			_connection->setSchema(_schemaName);

			_currentState = DatabaseState::kDatabaseConnected;

			return true;
		}
		catch (const sql::SQLException& e)
		{
			std::cout << e.what() << std::endl;
			}

		return false;
	}

	void DatabaseHelper::Disconnect() noexcept
	{
		delete _statement;
		delete _connection;

		_currentState = DatabaseState::kDatabaseDisconnected;
	}

} // !namespase Database