#include "server.h"

#ifdef _WIN32
#include <WS2tcpip.h>
#endif // WIN32

#include <array>
#include <bit>
#include <chrono>
#include <iostream>
#include <string>
#include <thread>
#include <memory>

#include "common/common.h"
#include "database/database.h"
#include "messageBuffer/messageBuffer.h"

namespace ServerNetworking
{
    Server::Server()
    {
        _connections.fill(INVALID_SOCKET);
    }

    bool Server::Start()
    {
#ifdef WIN32
        if (WSAStartup(NetworkCore::dllVersion, &_wsaData) != 0)
        {
            std::cerr << "WSA Startup Error: " << WSAGetLastError() << '\n';
            return false;
        }
#endif // WIN32

        _socketAddress.sin_addr.s_addr = inet_addr(NetworkCore::ipAddress.data());
        _socketAddress.sin_port = htons(NetworkCore::port);
        _socketAddress.sin_family = AF_INET;

        _serverSocket = socket(AF_INET, SOCK_STREAM, NULL);
        if (bind(_serverSocket,
                 std::bit_cast<SOCKADDR*>(&_socketAddress),
                 sizeof(_socketAddress)) != 0)
        {
            std::cerr << "bind error: " << WSAGetLastError() << '\n';
            return false;
        }

        if (listen(_serverSocket, SOMAXCONN) != 0)
        {
            std::cerr << "listen error: " << WSAGetLastError() << '\n';
            return false;
        }

        std::cout << "Server started successfully\n";
        return true;
    }

    void Server::Run()
    {
        static int sizeOfServerAddress = sizeof(_socketAddress);

        while (true)
        {
            const SOCKET newConnection = accept(_serverSocket,
                                                std::bit_cast<SOCKADDR*>(&_socketAddress),
                                                &sizeOfServerAddress);

            if (newConnection == INVALID_SOCKET)
            {
                std::cerr << "newConnection error: " << WSAGetLastError() << '\n';
                continue;
            }

            if (std::array<char, INET_ADDRSTRLEN> clientIp = {};
                inet_ntop(AF_INET, &_socketAddress.sin_addr, clientIp.data(), INET_ADDRSTRLEN) == nullptr)
            {
                std::cerr << "inet_ntop error: " << WSAGetLastError() << '\n';
            }
            else
            {
                std::cout << "Client Connected: " << clientIp.data() << '\n';
            }

            if (auto it = std::ranges::find(_connections.begin(), _connections.end(), INVALID_SOCKET);
                it != _connections.end())
            {
                *it = newConnection;
                ++_connectionsCurrentCount;

                std::thread(&Server::ClientHandler, this, newConnection).detach();
            }
            else
            {
                std::cerr << "No available slot for new connection\n";
                closesocket(newConnection);
            }
        }
    }

    void Server::ClientHandler(const SOCKET clientSocket)
    {
        auto actionType = NetworkCore::ActionType::kActionUndefined;

        while (true)
        {
            if (const int32_t recvReturnValue = recv(
                clientSocket,
                reinterpret_cast<char*>(&actionType),
                sizeof(actionType),
                0);
                recvReturnValue <= 0)
            {
                if (const auto it = std::ranges::find(_connections.begin(), _connections.end(), clientSocket);
                    it != _connections.end())
                {
                    *it = INVALID_SOCKET;
                }

                if (closesocket(clientSocket) != 0)
                {
                    std::cout << "close socket error: " << WSAGetLastError() << '\n';
                }
                else
                {
                    std::cout << "Client disconnected\n";
                }

                break;
            }

            HandleAction(clientSocket, actionType);
        }
    }

