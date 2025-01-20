﻿#include "database.h"

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
        va_list args = nullptr;
        va_start(args, query);
        const int size = vsnprintf(nullptr, 0, query.c_str(), args);

        std::vector<char> request(size + 1);;

        vsnprintf(request.data(), request.size(), query.c_str(), args);
        va_end(args);

        try
        {
            _statement = _connection->createStatement();
            return _statement->executeQuery(request.data());
        }
        catch (const sql::SQLException& e)
        {
            std::cout << e.what() << '\n';
            throw;
        }
    }

    bool DatabaseHelper::ExecuteUpdate(const std::string query, ...)
    {
        va_list args = nullptr;
        va_start(args, query);
        const int size = vsnprintf(nullptr, 0, query.c_str(), args);
        va_end(args);

        va_start(args, query);
        std::vector<char> request(size + 1);
        vsnprintf(request.data(), request.size(), query.c_str(), args);
        va_end(args);

        try
        {
            _statement = _connection->createStatement();
            _statement->executeUpdate(request.data());

            return true;
        }
        catch (const sql::SQLException& e)
        {
            std::cout << e.what() << '\n';
            std::cout << request.data() << '\n';
            throw;
        }
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
            _connection = _driver->connect(
                std::getenv("CHAT_DB_HOST"),
                std::getenv("CHAT_DB_USER"),
                std::getenv("CHAT_DB_PASSWORD"));
            _connection->setSchema(std::getenv("CHAT_DB_SCHEMA"));

            _currentState = DatabaseState::kDatabaseConnected;

            return true;
        }
        catch (const sql::SQLException& e)
        {
            std::cout << e.what() << '\n';
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