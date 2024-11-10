#pragma once

#include <condition_variable>
#include <mutex>
#include <optional>

#include "networkCore/networkCore.h"

#ifdef WIN32
#pragma comment (lib, "ws2_32.lib")
#pragma warning (disable:4996)
#endif

namespace ClientNetworking
{
    class Client final
    {
    public:
        Client(const Client&) = delete;
        void operator= (const Client&) = delete;

        static std::optional<Client*> GetInstance() noexcept;

        //The sender objects must have login and id fields filled in
        void SendUserMessage(const UserData::User& sender, const std::string& receiverUserLogin, const char* data) const noexcept;
        void SendUserCredentialsPacket(const NetworkCore::UserPacket& userCredentials) const noexcept;
        void SendChatInfoPacket(const NetworkCore::ChatPacket& chatInfo) const noexcept;

        void ReceiveThread() const noexcept;

        template<typename T>
        const T& GetServerResponse() const noexcept
        {
            std::unique_lock<std::mutex> lock(_mutex);
            _conditionalVariable.wait(lock);

            return std::get<T>(NetworkCore::serverResponse);
        }

        ~Client();

    private:
        Client() noexcept;

        bool Connect() noexcept;
        void Disconnect() noexcept;

    private:
        enum class ClientState : uint8_t
        {
            kClientDisconnected = 0,
            kCLientInited,
            kClientConnected,
        };

        WSADATA _wsaData;
        SOCKET _clientSocket;
        SOCKADDR_IN _socketAddress;
        ClientState _currentClientState = ClientState::kClientDisconnected;

        mutable std::mutex _mutex;
        mutable std::condition_variable _conditionalVariable;
    };

} // !namespace ClientNetworking
