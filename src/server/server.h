#pragma once

#ifdef _WIN32
#include <WinSock2.h>
#endif   // !_WIN32

#include <array>
#include <map>
#include <string>

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
        static void HandleCreateNewPersonalChat(SOCKET clientSocket);
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
        static void SendServerErrorMessage(const SOCKET clientSocket,
                                           const std::string_view errorMessage) noexcept;

#ifdef _WIN32
        WSADATA _wsaData = {};
#endif   // !_WIN32

        SOCKET _serverSocket = 0;
        SOCKADDR_IN _socketAddress = {};

        static constexpr size_t _maxConnectionsCount = 100;
        std::array<SOCKET, _maxConnectionsCount> _connections = {};
        uint8_t _connectionsCurrentCount = 0;

        using UserId = size_t;
        std::map<UserId, SOCKET> _connectionsToUserId = {};
    };
}   // namespace ServerNetworking