    void Server::HandleAction(const SOCKET clientSocket, const NetworkCore::ActionType actionType)
    {
        switch (actionType)
        {
            using enum NetworkCore::ActionType;

        case kCreateNewPersonalChat:
            HandleCreateNewPersonalChat(clientSocket);
            break;
        case kSendChatMessage:
            HandleSendChatMessage(clientSocket);
            break;
        case kAddUserCredentialsToDatabase:
            HandleAddUserCredentialsToDatabase(clientSocket);
            break;
        case kCheckUserExistence:
            HandleCheckUserExistence(clientSocket);
            break;
        case kCheckIsUserDataFromFileValid:
            HandleCheckIsUserDataFromFileValid(clientSocket);
            break;
        case kGetUserNameFromDatabase:
            HandleGetUserNameFromDatabase(clientSocket);
            break;
        case kGetUserIdFromDatabase:
            HandleGetUserIdFromDatabase(clientSocket);
            break;
        case kFindMatchingChats:
            HandleFindMatchingChats(clientSocket);
            break;
        case kGetAvailableChatsForUser:
            HandleGetAvailableChatsForUser(clientSocket);
            break;
        case kReceiveAllMessagesForSelectedChat:
            HandleReceiveAllMessagesForSelectedChat(clientSocket);
            break;
        default:
            std::cerr << "Undefined action type\n";
            break;
        }
    }

    void Server::HandleCreateNewPersonalChat(const SOCKET clientSocket)
    {
        size_t senderUserId = 0;
        recv(clientSocket, reinterpret_cast<char*>(&senderUserId), sizeof(senderUserId), NULL);

        size_t receiverUserNameSize = 0;
        std::array<char, Common::maxInputBufferSize> receiverUserName = {};
        recv(clientSocket, reinterpret_cast<char*>(&receiverUserNameSize), sizeof(receiverUserNameSize), NULL);
        recv(clientSocket, receiverUserName.data(), static_cast<int>(receiverUserNameSize), NULL);
        receiverUserName[receiverUserNameSize] = '\0';

        try
        {
            auto& dbHelper = Database::DatabaseHelper::GetInstance();
            auto* connection = dbHelper.GetConnection();
            connection->setAutoCommit(false);
            bool result = false;

            auto query = std::format(
                "INSERT INTO chats (name, photo, created_at) "
                "SELECT NULL, NULL, NOW() "
                "WHERE NOT EXISTS ("
                "    SELECT 1 FROM chats WHERE name = '{}'"
                ");",
                receiverUserName.data());

            result = dbHelper.ExecuteUpdate(query);

            size_t chatId = 0;
            if (auto* resultSet = dbHelper.ExecuteQuery("SELECT LAST_INSERT_ID() AS id;");
                resultSet->next())
            {
                chatId = resultSet->getInt("id");

                query = std::format(
                    "INSERT INTO users_chats (user_id, chat_id, joined_at) "
                    "SELECT {}, {}, NOW() "
                    "WHERE NOT EXISTS ("
                    "    SELECT 1 FROM users_chats WHERE user_id = {} AND chat_id = {}"
                    ");",
                    senderUserId, chatId,
                    senderUserId, chatId);

                result = dbHelper.ExecuteUpdate(query);

                query = std::format(
                    "INSERT INTO users_chats (user_id, chat_id, joined_at) "
                    "SELECT (SELECT id FROM users WHERE name = '{}' LIMIT 1), {}, NOW() "
                    "WHERE NOT EXISTS ("
                    "    SELECT 1 FROM users_chats WHERE user_id = (SELECT id FROM users WHERE name = '{}' LIMIT 1) AND chat_id = {});"
                    ");",
                    receiverUserName.data(), chatId,
                    receiverUserName.data(), chatId);

                result = dbHelper.ExecuteUpdate(query);
            }
            connection->commit();

            static constexpr auto actionType = NetworkCore::ActionType::kCreateNewPersonalChat;

            send(clientSocket, reinterpret_cast<const char*>(&actionType), sizeof(actionType), NULL);
            send(clientSocket, reinterpret_cast<const char*>(&chatId), sizeof(chatId), NULL);
        }
        catch (const sql::SQLException& e)
        {
            std::cerr << "kCreateNewPersonalChat error: " << e.what() << '\n';
            SendServerErrorMessage(clientSocket, "Server error: server cant create new personal chat");
            Database::DatabaseHelper::GetInstance().GetConnection()->rollback();
        }
    }

