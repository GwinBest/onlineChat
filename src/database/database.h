#pragma once 

#include "mysql_connection.h"

#include <cppconn/driver.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

namespace Database
{
    class DatabaseHelper final
    {
    public:
        DatabaseHelper(const DatabaseHelper&) = delete;
        DatabaseHelper& operator= (const DatabaseHelper&) = delete;

        DatabaseHelper(DatabaseHelper&&) = default;
        DatabaseHelper& operator=(DatabaseHelper&&) = default;

        static DatabaseHelper& GetInstance() noexcept;

        [[nodiscard]] sql::ResultSet* ExecuteQuery(const std::string_view query);
        [[nodiscard]] bool ExecuteUpdate(const std::string_view query);

        [[nodiscard]] sql::Connection* GetConnection() { return _connection; }

        ~DatabaseHelper();

    private:
        DatabaseHelper();

        bool Connect() noexcept;
        void Disconnect() noexcept;

    private:
        sql::Driver* _driver = nullptr;
        sql::Connection* _connection = nullptr;
        sql::Statement* _statement = nullptr;

        enum class DatabaseState : uint8_t
        {
            kDatabaseDisconnected = 0,
            kDatabaseConnected,
        };

        DatabaseState _currentState = DatabaseState::kDatabaseDisconnected;
    };

} // !namespase Database