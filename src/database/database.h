#pragma once 

#include "mysql_connection.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

namespace Database
{
    class DatabaseHelper
    {
    public:
        DatabaseHelper(const DatabaseHelper&) = delete;
        void operator= (const DatabaseHelper&) = delete;

        static DatabaseHelper& GetInstance() noexcept;

        [[nodiscard]] sql::ResultSet* ExecuteQuery(const std::string query, ...);
        bool ExecuteUpdate(const std::string  query, ...);

        ~DatabaseHelper();

    private:
        DatabaseHelper();

        bool Connect() noexcept;
        void Disconnect() noexcept;

    private:
        sql::Driver* _driver = nullptr;
        sql::Connection* _connection = nullptr;
        sql::Statement* _statement = nullptr;

        const sql::SQLString _hostName = "127.0.0.1:3306";
        const sql::SQLString _userName = "root";
        const sql::SQLString _password = "admin";
        const sql::SQLString _schemaName = "onlinechat";

        enum class DatabaseState : uint8_t
        {
            kDatabaseDisconnected = 0,
            kDatabaseConnected,
        };

        DatabaseState _currentState = DatabaseState::kDatabaseDisconnected;
    };

} // !namespase Database