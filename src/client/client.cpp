#include "client.h"

#ifdef _WIN32
#include <WinSock2.h>
#endif // !_WIN32

#include <array>
#include <bit>
#include <cstdint>
#include <thread>
#include <vector>

#ifndef NDEBUG
#include <iostream>
#endif // !NDEBUG

#include "common/common.h"
#include "userData/user.h"
#include "messageBuffer/messageBuffer.h"
#include "chatSystem/chatInfo.h"

extern UserData::User currentUser;

namespace ClientNetworking
{
    std::optional<std::reference_wrapper<Client>> Client::GetInstance() noexcept
    {
        static Client instance;
        if (instance._currentClientState != ClientState::kClientConnected)
        {
            constexpr uint8_t attemptsToConnect = 3;
            uint8_t currentAttempt = 1;

            while (!instance.Connect())
            {
                if (currentAttempt >= attemptsToConnect) return std::nullopt;

                ++currentAttempt;
            }

            std::thread(&Client::ReceiveThread, GetInstance().value()).detach();
        }

        return instance;
    }

    void Client::SendUserMessage(const size_t chatId, const size_t senderUserId, const std::string_view data) const noexcept
    {
        const auto type = NetworkCore::ActionType::kSendChatMessage;
        send(_clientSocket, reinterpret_cast<const char*>(&type), sizeof(type), NULL);

        send(_clientSocket, reinterpret_cast<const char*>(&chatId), sizeof(chatId), NULL);

        send(_clientSocket, reinterpret_cast<const char*>(&senderUserId), sizeof(senderUserId), NULL);

        const size_t dataSize = data.size();
        send(_clientSocket, reinterpret_cast<const char*>(&dataSize), sizeof(dataSize), NULL);
        send(_clientSocket, data.data(), dataSize, NULL);
    }

    void Client::CreateNewPersonalChat(const size_t senderUserId, const std::string_view receiverUserName) const
    {
        const auto type = NetworkCore::ActionType::kCreateNewPersonalChat;
        send(_clientSocket, reinterpret_cast<const char*>(&type), sizeof(type), NULL);

        send(_clientSocket, reinterpret_cast<const char*>(&senderUserId), sizeof(senderUserId), NULL);

        const size_t dataSize = receiverUserName.size();
        send(_clientSocket, reinterpret_cast<const char*>(&dataSize), sizeof(dataSize), NULL);
        send(_clientSocket, receiverUserName.data(), static_cast<int>(dataSize), NULL);
    }

    void Client::SendUserCredentialsPacket(const NetworkCore::UserPacket& userCredentials) const noexcept
    {
        const auto type = userCredentials.actionType;
        send(_clientSocket, reinterpret_cast<const char*>(&type), sizeof(type), NULL);

        const size_t nameLength = userCredentials.name.size();
        send(_clientSocket, reinterpret_cast<const char*>(&nameLength), sizeof(nameLength), NULL);
        send(_clientSocket, userCredentials.name.c_str(), nameLength, NULL);

        const size_t loginLength = userCredentials.login.size();
        send(_clientSocket, reinterpret_cast<const char*>(&loginLength), sizeof(loginLength), NULL);
        send(_clientSocket, userCredentials.login.c_str(), loginLength, NULL);

        send(_clientSocket, reinterpret_cast<const char*>(&userCredentials.password), sizeof(userCredentials.password), NULL);

        send(_clientSocket, reinterpret_cast<const char*>(&userCredentials.id), sizeof(userCredentials.id), NULL);

    }

    void Client::SendChatInfoPacket(const NetworkCore::ChatPacket& chatInfo) const noexcept
    {
        const auto type = chatInfo.actionType;
        send(_clientSocket, reinterpret_cast<const char*>(&type), sizeof(type), NULL);

        send(_clientSocket, reinterpret_cast<const char*>(&chatInfo.chatUserId), sizeof(chatInfo.chatUserId), NULL);

        send(_clientSocket, reinterpret_cast<const char*>(&chatInfo.id), sizeof(chatInfo.id), NULL);
    }

