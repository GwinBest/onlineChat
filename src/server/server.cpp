#include "Server.h"

#include <WS2tcpip.h>

#include <chrono>
#include <iostream>
#include <string>
#include <thread>

#include "common/common.h"
#include "database/database.h"
#include "messageBuffer/messageBuffer.h"

namespace ServerNetworking
{
    void Server::Start() noexcept
    {
        if (WSAStartup(NetworkCore::dllVersion, &_wsaData) != 0)
        {
            std::cout << "Error\n";
            exit(SOCKET_ERROR);
        }

        _socketAddress.sin_addr.s_addr = inet_addr(NetworkCore::ipAddress.c_str());
        _socketAddress.sin_port = htons(NetworkCore::port);
        _socketAddress.sin_family = AF_INET;

        _serverSocket = socket(AF_INET, SOCK_STREAM, NULL);
        bind(_serverSocket, reinterpret_cast<SOCKADDR*>(&_socketAddress), sizeof(_socketAddress));
        listen(_serverSocket, SOMAXCONN);
    }

    void Server::Run() noexcept
    {
        int sizeOfServerAddress = sizeof(_socketAddress);
        SOCKET newConnection = 0;

        for (size_t i = 0; i < std::size(_connections); i++)
        {
            newConnection = accept(_serverSocket, reinterpret_cast<SOCKADDR*>(&_socketAddress), &sizeOfServerAddress);

            if (newConnection == 0)
            {
                std::cout << "Error #2\n";
            }
            else
            {
                char clientIP[INET_ADDRSTRLEN] = "";
                inet_ntop(AF_INET, &(_socketAddress.sin_addr), clientIP, INET_ADDRSTRLEN);

                std::cout << "Client Connected! " << _connectionsCurrentCount << " " << clientIP << std::endl;

                _connections[i] = newConnection;
                _connectionsCurrentCount++;

                std::thread clientThread(&Server::ClientHandler, this, i);
                clientThread.detach();
            }
        }
    }

