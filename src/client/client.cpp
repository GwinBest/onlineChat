#include "client.h"

#ifndef NDEBUG
#include <iostream>
#endif // !NDEBUG

#include <thread>
#include <vector>

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
                if (currentAttempt >= attemptsToConnect)
                {
                    return std::nullopt;
                }

                ++currentAttempt;
            }

            std::thread receiveThread(&Client::ReceiveThread, GetInstance().value());
            receiveThread.detach();
        }

        return instance;
    }

    void Client::SendUserMessage(const UserData::User& sender, const std::string& receiverUserLogin, const char* data) const noexcept
    {
        NetworkCore::ActionType type = NetworkCore::ActionType::kSendChatMessage;
        send(_clientSocket, reinterpret_cast<char*>(&type), sizeof(type), NULL);

        size_t senderUserLoginSize = sender.GetUserLogin().size();
        send(_clientSocket, reinterpret_cast<char*>(&senderUserLoginSize), sizeof(senderUserLoginSize), NULL);
        send(_clientSocket, sender.GetUserLogin().c_str(), senderUserLoginSize, NULL);

        size_t senderUserId = sender.GetUserId();
        send(_clientSocket, reinterpret_cast<const char*>(&senderUserId), sizeof(senderUserId), NULL);

        size_t receiverUserLoginSize = receiverUserLogin.size();
        send(_clientSocket, reinterpret_cast<char*>(&receiverUserLoginSize), sizeof(receiverUserLoginSize), NULL);
        send(_clientSocket, receiverUserLogin.c_str(), receiverUserLoginSize, NULL);

        size_t dataSize = strlen(data);
        send(_clientSocket, reinterpret_cast<char*>(&dataSize), sizeof(dataSize), NULL);
        send(_clientSocket, data, dataSize, NULL);
    }

    void Client::SendUserCredentialsPacket(const NetworkCore::UserPacket& userCredentials) const noexcept
    {
        NetworkCore::ActionType type = userCredentials.actionType;
        send(_clientSocket, reinterpret_cast<char*>(&type), sizeof(type), NULL);

        size_t nameLength = userCredentials.name.size();
        send(_clientSocket, reinterpret_cast<char*>(&nameLength), sizeof(nameLength), NULL);
        send(_clientSocket, userCredentials.name.c_str(), nameLength, NULL);

        size_t loginLength = userCredentials.login.size();
        send(_clientSocket, reinterpret_cast<char*>(&loginLength), sizeof(loginLength), NULL);
        send(_clientSocket, userCredentials.login.c_str(), loginLength, NULL);

        send(_clientSocket, reinterpret_cast<const char*>(&userCredentials.password), sizeof(userCredentials.password), NULL);

        send(_clientSocket, reinterpret_cast<const char*>(&userCredentials.id), sizeof(userCredentials.id), NULL);

    }

    void Client::SendChatInfoPacket(const NetworkCore::ChatPacket& chatInfo) const noexcept
    {
        NetworkCore::ActionType type = chatInfo.actionType;
        send(_clientSocket, reinterpret_cast<char*>(&type), sizeof(type), NULL);

        send(_clientSocket, reinterpret_cast<const char*>(&chatInfo.chatUserId), sizeof(chatInfo.chatUserId), NULL);

        send(_clientSocket, reinterpret_cast<const char*>(&chatInfo.id), sizeof(chatInfo.id), NULL);
    }

    void Client::ReceiveThread() const noexcept
    {
        char serverResponse[NetworkCore::serverResponseSize];

        while (_currentClientState != ClientState::kClientDisconnected)
        {
            NetworkCore::ActionType type = NetworkCore::ActionType::kActionUndefined;
            recv(_clientSocket, reinterpret_cast<char*>(&type), sizeof(type), NULL);

            switch (type)
            {
            case NetworkCore::ActionType::kSendChatMessage:
            {
                size_t userId = 0;
                recv(_clientSocket, reinterpret_cast<char*>(&userId), sizeof(userId), NULL);

                size_t receiveMessageSize;
                char receiveMessage[Common::maxInputBufferSize];

                recv(_clientSocket, reinterpret_cast<char*>(&receiveMessageSize), sizeof(receiveMessageSize), NULL);
                recv(_clientSocket, receiveMessage, receiveMessageSize, NULL);
                receiveMessage[receiveMessageSize] = '\0';

                if (userId == currentUser.GetUserId())
                {
                    //MessageBuffer::messageBuffer.emplace_back(MessageBuffer::MessageNode(MessageBuffer::MessageStatus::kReceived, receiveMessage));
                }

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
                size_t responseSize;
                recv(_clientSocket, reinterpret_cast<char*>(&responseSize), sizeof(responseSize), NULL);
                recv(_clientSocket, serverResponse, responseSize, NULL);
                serverResponse[responseSize] = '\0';

                _serverResponse = serverResponse;
                _conditionalVariable.notify_one();

                break;
            }
            case NetworkCore::ActionType::kGetUserIdFromDatabase:
            {
                size_t response;
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

                std::vector<ChatSystem::ChatInfo> availableChatsVector;

                for (size_t i = 0; availableChatsCount > 0; ++i, --availableChatsCount)
                {
                    ChatSystem::ChatInfo foundChat;

                    size_t id;
                    recv(_clientSocket, reinterpret_cast<char*>(&id), sizeof(id), NULL);

                    char name[50];
                    size_t chatNameLength;
                    recv(_clientSocket, reinterpret_cast<char*>(&chatNameLength), sizeof(chatNameLength), NULL);
                    recv(_clientSocket, name, chatNameLength, NULL);
                    name[chatNameLength] = '\0';

                    size_t lastMessageSize;
                    char lastMessage[Common::maxInputBufferSize];
                    recv(_clientSocket, reinterpret_cast<char*>(&lastMessageSize), sizeof(lastMessageSize), NULL);
                    recv(_clientSocket, lastMessage, lastMessageSize, NULL);
                    lastMessage[lastMessageSize] = '\0';

                    size_t lastMessageSendTimeSize;
                    char lastMessageSendTime[Common::maxInputBufferSize];
                    recv(_clientSocket, reinterpret_cast<char*>(&lastMessageSendTimeSize), sizeof(lastMessageSendTimeSize), NULL);
                    recv(_clientSocket, lastMessageSendTime, lastMessageSendTimeSize, NULL);
                    lastMessageSendTime[lastMessageSendTimeSize] = '\0';

                    size_t photoSize;
                    char photo[Common::maxLastMessageSendTimeSize] = "";
                    recv(_clientSocket, reinterpret_cast<char*>(&photoSize), sizeof(photoSize), NULL);
                    if (photoSize != 0) recv(_clientSocket, lastMessageSendTime, photoSize, NULL);
                    photo[photoSize] = '\0';

                    foundChat.id = id;
                    foundChat.name = name;
                    foundChat.lastMessage = lastMessage;
                    foundChat.lastMessageSendTime = lastMessageSendTime;

                    availableChatsVector.push_back(foundChat);
                }

                _serverResponse = availableChatsVector;
                _conditionalVariable.notify_one();

                break;
            }
            case NetworkCore::ActionType::kReceiveAllMessagesForSelectedChat:
            {
                size_t messageCount;
                recv(_clientSocket, reinterpret_cast<char*>(&messageCount), sizeof(messageCount), NULL);

                std::vector<MessageBuffer::MessageNode> messageBuffer;

                _serverResponse = false;
                while (messageCount > 0)
                {
                    size_t receiveMessageSize;
                    char receiveMessage[Common::maxInputBufferSize];

                    recv(_clientSocket, reinterpret_cast<char*>(&receiveMessageSize), sizeof(receiveMessageSize), NULL);
                    recv(_clientSocket, receiveMessage, receiveMessageSize, NULL);
                    receiveMessage[receiveMessageSize] = '\0';

                    MessageBuffer::MessageStatus messageType;
                    recv(_clientSocket, reinterpret_cast<char*>(&messageType), sizeof(messageType), NULL);

                    size_t lastMessageSendTimeSize;
                    char lastMessageSendTime[Common::maxInputBufferSize];
                    recv(_clientSocket, reinterpret_cast<char*>(&lastMessageSendTimeSize), sizeof(lastMessageSendTimeSize), NULL);
                    recv(_clientSocket, lastMessageSendTime, lastMessageSendTimeSize, NULL);
                    lastMessageSendTime[lastMessageSendTimeSize] = '\0';

                    if (messageType == MessageBuffer::MessageStatus::kReceived)
                    {
                        messageBuffer.emplace_back(messageType, std::string(receiveMessage), std::string(lastMessageSendTime));
                    }
                    else if (messageType == MessageBuffer::MessageStatus::kSend)
                    {
                        messageBuffer.emplace_back(messageType, std::string(receiveMessage), std::string(lastMessageSendTime));
                    }

                    messageCount--;

                    if (!std::get<bool>(_serverResponse) && ((messageBuffer.size() + 1) % 15 == 0 || messageCount == 0))
                    {
                        _serverResponse = messageBuffer;
                        _conditionalVariable.notify_one();
                    }
                }

                break;
            }
            case NetworkCore::ActionType::kServerError:
            {
                size_t errorMessageSzie;
                char errorMessage[4096 + 1];

                recv(_clientSocket, reinterpret_cast<char*>(&errorMessageSzie), sizeof(errorMessageSzie), NULL);
                recv(_clientSocket, errorMessage, errorMessageSzie, NULL);
                errorMessage[errorMessageSzie] = '\0';

#ifndef NDEBUG
                std::cout << errorMessage << std::endl;
#endif // !NDEBUG

                break;
            }
            default:
                break;
            }
        }
    }

    Client::~Client()
    {
        if (_currentClientState != ClientState::kClientDisconnected)
            Client::Disconnect();
    }

    Client::Client() noexcept
    {
        if (WSAStartup(NetworkCore::dllVersion, &_wsaData))
        {
#ifndef NDEBUG
            std::cout << "WSAstratup error: " << WSAGetLastError() << std::endl;
#endif // !NDEBUG

            exit(SOCKET_ERROR);
        }

        _socketAddress.sin_family = AF_INET;
        _socketAddress.sin_addr.s_addr = inet_addr(NetworkCore::ipAddress.c_str());
        _socketAddress.sin_port = htons(NetworkCore::port);

        if ((_clientSocket = socket(AF_INET, SOCK_STREAM, NULL)) == SOCKET_ERROR)
        {
#ifndef NDEBUG
            std::cout << "socket error: " << WSAGetLastError() << std::endl;
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
            std::cout << "client already inited!" << std::endl;
#endif // !NDEBUG
            return false;
        }

        if (connect(_clientSocket, reinterpret_cast<SOCKADDR*>(&_socketAddress), sizeof(_socketAddress)) != 0)
        {
#ifndef NDEBUG
            std::cout << "connect error: " << WSAGetLastError() << std::endl;
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
                std::cout << "closesocket error: " << WSAGetLastError() << std::endl;
#endif // !NDEBUG

                exit(SOCKET_ERROR);
            }

            if (WSACleanup() == SOCKET_ERROR)
            {
#ifndef NDEBUG
                std::cout << "WSAcleanup error: " << WSAGetLastError() << std::endl;
#endif // !NDEBUG

                exit(SOCKET_ERROR);
            }

            _currentClientState = ClientState::kClientDisconnected;
        }
    }

} // !namespace ClientNetworking