    void Client::ReceiveThread() const
    {
        std::array<char, NetworkCore::serverResponseSize> serverResponse = {};

        while (_currentClientState != ClientState::kClientDisconnected)
        {
            auto type = NetworkCore::ActionType::kActionUndefined;
            recv(_clientSocket, reinterpret_cast<char*>(&type), sizeof(type), NULL);

            switch (type)
            {
            case NetworkCore::ActionType::kCreateNewPersonalChat:
            {
                size_t result = 0;
                recv(_clientSocket, reinterpret_cast<char*>(&result), sizeof(result), NULL);

                _serverResponse = result;

                _conditionalVariable.notify_one();
                break;
            }
            case NetworkCore::ActionType::kSendChatMessage:
            {
                size_t receiveMessageSize = 0;
                std::array<char, Common::maxInputBufferSize> receiveMessage = {};
                recv(_clientSocket, reinterpret_cast<char*>(&receiveMessageSize), sizeof(receiveMessageSize), NULL);
                recv(_clientSocket, receiveMessage.data(), static_cast<int>(receiveMessageSize), NULL);
                receiveMessage[receiveMessageSize] = '\0';

                size_t sendTimeSize = 0;
                std::array<char, Common::maxLastMessageSendTimeSize> sentAt = {};
                recv(_clientSocket, reinterpret_cast<char*>(&sendTimeSize), sizeof(sendTimeSize), NULL);
                recv(_clientSocket, sentAt.data(), static_cast<int>(sendTimeSize), NULL);
                sentAt[sendTimeSize] = '\0';

                const MessageBuffer::MessageNode messageNode(
                    MessageBuffer::MessageStatus::kReceived,
                    std::string(receiveMessage.data()),
                    sentAt.data());

                if (_receiveMessageCallback) _receiveMessageCallback(messageNode);
                break;
            }
            case NetworkCore::ActionType::kAddUserCredentialsToDatabase:
            case NetworkCore::ActionType::kCheckIsUserDataFromFileValid:
            case NetworkCore::ActionType::kCheckUserExistence:
            {
                bool response = false;
                recv(_clientSocket, reinterpret_cast<char*>(&response), sizeof(response), NULL);

                _serverResponse = response;

                _conditionalVariable.notify_one();
                break;
            }
            case NetworkCore::ActionType::kGetUserNameFromDatabase:
            {
                size_t responseSize = 0;
                recv(_clientSocket, reinterpret_cast<char*>(&responseSize), sizeof(responseSize), NULL);
                recv(_clientSocket, serverResponse.data(), responseSize, NULL);
                serverResponse[responseSize] = '\0';

                _serverResponse = serverResponse.data();

                _conditionalVariable.notify_one();
                break;
            }
            case NetworkCore::ActionType::kGetUserIdFromDatabase:
            {
                size_t response = 0;
                recv(_clientSocket, reinterpret_cast<char*>(&response), sizeof(response), NULL);

                _serverResponse = response;

                _conditionalVariable.notify_one();
                break;
            }
            case NetworkCore::ActionType::kFindMatchingChats:
            case NetworkCore::ActionType::kGetAvailableChatsForUser:
            {
                size_t availableChatsCount = 0;
                recv(_clientSocket, reinterpret_cast<char*>(&availableChatsCount), sizeof(availableChatsCount), NULL);

                std::vector<ChatSystem::ChatInfo> availableChatsVector = {};
                availableChatsVector.reserve(availableChatsCount);

                while (availableChatsCount > 0)
                {
                    size_t id = 0;
                    recv(_clientSocket, reinterpret_cast<char*>(&id), sizeof(id), NULL);

                    std::array<char, Common::userNameSize> name = {};
                    size_t chatNameLength = 0;
                    recv(_clientSocket, reinterpret_cast<char*>(&chatNameLength), sizeof(chatNameLength), NULL);
                    recv(_clientSocket, name.data(), chatNameLength, NULL);
                    name[chatNameLength] = '\0';

                    std::array<char, Common::maxInputBufferSize> lastMessage = {};
                    size_t lastMessageSize = 0;
                    recv(_clientSocket, reinterpret_cast<char*>(&lastMessageSize), sizeof(lastMessageSize), NULL);
                    recv(_clientSocket, lastMessage.data(), lastMessageSize, NULL);
                    lastMessage[lastMessageSize] = '\0';

                    std::array<char, Common::maxInputBufferSize> lastMessageSendTime = {};
                    size_t lastMessageSendTimeSize = 0;
                    recv(_clientSocket, reinterpret_cast<char*>(&lastMessageSendTimeSize), sizeof(lastMessageSendTimeSize), NULL);
                    recv(_clientSocket, lastMessageSendTime.data(), lastMessageSendTimeSize, NULL);
                    lastMessageSendTime[lastMessageSendTimeSize] = '\0';

                    std::array<char, Common::maxLastMessageSendTimeSize> photo = {};
                    size_t photoSize = 0;
                    recv(_clientSocket, reinterpret_cast<char*>(&photoSize), sizeof(photoSize), NULL);
                    if (photoSize != 0) recv(_clientSocket, photo.data(), photoSize, NULL);
                    photo[photoSize] = '\0';

                    availableChatsVector.emplace_back(id, name.data(),
                                                      lastMessage.data(), lastMessageSendTime.data());

                    --availableChatsCount;
                }

                _serverResponse = availableChatsVector;

                _conditionalVariable.notify_one();
                break;
            }
            case NetworkCore::ActionType::kReceiveAllMessagesForSelectedChat:
            {
                size_t messageCount = 0;
                recv(_clientSocket, reinterpret_cast<char*>(&messageCount), sizeof(messageCount), NULL);

                std::vector<MessageBuffer::MessageNode> messageBuffer;
                messageBuffer.reserve(messageCount);

                _serverResponse = false;
                while (messageCount > 0)
                {
                    size_t receiveMessageSize = 0;
                    std::array<char, Common::maxInputBufferSize> receiveMessage = {};
                    recv(_clientSocket, reinterpret_cast<char*>(&receiveMessageSize), sizeof(receiveMessageSize), NULL);
                    recv(_clientSocket, receiveMessage.data(), receiveMessageSize, NULL);
                    receiveMessage[receiveMessageSize] = '\0';

                    auto messageType = MessageBuffer::MessageStatus::kUndefined;
                    recv(_clientSocket, reinterpret_cast<char*>(&messageType), sizeof(messageType), NULL);

                    size_t lastMessageSendTimeSize = 0;
                    std::array<char, Common::maxLastMessageSendTimeSize> lastMessageSendTime = {};
                    recv(_clientSocket, reinterpret_cast<char*>(&lastMessageSendTimeSize), sizeof(lastMessageSendTimeSize), NULL);
                    recv(_clientSocket, lastMessageSendTime.data(), lastMessageSendTimeSize, NULL);
                    lastMessageSendTime[lastMessageSendTimeSize] = '\0';

                    if (messageType == MessageBuffer::MessageStatus::kReceived)
                    {
                        messageBuffer.emplace_back(
                            messageType,
                            receiveMessage.data(),
                            lastMessageSendTime.data());
                    }
                    else if (messageType == MessageBuffer::MessageStatus::kSend)
                    {
                        messageBuffer.emplace_back(
                            messageType,
                            receiveMessage.data(),
                            lastMessageSendTime.data());
                    }

                    --messageCount;
                }

                _serverResponse = messageBuffer;

                _conditionalVariable.notify_one();
                break;
            }
            case NetworkCore::ActionType::kServerError:
            {
                size_t errorMessageSize = 0;
                recv(_clientSocket, reinterpret_cast<char*>(&errorMessageSize), sizeof(errorMessageSize), NULL);
                recv(_clientSocket, serverResponse.data(), errorMessageSize, NULL);
                serverResponse[errorMessageSize] = '\0';

#ifndef NDEBUG
                std::cout << serverResponse.data() << '\n';
#endif // !NDEBUG

                break;
            }
            default:
                break;
            }
        }
    }