    void Server::HandleSendChatMessage(const SOCKET clientSocket)
    {
        size_t chatId = 0;
        recv(clientSocket, reinterpret_cast<char*>(&chatId), sizeof(chatId), NULL);

        size_t senderUserId = 0;
        recv(clientSocket, reinterpret_cast<char*>(&senderUserId), sizeof(senderUserId), NULL);

        size_t messageSize = 0;
        std::array<char, Common::maxInputBufferSize> message = {};
        recv(clientSocket, reinterpret_cast<char*>(&messageSize), sizeof(messageSize), NULL);
        recv(clientSocket, message.data(), static_cast<int>(messageSize), NULL);
        message[messageSize] = '\0';

        try
        {
            auto& dbHelper = Database::DatabaseHelper::GetInstance();
            auto* connection = dbHelper.GetConnection();
            connection->setAutoCommit(false);

            auto query = std::format(
                "INSERT INTO chats (name, photo, created_at) "
                "SELECT NULL, NULL, NOW() "
                "WHERE NOT EXISTS ("
                "    SELECT 1 FROM chats WHERE id = {}"
                ");",
                chatId
            );
            bool result = dbHelper.ExecuteUpdate(query);

            query = std::format(
                "INSERT INTO users_chats (user_id, chat_id, joined_at) "
                "SELECT {}, "
                "       (SELECT id FROM chats WHERE id = {} LIMIT 1), "
                "       NOW() "
                "WHERE NOT EXISTS ("
                "    SELECT 1 FROM users_chats WHERE user_id = {} AND chat_id = {}"
                ");",
                senderUserId, chatId,
                senderUserId, chatId
            );
            result = dbHelper.ExecuteUpdate(query);

            query = std::format(
                "INSERT INTO messages (chat_id, user_id, content, sent_at) "
                "VALUES ("
                "    (SELECT id FROM chats WHERE id = {} LIMIT 1), "
                "    {}, '{}', NOW()"
                ");",
                chatId, senderUserId, message.data()
            );
            result = dbHelper.ExecuteUpdate(query);

            connection->commit();

            query = std::format(
                "SELECT users_chats.user_id, messages.sent_at "
                "FROM users_chats "
                "JOIN messages ON users_chats.chat_id = messages.chat_id "
                "WHERE users_chats.chat_id = {} AND users_chats.user_id != {} "
                "ORDER BY messages.sent_at DESC LIMIT 1;",
                chatId,
                senderUserId);
            auto* resultSet = dbHelper.ExecuteQuery(query);

            static constexpr auto actionType = NetworkCore::ActionType::kSendChatMessage;
            while (resultSet->next())
            {
                const UserId receiverUserId = resultSet->getInt("user_id");
                const std::string sentAt = resultSet->getString("sent_at");

                if (auto it = _connectionsToUserId.find(receiverUserId);
                    it != _connectionsToUserId.end())
                {
                    const SOCKET receiverSocket = it->second;

                    send(receiverSocket, reinterpret_cast<const char*>(&actionType), sizeof(actionType), NULL);

                    const size_t sendMessageSize = message.size();
                    send(receiverSocket, reinterpret_cast<const char*>(&sendMessageSize), sizeof(sendMessageSize), NULL);
                    send(receiverSocket, message.data(), static_cast<int>(sendMessageSize), NULL);

                    const size_t sendTimeSize = sentAt.size();
                    send(receiverSocket, reinterpret_cast<const char*>(&sendTimeSize), sizeof(sendTimeSize), NULL);
                    send(receiverSocket, sentAt.data(), static_cast<int>(sendTimeSize), NULL);
                }
            }
        }
        catch (const sql::SQLException& e)
        {
            std::cerr << "kSendChatMessage error: " << e.what() << '\n';
            SendServerErrorMessage(clientSocket, "Server error: server cant push message to database");

            Database::DatabaseHelper::GetInstance().GetConnection()->rollback();
        }
    }

