#pragma once

#include <condition_variable>
#include <functional>
#include <mutex>
#include <optional>
#include <string>
#include <variant>
#include <vector>

#include "messageBuffer/messageBuffer.h"
#include "networkCore/networkCore.h"

#ifdef WIN32
#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable : 4996)
#endif // !WIN32

// forward declarations
namespace UserData
{
    class User;
}
namespace ChatSystem
{
    struct ChatInfo;
}

namespace ClientNetworking
{
    class Client final
    {
    public:
        using ServerResponse = std::variant<bool, size_t,
                                            std::string,
                                            std::vector<UserData::User>,
                                            std::vector<ChatSystem::ChatInfo>,
                                            std::vector<MessageBuffer::MessageNode>>;

    public:
        Client(const Client &) = delete;
        Client &operator=(const Client &) = delete;

        Client(Client &&) = default;
        Client &operator=(Client &&) = default;

        static std::optional<std::reference_wrapper<Client>> GetInstance() noexcept;

        void SendUserMessage(const size_t chatId, const size_t senderUserId, const std::string_view data) const noexcept;
        void CreateNewPersonalChat(size_t senderUserId, const std::string_view receiverUserName) const;
        void SendUserCredentialsPacket(const NetworkCore::UserPacket &userCredentials) const noexcept;
        void SendChatInfoPacket(const NetworkCore::ChatPacket &chatInfo) const noexcept;

        void ReceiveThread() const;

        void RegisterReceiveMessageCallback(std::function<void(const MessageBuffer::MessageNode &)> callback);

        template <typename T>
        T GetServerResponse() noexcept
        {
            std::unique_lock lock(_mutex);
            if (_conditionalVariable.wait_for(lock,
                                              std::chrono::seconds(5)) == std::cv_status::timeout)
            {
                _currentClientState = ClientState::kClientDisconnected;
                Disconnect();

                return {};
            }

            return std::get<T>(_serverResponse);
        }

        ~Client();

    private:
        Client() noexcept = default;

        bool Init() noexcept;
        bool Connect() noexcept;
        void Disconnect() noexcept;

    private:
        enum class ClientState : uint8_t
        {
            kClientDisconnected = 0,
            kCLientInited,
            kClientConnected,
        };

#ifdef WIN32
        WSADATA _wsaData;
#endif // !WIN32

        SOCKET _clientSocket;
        SOCKADDR_IN _socketAddress;
        ClientState _currentClientState = ClientState::kClientDisconnected;

        mutable std::mutex _mutex;
        mutable std::condition_variable _conditionalVariable;

        mutable ServerResponse _serverResponse;

        std::function<void(const MessageBuffer::MessageNode &)> _receiveMessageCallback = nullptr;
    };
} // !namespace ClientNetworking