    void Client::RegisterReceiveMessageCallback(std::function<void(const MessageBuffer::MessageNode&)> callback)
    {
        _receiveMessageCallback = std::move(callback);
    }

    Client::~Client()
    {
        if (_currentClientState != ClientState::kClientDisconnected)
        {
            Client::Disconnect();
        }
    }

    Client::Client() noexcept
    {
#ifdef _WIN32
        if (WSAStartup(NetworkCore::dllVersion, &_wsaData))
        {
#ifndef NDEBUG
            std::cout << "WSA startup error: " << WSAGetLastError() << '\n';
#endif // !NDEBUG

            exit(SOCKET_ERROR);
        }
#endif // !_WIN32

        _socketAddress.sin_family = AF_INET;
        _socketAddress.sin_addr.s_addr = inet_addr(NetworkCore::ipAddress.data());
        _socketAddress.sin_port = htons(NetworkCore::port);

        if ((_clientSocket = socket(AF_INET, SOCK_STREAM, NULL)) == SOCKET_ERROR)
        {
#ifndef NDEBUG
            std::cout << "socket error: " << WSAGetLastError() << '\n';
#endif // !NDEBUG

            exit(SOCKET_ERROR);
        }

        _currentClientState = ClientState::kCLientInited;
    }

    bool Client::Connect() noexcept
    {
        if (_currentClientState != ClientState::kCLientInited)
        {
#ifndef NDEBUG
            std::cout << "client already inited!" << '\n';
#endif // !NDEBUG
            return false;
        }

        if (connect(_clientSocket,
                    std::bit_cast<SOCKADDR*>(&_socketAddress),
                    sizeof(_socketAddress)) != 0)
        {
#ifndef NDEBUG
            std::cout << "connect error: " << WSAGetLastError() << '\n';
#endif // !NDEBUG
            return false;
        }

        _currentClientState = ClientState::kClientConnected;

        return true;
    }

    void Client::Disconnect() noexcept
    {
        if (_currentClientState != ClientState::kClientDisconnected)
        {
            if (closesocket(_clientSocket) == SOCKET_ERROR)
            {
#ifndef NDEBUG
                std::cout << "closesocket error: " << WSAGetLastError() << '\n';
#endif // !NDEBUG

                exit(SOCKET_ERROR);
            }

#ifdef _WIN32
            if (WSACleanup() == SOCKET_ERROR)
            {
#ifndef NDEBUG
                std::cout << "WSA cleanup error: " << WSAGetLastError() << '\n';
#endif // !NDEBUG

                exit(SOCKET_ERROR);
            }
#endif // !_WIN32

            _currentClientState = ClientState::kClientDisconnected;
        }
    }
} // !namespace ClientNetworking