    void Server::HandleAddUserCredentialsToDatabase(const SOCKET clientSocket)
    {
        const NetworkCore::UserPacket userPacket = ReceiveUserCredentialsPacket(clientSocket);

        if (userPacket.name.empty())
        {
            SendServerErrorMessage(clientSocket, "Server error: username is empty");
            return;
        }

        if (userPacket.login.empty())
        {
            SendServerErrorMessage(clientSocket, "Server error: user login is empty");
            return;
        }

        if (userPacket.password == 0)
        {
            SendServerErrorMessage(clientSocket, "Server error: user password is empty");
            return;
        }

        bool result = false;
        static constexpr auto actionType = NetworkCore::ActionType::kAddUserCredentialsToDatabase;

        try
        {
            auto query = std::format(
                "INSERT INTO users(name, login, password) "
                "VALUES('{}', '{}', {});",
                userPacket.name.c_str(),
                userPacket.login.c_str(),
                userPacket.password);
            result = Database::DatabaseHelper::GetInstance().ExecuteUpdate(query);

            send(clientSocket, reinterpret_cast<const char*>(&actionType), sizeof(actionType), NULL);
            send(clientSocket, reinterpret_cast<char*>(&result), sizeof(result), NULL);
        }
        catch (const sql::SQLException& e)
        {
            std::cerr << "AddUserCredentialsToDatabase error: " << e.what() << '\n';

            send(clientSocket, reinterpret_cast<const char*>(&actionType), sizeof(actionType), NULL);
            send(clientSocket, reinterpret_cast<char*>(&result), sizeof(result), NULL);

            SendServerErrorMessage(clientSocket, "Server error: server cant add user credentials to database");

            throw;
        }
    }

    void Server::HandleCheckUserExistence(const SOCKET clientSocket)
    {
        const NetworkCore::UserPacket userPacket = ReceiveUserCredentialsPacket(clientSocket);

        if (userPacket.login.empty())
        {
            SendServerErrorMessage(clientSocket, "Server error: user login is empty");
            return;
        }

        if (userPacket.password == 0)
        {
            SendServerErrorMessage(clientSocket, "Server error: user password is empty");
            return;
        }

        try
        {
            auto query = std::format(
                "SELECT * FROM users "
                "WHERE login = '{}' AND password = {};",
                userPacket.login.c_str(),
                userPacket.password);
            auto* resultSet = Database::DatabaseHelper::GetInstance().ExecuteQuery(query);

            bool result = false;
            if (resultSet->next()) result = true;

            static constexpr auto actionType = NetworkCore::ActionType::kCheckUserExistence;

            send(clientSocket, reinterpret_cast<const char*>(&actionType), sizeof(actionType), NULL);
            send(clientSocket, reinterpret_cast<char*>(&result), sizeof(result), NULL);
        }
        catch (const sql::SQLException& e)
        {
            std::cerr << "CheckUserExistence error: " << e.what() << '\n';

            SendServerErrorMessage(clientSocket, "Server error: server cant check user existence");
        }
    }

    void Server::HandleCheckIsUserDataFromFileValid(const SOCKET clientSocket)
    {
        const NetworkCore::UserPacket userPacket = ReceiveUserCredentialsPacket(clientSocket);

        try
        {
            auto query = std::format(
                "SELECT * FROM users "
                "WHERE name = '{}' AND login = '{}' "
                "AND password = {} AND id = {};",
                userPacket.name.c_str(), userPacket.login.c_str(),
                userPacket.password, userPacket.id);
            auto* resultSet = Database::DatabaseHelper::GetInstance().ExecuteQuery(query);

            bool result = false;
            if (resultSet->next()) result = true;

            static constexpr auto actionType = NetworkCore::ActionType::kCheckIsUserDataFromFileValid;

            send(clientSocket, reinterpret_cast<const char*>(&actionType), sizeof(actionType), NULL);
            send(clientSocket, reinterpret_cast<char*>(&result), sizeof(result), NULL);
        }
        catch (const sql::SQLException& e)
        {
            std::cerr << "kCheckIsUserDataFromFileValid error: " << e.what() << '\n';
            SendServerErrorMessage(clientSocket, "Server error: server cant check is data from file valid");
        }
    }

