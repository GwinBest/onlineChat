#include "client.h"

#ifndef NDEBUG
#include <iostream>
#endif // !NDEBUG

#include <thread>
#include <vector>

#include "../common/common.h"
#include "../userData/user.h"
#include "../messageBuffer/messageBuffer.h"

extern std::list<MessageBuffer::MessageNode> MessageBuffer::messageBuffer;

extern UserData::User currentUser;

namespace ClientNetworking
{
    Client& Client::GetInstance() noexcept
    {
        static Client instance;
        if (instance._currentClientState != ClientState::kClientConnected)
        {
            while (!instance.Connect())
            {
                //TODO: add !connect handle
            };

            std::thread receiveThread(&Client::ReceiveThread, &Client::GetInstance());
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

        send(_clientSocket, reinterpret_cast<const char*>(&chatInfo.chatId), sizeof(chatInfo.chatId), NULL);
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
                size_t userId;

                recv(_clientSocket, reinterpret_cast<char*>(&userId), sizeof(userId), NULL);

                size_t receiveMessageSize;
                char receiveMessage[Common::maxInputBufferSize];

                recv(_clientSocket, reinterpret_cast<char*>(&receiveMessageSize), sizeof(receiveMessageSize), NULL);
                recv(_clientSocket, receiveMessage, receiveMessageSize, NULL);
                receiveMessage[receiveMessageSize] = '\0';

                if (userId == currentUser.GetUserId())
                {
                    MessageBuffer::messageBuffer.emplace_back(MessageBuffer::MessageNode(MessageBuffer::MessageStatus::kReceived, receiveMessage));
                }

                break;
            }
            case NetworkCore::ActionType::kAddUserCredentialsToDatabase:
            case NetworkCore::ActionType::kCheckIsUserDataFromFileValid:
            case NetworkCore::ActionType::kCheckUserExistence:
            {
                bool response = false;
                recv(_clientSocket, reinterpret_cast<char*>(&response), sizeof(response), NULL);

                NetworkCore::serverResponse = response;

                _conditionalVariable.notify_one();

                break;
            }
            case NetworkCore::ActionType::kGetUserNameFromDatabase:
            {
                size_t responseSize;
                recv(_clientSocket, reinterpret_cast<char*>(&responseSize), sizeof(responseSize), NULL);
                recv(_clientSocket, serverResponse, responseSize, NULL);
                serverResponse[responseSize] = '\0';

                NetworkCore::serverResponse = serverResponse;
                _conditionalVariable.notify_one();

                break;
            }
            case NetworkCore::ActionType::kGetUserIdFromDatabase:
            {
                size_t response;
                recv(_clientSocket, reinterpret_cast<char*>(&response), sizeof(response), NULL);

                NetworkCore::serverResponse = response;
                _conditionalVariable.notify_one();

                break;
            }
            case NetworkCore::ActionType::kFindUsersByLogin:
            {
                size_t foundUsersCount = 0;
                recv(_clientSocket, reinterpret_cast<char*>(&foundUsersCount), sizeof(foundUsersCount), NULL);

                std::vector<UserData::User> foundUsersVector;

                for (size_t i = 0; foundUsersCount > 0; ++i, --foundUsersCount)
                {
                    char userLogin[Common::userLoginSize];
                    size_t userLoginLength;
                    UserData::User foundUser;

                    recv(_clientSocket, reinterpret_cast<char*>(&userLoginLength), sizeof(userLoginLength), NULL);
                    recv(_clientSocket, userLogin, userLoginLength, NULL);
                    userLogin[userLoginLength] = '\0';

                    foundUser.SetUserLogin(userLogin);
                    foundUsersVector.push_back(foundUser);
                }

                NetworkCore::serverResponse = foundUsersVector;
                _conditionalVariable.notify_one();

                break;
            }
            case NetworkCore::ActionType::kGetAvailableChatsForUser:
            {
                size_t availableChatsCount = 0;
                recv(_clientSocket, reinterpret_cast<char*>(&availableChatsCount), sizeof(availableChatsCount), NULL);

                std::vector<ChatSystem::Chat> availableChatsVector;

                for (size_t i = 0; availableChatsCount > 0; ++i, --availableChatsCount)
                {
                    char chatName[50];
                    size_t chatNameLength;
                    ChatSystem::Chat foundChat;

                    size_t chatId;
                    recv(_clientSocket, reinterpret_cast<char*>(&chatId), sizeof(chatId), NULL);

                    recv(_clientSocket, reinterpret_cast<char*>(&chatNameLength), sizeof(chatNameLength), NULL);
                    recv(_clientSocket, chatName, chatNameLength, NULL);
                    chatName[chatNameLength] = '\0';

                    foundChat.SetChatName(chatName);
                    foundChat.SetChatId(chatId);

                    availableChatsVector.push_back(foundChat);
                }

                NetworkCore::serverResponse = availableChatsVector;
                _conditionalVariable.notify_one();

                break;
            }
            case NetworkCore::ActionType::kReceiveAllMessagesForSelectedChat:
            {
                size_t messageCount;
                recv(_clientSocket, reinterpret_cast<char*>(&messageCount), sizeof(messageCount), NULL);

                MessageBuffer::messageBuffer.clear();

                NetworkCore::serverResponse = false;
                while (messageCount > 0)
                {
                    size_t receiveMessageSize;
                    char receiveMessage[Common::maxInputBufferSize];

                    recv(_clientSocket, reinterpret_cast<char*>(&receiveMessageSize), sizeof(receiveMessageSize), NULL);
                    recv(_clientSocket, receiveMessage, receiveMessageSize, NULL);
                    receiveMessage[receiveMessageSize] = '\0';

                    MessageBuffer::MessageStatus messageType;
                    recv(_clientSocket, reinterpret_cast<char*>(&messageType), sizeof(messageType), NULL);

                    if (messageType == MessageBuffer::MessageStatus::kReceived)
                    {
                        MessageBuffer::messageBuffer.emplace_back(MessageBuffer::MessageNode(messageType, std::string(receiveMessage)));
                    }
                    else if (messageType == MessageBuffer::MessageStatus::kSend)
                    {
                        MessageBuffer::messageBuffer.emplace_back(MessageBuffer::MessageNode(messageType, std::string(receiveMessage)));
                    }

                    messageCount--;

                    if (!std::get<bool>(NetworkCore::serverResponse) && ((MessageBuffer::messageBuffer.size() + 1) % 15 == 0 || messageCount == 0))
                    {
                        NetworkCore::serverResponse = true;
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