    void Server::ClientHandler(int index)
    {
        NetworkCore::ActionType actionType = NetworkCore::ActionType::kActionUndefined;
        int32_t recvReturnValue = 0;
        sql::ResultSet* resultSet = nullptr;

        while (true)
        {
            recvReturnValue = recv(_connections[index], reinterpret_cast<char*>(&actionType), sizeof(actionType), NULL);

            if (recvReturnValue <= 0)
            {
                closesocket(_connections[index]);
                index--;

                std::cout << GetLastError() << std::endl;
                std::cout << "Client disconnected: " << index << std::endl;

                return;
            }

            switch (actionType)
            {
            case NetworkCore::ActionType::kSendChatMessage:
            {
                size_t chatId = 0;
                recv(_connections[index], reinterpret_cast<char*>(&chatId), sizeof(chatId), NULL);

                size_t senderUserId = 0;
                recv(_connections[index], reinterpret_cast<char*>(&senderUserId), sizeof(senderUserId), NULL);

                size_t messageSize = 0;
                char message[Common::maxInputBufferSize] = "";
                recv(_connections[index], reinterpret_cast<char*>(&messageSize), sizeof(messageSize), NULL);
                recv(_connections[index], message, messageSize, NULL);
                message[messageSize] = '\0';

                try
                {
                    Database::DatabaseHelper::GetInstance().GetConnection()->setAutoCommit(false);

                    Database::DatabaseHelper::GetInstance().ExecuteUpdate(
                        "INSERT INTO chats (name, photo, created_at) "
                        "SELECT NULL, NULL, NOW() "
                        "WHERE NOT EXISTS ("
                        "    SELECT 1 FROM chats WHERE id = %zu"
                        ");",
                        chatId
                    );

                    Database::DatabaseHelper::GetInstance().ExecuteUpdate(
                        "INSERT INTO users_chats (user_id, chat_id, joined_at) "
                        "SELECT %zu, "
                        "       (SELECT id FROM chats WHERE id = %zu LIMIT 1), "
                        "       NOW() "
                        "WHERE NOT EXISTS ("
                        "    SELECT 1 FROM users_chats WHERE user_id = %zu AND chat_id = %zu"
                        ");",
                        senderUserId, chatId,
                        senderUserId, chatId
                    );

                    Database::DatabaseHelper::GetInstance().ExecuteUpdate(
                        "INSERT INTO messages (chat_id, user_id, content, sent_at) "
                        "VALUES ("
                        "    (SELECT id FROM chats WHERE id = %zu LIMIT 1), "
                        "    %zu, '%s', NOW()"
                        ");",
                        chatId, senderUserId, message
                    );

                    Database::DatabaseHelper::GetInstance().GetConnection()->commit();

                    for (size_t i = 0; i < _connectionsCurrentCount; ++i)
                    {
                        send(_connections[i], reinterpret_cast<char*>(&actionType), sizeof(actionType), NULL);

                        //send(_connections[i], reinterpret_cast<char*>(&receiverUserId), sizeof(receiverUserId), NULL);

                        size_t messageSize = strlen(message);
                        send(_connections[i], reinterpret_cast<char*>(&messageSize), sizeof(messageSize), NULL);
                        send(_connections[i], message, messageSize, NULL);
                    }
                }
                catch (const sql::SQLException& e)
                {
                    SendServerErrorMessage(index, "Server error: server cant push message to database");

                    Database::DatabaseHelper::GetInstance().GetConnection()->rollback();

                    break;
                }

                break;
            }
            case NetworkCore::ActionType::kAddUserCredentialsToDatabase:
            {
                NetworkCore::UserPacket userPacket = ReceiveUserCredentialsPacket(index);

                if (userPacket.name.empty())
                {
                    SendServerErrorMessage(index, "Server error: username is empty");

                    break;
                }

                if (userPacket.login.empty())
                {
                    SendServerErrorMessage(index, "Server error: user login is empty");

                    break;
                }

                if (userPacket.password == 0)
                {
                    SendServerErrorMessage(index, "Server error: user password is empty");

                    break;
                }

                bool result = false;

                try
                {
                    result = Database::DatabaseHelper::GetInstance().ExecuteUpdate(
                        "INSERT INTO users(name, login, password) "
                        "VALUES('%s', '%s', %zu);",
                        userPacket.name.c_str(), userPacket.login.c_str(), userPacket.password);

                    send(_connections[index], reinterpret_cast<char*>(&actionType), sizeof(actionType), NULL);
                    send(_connections[index], reinterpret_cast<char*>(&result), sizeof(result), NULL);
                }
                catch (const sql::SQLException& e)
                {
                    send(_connections[index], reinterpret_cast<char*>(&actionType), sizeof(actionType), NULL);
                    send(_connections[index], reinterpret_cast<char*>(&result), sizeof(result), NULL);

                    SendServerErrorMessage(index, "Server error: server cant add user credentials to database");
                }

                break;
            }
            case NetworkCore::ActionType::kCheckUserExistence:
            {
                NetworkCore::UserPacket userPacket = ReceiveUserCredentialsPacket(index);

                if (userPacket.login.empty())
                {
                    SendServerErrorMessage(index, "Server error: user login is empty");

                    break;
                }

                if (userPacket.password == 0)
                {
                    SendServerErrorMessage(index, "Server error: user password is empty");

                    break;
                }

                try
                {
                    resultSet = Database::DatabaseHelper::GetInstance().ExecuteQuery(
                        "SELECT * FROM users "
                        "WHERE login = '%s' AND password = %zu;",
                        userPacket.login.c_str(), userPacket.password);

                    bool result = false;

                    if (resultSet->next())
                    {
                        result = true;
                    }

                    send(_connections[index], reinterpret_cast<char*>(&actionType), sizeof(actionType), NULL);
                    send(_connections[index], reinterpret_cast<char*>(&result), sizeof(result), NULL);
                }
                catch (const sql::SQLException& e)
                {
                    SendServerErrorMessage(index, "Server error: server cant check user existence");
                }

                break;
            }
            case NetworkCore::ActionType::kCheckIsUserDataFromFileValid:
            {
                NetworkCore::UserPacket userPacket = ReceiveUserCredentialsPacket(index);

                try
                {
                    resultSet = Database::DatabaseHelper::GetInstance().ExecuteQuery(
                        "SELECT * FROM users "
                        "WHERE name = '%s' AND login = '%s' "
                        "AND password = %zu AND id = %zu;",
                        userPacket.name.c_str(), userPacket.login.c_str(),
                        userPacket.password, userPacket.id);

                    bool result = false;

                    if (resultSet->next())
                    {
                        result = true;
                    }

                    send(_connections[index], reinterpret_cast<char*>(&actionType), sizeof(actionType), NULL);
                    send(_connections[index], reinterpret_cast<char*>(&result), sizeof(result), NULL);
                }
                catch (const sql::SQLException& e)
                {
                    SendServerErrorMessage(index, "Server error: server cant check is data from file valid");
                }

                break;
            }
            case NetworkCore::ActionType::kGetUserNameFromDatabase:
            {
                NetworkCore::UserPacket userPacket = ReceiveUserCredentialsPacket(index);

                if (userPacket.login.empty())
                {
                    SendServerErrorMessage(index, "Server error: user login is empty");

                    break;
                }

                try
                {
                    resultSet = Database::DatabaseHelper::GetInstance().ExecuteQuery(
                        "SELECT name FROM users "
                        "WHERE login = '%s';",
                        userPacket.login.c_str());

                    std::string result;
                    size_t responseSize = result.size();

                    if (resultSet->next())
                    {
                        result = resultSet->getString("name");
                    }

                    responseSize = result.size();

                    send(_connections[index], reinterpret_cast<char*>(&actionType), sizeof(actionType), NULL);

                    send(_connections[index], reinterpret_cast<char*>(&responseSize), sizeof(responseSize), NULL);
                    send(_connections[index], result.c_str(), responseSize, NULL);
                }
                catch (const sql::SQLException& e)
                {
                    SendServerErrorMessage(index, "Server error: server cant get user name from database");
                }

                break;
            }
            case NetworkCore::ActionType::kGetUserIdFromDatabase:
            {
                NetworkCore::UserPacket userPacket = ReceiveUserCredentialsPacket(index);

                if (userPacket.login.empty())
                {
                    SendServerErrorMessage(index, "Server error: user login is empty");

                    break;
                }

                try
                {
                    resultSet = Database::DatabaseHelper::GetInstance().ExecuteQuery(
                        "SELECT id FROM users "
                        "WHERE login = '%s';",
                        userPacket.login.c_str());

                    size_t result = 0;

                    if (resultSet->next())
                    {
                        result = resultSet->getInt("id");
                    }

                    send(_connections[index], reinterpret_cast<char*>(&actionType), sizeof(actionType), NULL);

                    send(_connections[index], reinterpret_cast<char*>(&result), sizeof(result), NULL);
                }
                catch (const sql::SQLException& e)
                {
                    SendServerErrorMessage(index, "Server error: server cant get user name from database");
                }

                break;
            }
            case NetworkCore::ActionType::kFindMatchingChats:
            {
                NetworkCore::UserPacket userPacket = ReceiveUserCredentialsPacket(index);

                size_t* chatIdResult = nullptr;
                std::string* chatNameResult = nullptr;
                std::string* chatPhotoResult = nullptr;
                std::string* lastMessageResult = nullptr;
                std::string* lastMessageTimeResult = nullptr;

                try
                {
                    resultSet = Database::DatabaseHelper::GetInstance().ExecuteQuery(
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
                        "u.id = %zu "
                        "AND (c.name LIKE '%s%%' OR u2.name LIKE '%s%%') "
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
                        "WHERE u.name LIKE '%s%%' "
                        "AND u.id != %zu "
                        "AND NOT EXISTS ( "
                        "SELECT 1 "
                        "FROM users_chats uc "
                        "JOIN chats c ON uc.chat_id = c.id "
                        "WHERE uc.user_id = %zu "
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

                    size_t counter = 0;
                    chatIdResult = new size_t[resultSet->rowsCount()];
                    chatNameResult = new std::string[resultSet->rowsCount()];
                    chatPhotoResult = new std::string[resultSet->rowsCount()];
                    lastMessageResult = new std::string[resultSet->rowsCount()];
                    lastMessageTimeResult = new std::string[resultSet->rowsCount()];

                    while (resultSet->next())
                    {
                        chatIdResult[counter] = resultSet->getInt("chat_id");
                        chatNameResult[counter] = resultSet->getString("chat_name");
                        chatPhotoResult[counter] = resultSet->getString("chat_photo");
                        lastMessageResult[counter] = resultSet->getString("last_message");
                        lastMessageTimeResult[counter] = resultSet->getString("last_message_time");

                        ++counter;
                    }

                    send(_connections[index], reinterpret_cast<char*>(&actionType), sizeof(actionType), NULL);
                    send(_connections[index], reinterpret_cast<char*>(&counter), sizeof(counter), NULL);

                    int i = 0;
                    while (counter > 0)
                    {
                        send(_connections[index], reinterpret_cast<char*>(&chatIdResult[i]), sizeof(chatIdResult[0]), NULL);

                        size_t resultLength = chatNameResult[i].size();
                        send(_connections[index], reinterpret_cast<char*>(&resultLength), sizeof(resultLength), NULL);
                        send(_connections[index], chatNameResult[i].c_str(), resultLength, NULL);

                        resultLength = lastMessageResult[i].size();
                        send(_connections[index], reinterpret_cast<char*>(&resultLength), sizeof(resultLength), NULL);
                        send(_connections[index], lastMessageResult[i].c_str(), resultLength, NULL);

                        resultLength = lastMessageTimeResult[i].size();
                        send(_connections[index], reinterpret_cast<char*>(&resultLength), sizeof(resultLength), NULL);
                        send(_connections[index], lastMessageTimeResult[i].c_str(), resultLength, NULL);

                        resultLength = chatPhotoResult[i].size();
                        send(_connections[index], reinterpret_cast<char*>(&resultLength), sizeof(resultLength), NULL);
                        send(_connections[index], chatPhotoResult[i].c_str(), resultLength, NULL);

                        counter--;
                        i++;
                    }
                }
                catch (const sql::SQLException& e)
                {
                    SendServerErrorMessage(index, "Server error: server cant push message to database");
                }

                delete[] chatNameResult;
                delete[] chatIdResult;
                delete[] chatPhotoResult;
                delete[] lastMessageResult;
                delete[] lastMessageTimeResult;

                break;
            }
            case NetworkCore::ActionType::kGetAvailableChatsForUser:
            {
                NetworkCore::UserPacket userPacket = ReceiveUserCredentialsPacket(index);

                if (userPacket.id == 0)
                {
                    SendServerErrorMessage(index, "Server error: user id is empty");

                    break;
                }

                size_t* chatIdResult = nullptr;
                std::string* chatNameResult = nullptr;
                std::string* chatPhotoResult = nullptr;
                std::string* lastMessageResult = nullptr;
                std::string* lastMessageTimeResult = nullptr;

                try
                {
                    resultSet = Database::DatabaseHelper::GetInstance().ExecuteQuery(
                        "SELECT DISTINCT "
                        "c.id AS chat_id, "
                        "COALESCE(c.name, u2.name) AS chat_name, "
                        "COALESCE(c.photo, u2.photo) AS chat_photo, "
                        "m.content AS last_message, "
                        "m.sent_at AS last_message_time "
                        "FROM chats c "
                        "JOIN users_chats uc ON c.id = uc.chat_id "
                        "JOIN users u ON uc.user_id = u.id "
                        "LEFT JOIN users_chats uc2 ON c.id = uc2.chat_id AND uc2.user_id != u.id "
                        "LEFT JOIN users u2 ON uc2.user_id = u2.id "
                        "LEFT JOIN messages m ON c.id = m.chat_id "
                        "WHERE u.id = %zu "
                        "AND m.sent_at = "
                        "(SELECT MAX(sent_at) "
                        "FROM messages "
                        "WHERE chat_id = c.id);",
                        userPacket.id);

                    size_t counter = 0;
                    chatIdResult = new size_t[resultSet->rowsCount()];
                    chatNameResult = new std::string[resultSet->rowsCount()];
                    chatPhotoResult = new std::string[resultSet->rowsCount()];
                    lastMessageResult = new std::string[resultSet->rowsCount()];
                    lastMessageTimeResult = new std::string[resultSet->rowsCount()];

                    while (resultSet->next())
                    {
                        chatIdResult[counter] = resultSet->getInt("chat_id");
                        chatNameResult[counter] = resultSet->getString("chat_name");
                        chatPhotoResult[counter] = resultSet->getString("chat_photo");
                        lastMessageResult[counter] = resultSet->getString("last_message");
                        lastMessageTimeResult[counter] = resultSet->getString("last_message_time");

                        counter++;
                    }

                    send(_connections[index], reinterpret_cast<char*>(&actionType), sizeof(actionType), NULL);
                    send(_connections[index], reinterpret_cast<char*>(&counter), sizeof(counter), NULL);

                    int i = 0;
                    while (counter > 0)
                    {
                        send(_connections[index], reinterpret_cast<char*>(&chatIdResult[i]), sizeof(chatIdResult[0]), NULL);

                        size_t resultLength = chatNameResult[i].size();
                        send(_connections[index], reinterpret_cast<char*>(&resultLength), sizeof(resultLength), NULL);
                        send(_connections[index], chatNameResult[i].c_str(), resultLength, NULL);

                        resultLength = lastMessageResult[i].size();
                        send(_connections[index], reinterpret_cast<char*>(&resultLength), sizeof(resultLength), NULL);
                        send(_connections[index], lastMessageResult[i].c_str(), resultLength, NULL);

                        resultLength = lastMessageTimeResult[i].size();
                        send(_connections[index], reinterpret_cast<char*>(&resultLength), sizeof(resultLength), NULL);
                        send(_connections[index], lastMessageTimeResult[i].c_str(), resultLength, NULL);

                        resultLength = chatPhotoResult[i].size();
                        send(_connections[index], reinterpret_cast<char*>(&resultLength), sizeof(resultLength), NULL);
                        send(_connections[index], chatPhotoResult[i].c_str(), resultLength, NULL);

                        counter--;
                        i++;
                    }
                }
                catch ([[maybe_unused]] const sql::SQLException& e)
                {
                    SendServerErrorMessage(index, "Server error: server cant get available chats for user");
                }

                delete[] chatNameResult;
                delete[] chatIdResult;
                delete[] chatPhotoResult;
                delete[] lastMessageResult;
                delete[] lastMessageTimeResult;

                break;
            }
            case NetworkCore::ActionType::kReceiveAllMessagesForSelectedChat:
            {
                size_t userId = 0;
                recv(_connections[index], reinterpret_cast<char*>(&userId), sizeof(userId), NULL);

                size_t id = 0;
                recv(_connections[index], reinterpret_cast<char*>(&id), sizeof(id), NULL);

                if (id == 0)
                {
                    SendServerErrorMessage(index, "Server error: chat id is empty");

                    break;
                }

                std::string* messagesResult = nullptr;
                std::string* messagesSendTime = nullptr;
                MessageBuffer::MessageStatus* messageTypeResult = nullptr;

                try
                {
                    resultSet = Database::DatabaseHelper::GetInstance().ExecuteQuery(
                        "SELECT * "
                        "FROM messages "
                        "WHERE chat_id = %zu;",
                        id);

                    messagesResult = new std::string[resultSet->rowsCount()];
                    messageTypeResult = new MessageBuffer::MessageStatus[resultSet->rowsCount()];
                    messagesSendTime = new std::string[resultSet->rowsCount()];
                    size_t count = 0;

                    while (resultSet->next())
                    {
                        size_t messageAuthorId = resultSet->getInt("user_id");

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

                        count++;
                    }

                    send(_connections[index], reinterpret_cast<char*>(&actionType), sizeof(actionType), NULL);
                    send(_connections[index], reinterpret_cast<char*>(&count), sizeof(count), NULL);

                    int i = 0;
                    while (count > 0)
                    {
                        size_t msgSize = messagesResult[i].size();
                        send(_connections[index], reinterpret_cast<char*>(&msgSize), sizeof(msgSize), NULL);
                        send(_connections[index], messagesResult[i].c_str(), msgSize, NULL);
                        send(_connections[index], reinterpret_cast<char*>(&messageTypeResult[i]), sizeof(messageTypeResult[i]), NULL);

                        size_t sendTimeSize = messagesSendTime[i].size();
                        send(_connections[index], reinterpret_cast<char*>(&sendTimeSize), sizeof(sendTimeSize), NULL);
                        send(_connections[index], messagesSendTime[i].c_str(), sendTimeSize, NULL);
                        i++;
                        count--;
                    }
                }
                catch (const sql::SQLException& e)
                {
                    SendServerErrorMessage(index, "Server error: server cant receive all messages for selected chat");
                }

                delete[] messagesResult;
                delete[] messageTypeResult;
                delete[] messagesSendTime;

                break;
            }
            default:
                break;
            }
        }
    }

    NetworkCore::UserPacket Server::ReceiveUserCredentialsPacket(size_t index) const noexcept
    {
        NetworkCore::UserPacket newUserPacket;

        size_t userNameLength;
        char userName[Common::userNameSize] = "";
        recv(_connections[index], reinterpret_cast<char*>(&userNameLength), sizeof(userNameLength), NULL);
        recv(_connections[index], userName, userNameLength, NULL);
        userName[userNameLength] = '\0';
        newUserPacket.name = userName;

        size_t userLoginLength;
        char userLogin[Common::userLoginSize] = "";
        recv(_connections[index], reinterpret_cast<char*>(&userLoginLength), sizeof(userLoginLength), NULL);
        recv(_connections[index], userLogin, userLoginLength, NULL);
        userLogin[userLoginLength] = '\0';
        newUserPacket.login = userLogin;

        size_t userPassword = 0;
        recv(_connections[index], reinterpret_cast<char*>(&userPassword), sizeof(userPassword), NULL);
        newUserPacket.password = userPassword;

        size_t userId = 0;
        recv(_connections[index], reinterpret_cast<char*>(&userId), sizeof(userId), NULL);
        newUserPacket.id = userId;

        return newUserPacket;
    }

    void Server::SendServerErrorMessage(const size_t index, const std::string& errorMessage) const noexcept
    {
        NetworkCore::ActionType actionType = NetworkCore::ActionType::kServerError;

        send(_connections[index], reinterpret_cast<char*>(&actionType), sizeof(actionType), NULL);

        size_t errorMessageSize = errorMessage.size();
        send(_connections[index], reinterpret_cast<char*>(&errorMessageSize), sizeof(errorMessageSize), NULL);
        send(_connections[index], errorMessage.c_str(), errorMessageSize, NULL);
    }
} // !namespace ServerNetworking