    void Server::HandleGetUserNameFromDatabase(const SOCKET clientSocket)
    {
        const NetworkCore::UserPacket userPacket = ReceiveUserCredentialsPacket(clientSocket);

        if (userPacket.login.empty())
        {
            SendServerErrorMessage(clientSocket, "Server error: user login is empty");
            return;
        }

        try
        {
            auto query = std::format(
                "SELECT name FROM users "
                "WHERE login = '{}';",
                userPacket.login.c_str());
            auto* resultSet = Database::DatabaseHelper::GetInstance().ExecuteQuery(query);

            std::string result;
            size_t responseSize = result.size();

            if (resultSet->next()) result = resultSet->getString("name");
            responseSize = result.size();

            static constexpr auto actionType = NetworkCore::ActionType::kGetUserNameFromDatabase;

            send(clientSocket, reinterpret_cast<const char*>(&actionType), sizeof(actionType), NULL);
            send(clientSocket, reinterpret_cast<char*>(&responseSize), sizeof(responseSize), NULL);
            send(clientSocket, result.c_str(), static_cast<int>(responseSize), NULL);
        }
        catch (const sql::SQLException& e)
        {
            std::cerr << "GetUserNameFromDatabase error: " << e.what() << '\n';
            SendServerErrorMessage(clientSocket, "Server error: server cant get user name from database");
        }
    }

    void Server::HandleGetUserIdFromDatabase(const SOCKET clientSocket)
    {
        const NetworkCore::UserPacket userPacket = ReceiveUserCredentialsPacket(clientSocket);

        if (userPacket.login.empty())
        {
            SendServerErrorMessage(clientSocket, "Server error: user login is empty");
            return;
        }

        try
        {
            auto query = std::format(
                "SELECT id FROM users "
                "WHERE login = '{}';",
                userPacket.login.c_str());
            auto* resultSet = Database::DatabaseHelper::GetInstance().ExecuteQuery(query);

            size_t userId = 0;
            if (resultSet->next()) userId = resultSet->getInt("id");

            static constexpr auto actionType = NetworkCore::ActionType::kGetUserIdFromDatabase;

            send(clientSocket, reinterpret_cast<const char*>(&actionType), sizeof(actionType), NULL);
            send(clientSocket, reinterpret_cast<char*>(&userId), sizeof(userId), NULL);

            _connectionsToUserId[userId] = clientSocket;
        }
        catch (const sql::SQLException& e)
        {
            std::cerr << "GetUserIdFromDatabase error: " << e.what() << '\n';
            SendServerErrorMessage(clientSocket, "Server error: server cant get user name from database");
        }
    }

