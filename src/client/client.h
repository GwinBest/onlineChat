#pragma once

#include <condition_variable>
#include <mutex>
#include <optional>
#include <variant>

#include "messageBuffer/messageBuffer.h"
#include "networkCore/networkCore.h"

#ifdef WIN32
#pragma comment (lib, "ws2_32.lib")
#pragma warning (disable:4996)
#endif

// forward declarations
namespace UserData { class User; }
namespace ChatSystem { struct ChatInfo; }

namespace ClientNetworking
{
    class Client final
    {
    public:
        using ServerResponse = std::variant<bool, size_t,
            std::string, std::vector<UserData::User>,
            std::vector<ChatSystem::ChatInfo>,
            std::vector<MessageBuffer::MessageNode>>;

    public:
        Client(const Client&) = delete;
        void operator= (const Client&) = delete;

        static std::optional<std::reference_wrapper<Client>> GetInstance() noexcept;

        void SendUserMessage(const size_t chatId, const size_t senderUserId, const char* const data) const noexcept;
        void SendUserCredentialsPacket(const NetworkCore::UserPacket& userCredentials) const noexcept;
        void SendChatInfoPacket(const NetworkCore::ChatPacket& chatInfo) const noexcept;

        void ReceiveThread() const noexcept;

        template<typename T>
        const T& GetServerResponse() const noexcept
        {
            std::unique_lock<std::mutex> lock(_mutex);
            _conditionalVariable.wait(lock);

            return std::get<T>(_serverResponse);
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

        mutable ServerResponse _serverResponse;
    };
} // !namespace ClientNetworking
