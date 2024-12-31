#pragma once

#include <WinSock2.h>

#include <array>
#include <map>
#include <string>

#include "database/database.h"
#include "networkCore/networkCore.h"

namespace ServerNetworking
{
    class Server final
    {
    public:
        Server();
        bool Start();
        [[noreturn]] void Run();
        void ClientHandler(const SOCKET clientSocket);

    private:
        void HandleAction(const SOCKET clientSocket, const NetworkCore::ActionType actionType);
        void HandleSendChatMessage(const SOCKET clientSocket);
        static void HandleAddUserCredentialsToDatabase(const SOCKET clientSocket);
        static void HandleCheckUserExistence(const SOCKET clientSocket);
        static void HandleCheckIsUserDataFromFileValid(const SOCKET clientSocket);
        static void HandleGetUserNameFromDatabase(const SOCKET clientSocket);
        void HandleGetUserIdFromDatabase(const SOCKET clientSocket);
        static void HandleFindMatchingChats(const SOCKET clientSocket);
        static void HandleGetAvailableChatsForUser(const SOCKET clientSocket);
        static void HandleReceiveAllMessagesForSelectedChat(const SOCKET clientSocket);

        static NetworkCore::UserPacket ReceiveUserCredentialsPacket(const SOCKET clientSocket);
        static void SendServerErrorMessage(const SOCKET clientSocket, const std::string& errorMessage) noexcept;

        WSADATA _wsaData = {};
        SOCKET _serverSocket = 0;
        SOCKADDR_IN _socketAddress = {};

        static constexpr size_t _maxConnectionsCount = 100;
        std::array<SOCKET, _maxConnectionsCount> _connections = {};
        uint8_t _connectionsCurrentCount = 0;

        using UserId = size_t;
        std::map<UserId, SOCKET> _connectionsToUserId = {};
    };
} // !namespace ServerNetworking