    void Server::HandleFindMatchingChats(const SOCKET clientSocket)
    {
        try
        {
            const NetworkCore::UserPacket userPacket = ReceiveUserCredentialsPacket(clientSocket);

            auto query = std::format(
                "(SELECT DISTINCT "
                "c.id AS chat_id, "
                "COALESCE(c.name, u2.name) AS chat_name, "
                "COALESCE(c.photo, u2.photo) AS chat_photo, "
                "m.content AS last_message, "
                "m.sent_at AS last_message_time "
                "FROM chats c "
                "JOIN "
                "users_chats uc ON c.id = uc.chat_id "
                "JOIN "
                "users u ON uc.user_id = u.id "
                "LEFT JOIN "
                "users_chats uc2 ON c.id = uc2.chat_id AND uc2.user_id != u.id "
                "LEFT JOIN "
                "users u2 ON uc2.user_id = u2.id "
                "LEFT JOIN "
                "messages m ON c.id = m.chat_id "
                "WHERE "
                "u.id = {} "
                "AND (c.name LIKE '{}%' OR u2.name LIKE '{}%') "
                "AND m.sent_at = ( "
                "SELECT MAX(sent_at) "
                "FROM messages "
                "WHERE chat_id = c.id)) "
                "UNION ( SELECT "
                "NULL AS chat_id, "
                "u.name AS chat_name, "
                "u.photo AS chat_photo, "
                "NULL AS last_message, "
                "NULL AS last_message_time "
                "FROM users u "
                "WHERE u.name LIKE '{}%' "
                "AND u.id != {} "
                "AND NOT EXISTS ( "
                "SELECT 1 "
                "FROM users_chats uc "
                "JOIN chats c ON uc.chat_id = c.id "
                "WHERE uc.user_id = {} "
                "AND c.id IN ( "
                "SELECT chat_id "
                "FROM users_chats "
                "WHERE user_id = u.id)));",
                userPacket.id,
                userPacket.login.c_str(),
                userPacket.login.c_str(),
                userPacket.login.c_str(),
                userPacket.id,
                userPacket.id);
            auto* resultSet = Database::DatabaseHelper::GetInstance().ExecuteQuery(query);

            size_t counter = 0;
            auto chatIdResult = std::make_unique<size_t[]>(resultSet->rowsCount());
            auto chatNameResult = std::make_unique<std::string[]>(resultSet->rowsCount());
            auto chatPhotoResult = std::make_unique<std::string[]>(resultSet->rowsCount());
            auto lastMessageResult = std::make_unique<std::string[]>(resultSet->rowsCount());
            auto lastMessageTimeResult = std::make_unique<std::string[]>(resultSet->rowsCount());

            while (resultSet->next())
            {
                chatIdResult[counter] = resultSet->getInt("chat_id");
                chatNameResult[counter] = resultSet->getString("chat_name");
                chatPhotoResult[counter] = resultSet->getString("chat_photo");
                lastMessageResult[counter] = resultSet->getString("last_message");
                lastMessageTimeResult[counter] = resultSet->getString("last_message_time");

                ++counter;
            }

            static constexpr auto actionType = NetworkCore::ActionType::kFindMatchingChats;

            send(clientSocket, reinterpret_cast<const char*>(&actionType), sizeof(actionType), NULL);
            send(clientSocket, reinterpret_cast<char*>(&counter), sizeof(counter), NULL);

            for (size_t i = 0; i < counter; ++i)
            {
                send(clientSocket, reinterpret_cast<char*>(&chatIdResult[i]), sizeof(chatIdResult[0]), NULL);

                size_t resultLength = chatNameResult[i].size();
                send(clientSocket, reinterpret_cast<char*>(&resultLength), sizeof(resultLength), NULL);
                send(clientSocket, chatNameResult[i].c_str(), static_cast<int>(resultLength), NULL);

                resultLength = lastMessageResult[i].size();
                send(clientSocket, reinterpret_cast<char*>(&resultLength), sizeof(resultLength), NULL);
                send(clientSocket, lastMessageResult[i].c_str(), static_cast<int>(resultLength), NULL);

                resultLength = lastMessageTimeResult[i].size();
                send(clientSocket, reinterpret_cast<char*>(&resultLength), sizeof(resultLength), NULL);
                send(clientSocket, lastMessageTimeResult[i].c_str(), static_cast<int>(resultLength), NULL);

                resultLength = chatPhotoResult[i].size();
                send(clientSocket, reinterpret_cast<char*>(&resultLength), sizeof(resultLength), NULL);
                send(clientSocket, chatPhotoResult[i].c_str(), static_cast<int>(resultLength), NULL);
            }
        }
        catch (const sql::SQLException& e)
        {
            std::cerr << "FindMatchingChats error: " << e.what() << '\n';
            SendServerErrorMessage(clientSocket, "Server error: server cant push message to database");
        }
    }

    void Server::HandleGetAvailableChatsForUser(const SOCKET clientSocket)
    {
        try
        {
            const NetworkCore::UserPacket userPacket = ReceiveUserCredentialsPacket(clientSocket);

            if (userPacket.id == 0)
            {
                SendServerErrorMessage(clientSocket, "Server error: user id is empty");
                return;
            }

            auto query = std::format(
                "SELECT DISTINCT "
                "c.id AS chat_id, "
                "COALESCE(c.name, u2.login) AS chat_name, "
                "COALESCE(c.photo, u2.photo) AS chat_photo, "
                "m.content AS last_message, "
                "m.sent_at AS last_message_time "
                "FROM chats c "
                "JOIN users_chats uc ON c.id = uc.chat_id "
                "JOIN users u ON uc.user_id = u.id "
                "LEFT JOIN users_chats uc2 ON c.id = uc2.chat_id AND uc2.user_id != u.id "
                "LEFT JOIN users u2 ON uc2.user_id = u2.id "
                "LEFT JOIN messages m ON c.id = m.chat_id "
                "WHERE u.id = {} "
                "AND m.sent_at = "
                "(SELECT MAX(sent_at) "
                "FROM messages "
                "WHERE chat_id = c.id);",
                userPacket.id);
            auto* resultSet = Database::DatabaseHelper::GetInstance().ExecuteQuery(query);

            size_t counter = 0;
            auto chatIdResult = std::make_unique<size_t[]>(resultSet->rowsCount());
            auto chatNameResult = std::make_unique<std::string[]>(resultSet->rowsCount());
            auto chatPhotoResult = std::make_unique<std::string[]>(resultSet->rowsCount());
            auto lastMessageResult = std::make_unique<std::string[]>(resultSet->rowsCount());
            auto lastMessageTimeResult = std::make_unique<std::string[]>(resultSet->rowsCount());

            while (resultSet->next())
            {
                chatIdResult[counter] = resultSet->getInt("chat_id");
                chatNameResult[counter] = resultSet->getString("chat_name");
                chatPhotoResult[counter] = resultSet->getString("chat_photo");
                lastMessageResult[counter] = resultSet->getString("last_message");
                lastMessageTimeResult[counter] = resultSet->getString("last_message_time");

                ++counter;
            }

            static constexpr auto actionType = NetworkCore::ActionType::kGetAvailableChatsForUser;

            send(clientSocket, reinterpret_cast<const char*>(&actionType), sizeof(actionType), NULL);
            send(clientSocket, reinterpret_cast<char*>(&counter), sizeof(counter), NULL);

            for (size_t i = 0; i < counter; ++i)
            {
                send(clientSocket, reinterpret_cast<char*>(&chatIdResult[i]), sizeof(chatIdResult[0]), NULL);

                size_t resultLength = chatNameResult[i].size();
                send(clientSocket, reinterpret_cast<char*>(&resultLength), sizeof(resultLength), NULL);
                send(clientSocket, chatNameResult[i].c_str(), static_cast<int>(resultLength), NULL);

                resultLength = lastMessageResult[i].size();
                send(clientSocket, reinterpret_cast<char*>(&resultLength), sizeof(resultLength), NULL);
                send(clientSocket, lastMessageResult[i].c_str(), static_cast<int>(resultLength), NULL);

                resultLength = lastMessageTimeResult[i].size();
                send(clientSocket, reinterpret_cast<char*>(&resultLength), sizeof(resultLength), NULL);
                send(clientSocket, lastMessageTimeResult[i].c_str(), static_cast<int>(resultLength), NULL);

                resultLength = chatPhotoResult[i].size();
                send(clientSocket, reinterpret_cast<char*>(&resultLength), sizeof(resultLength), NULL);
                send(clientSocket, chatPhotoResult[i].c_str(), static_cast<int>(resultLength), NULL);
            }
        }
        catch (const sql::SQLException& e)
        {
            std::cerr << "GetAvailableChatsForUser error: " << e.what() << '\n';
            SendServerErrorMessage(clientSocket, "Server error: server cant get available chats for user");
        }
    }

    void Server::HandleReceiveAllMessagesForSelectedChat(const SOCKET clientSocket)
    {
        size_t userId = 0;
        recv(clientSocket, reinterpret_cast<char*>(&userId), sizeof(userId), NULL);

        size_t id = 0;
        recv(clientSocket, reinterpret_cast<char*>(&id), sizeof(id), NULL);

        if (id == 0)
        {
            SendServerErrorMessage(clientSocket, "Server error: chat id is empty");
            return;
        }

        try
        {
            auto query = std::format(
                "SELECT * "
                "FROM messages "
                "WHERE chat_id = {};",
                id);
            auto* resultSet = Database::DatabaseHelper::GetInstance().ExecuteQuery(query);

            auto messagesResult = std::make_unique<std::string[]>(resultSet->rowsCount());
            auto messageTypeResult = std::make_unique<MessageBuffer::MessageStatus[]>(resultSet->rowsCount());
            auto messagesSendTime = std::make_unique<std::string[]>(resultSet->rowsCount());
            size_t count = 0;

            while (resultSet->next())
            {
                const size_t messageAuthorId = resultSet->getInt("user_id");

                messagesResult[count] = resultSet->getString("content");
                messagesSendTime[count] = resultSet->getString("sent_at");

                if (messageAuthorId == userId)
                {
                    messageTypeResult[count] = MessageBuffer::MessageStatus::kSend;
                }
                else
                {
                    messageTypeResult[count] = MessageBuffer::MessageStatus::kReceived;
                }

                ++count;
            }

            static constexpr auto actionType = NetworkCore::ActionType::kReceiveAllMessagesForSelectedChat;

            send(clientSocket, reinterpret_cast<const char*>(&actionType), sizeof(actionType), NULL);
            send(clientSocket, reinterpret_cast<char*>(&count), sizeof(count), NULL);

            for (size_t i = 0; i < count; ++i)
            {
                size_t msgSize = messagesResult[i].size();
                send(clientSocket, reinterpret_cast<char*>(&msgSize), sizeof(msgSize), NULL);
                send(clientSocket, messagesResult[i].c_str(), msgSize, NULL);
                send(clientSocket, reinterpret_cast<char*>(&messageTypeResult[i]), sizeof(messageTypeResult[i]), NULL);

                size_t sendTimeSize = messagesSendTime[i].size();
                send(clientSocket, reinterpret_cast<char*>(&sendTimeSize), sizeof(sendTimeSize), NULL);
                send(clientSocket, messagesSendTime[i].c_str(), sendTimeSize, NULL);
            }
        }
        catch (const sql::SQLException& e)
        {
            std::cerr << "ReceiveAllMessagesForSelectedChat error: " << e.what() << '\n';
            SendServerErrorMessage(clientSocket, "Server error: server cant receive all messages for selected chat");
        }
    }

    NetworkCore::UserPacket Server::ReceiveUserCredentialsPacket(const SOCKET clientSocket)
    {
        NetworkCore::UserPacket newUserPacket = {};

        size_t userNameLength = 0;
        std::array<char, Common::userNameSize> userName = {};
        recv(clientSocket, reinterpret_cast<char*>(&userNameLength), sizeof(userNameLength), NULL);
        recv(clientSocket, userName.data(), static_cast<int>(userNameLength), NULL);
        userName[userNameLength] = '\0';
        newUserPacket.name = userName.data();

        size_t userLoginLength = 0;
        std::array<char, Common::userLoginSize> userLogin = {};
        recv(clientSocket, reinterpret_cast<char*>(&userLoginLength), sizeof(userLoginLength), NULL);
        recv(clientSocket, userLogin.data(), static_cast<int>(userLoginLength), NULL);
        userLogin[userLoginLength] = '\0';
        newUserPacket.login = userLogin.data();

        size_t userPassword = 0;
        recv(clientSocket, reinterpret_cast<char*>(&userPassword), sizeof(userPassword), NULL);
        newUserPacket.password = userPassword;

        size_t userId = 0;
        recv(clientSocket, reinterpret_cast<char*>(&userId), sizeof(userId), NULL);
        newUserPacket.id = userId;

        return newUserPacket;
    }

    void Server::SendServerErrorMessage(const SOCKET clientSocket, const std::string_view errorMessage) noexcept
    {
        static constexpr auto actionType = NetworkCore::ActionType::kServerError;

        send(clientSocket, reinterpret_cast<const char*>(&actionType), sizeof(actionType), NULL);

        const size_t errorMessageSize = errorMessage.size();
        send(clientSocket, reinterpret_cast<const char*>(&errorMessageSize), sizeof(errorMessageSize), NULL);
        send(clientSocket, errorMessage.data(), static_cast<int>(errorMessageSize), NULL);
    }
} // !namespace ServerNetworking
