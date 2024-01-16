﻿#include "client.h"

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

	void Client::SendUserMessage(const std::string& currentUserLogin, const std::string& selectedUserLogin, const std::string data) const noexcept
	{
		NetworkCore::ActionType type = NetworkCore::ActionType::kSendChatMessage;
		send(_clientSocket, reinterpret_cast<char*>(&type), sizeof(type), NULL);

		size_t currentUserLoginSize = currentUserLogin.size();
		send(_clientSocket, reinterpret_cast<char*>(&currentUserLoginSize), sizeof(currentUserLoginSize), NULL);
		send(_clientSocket, currentUserLogin.c_str(), currentUserLoginSize, NULL);

		size_t selectedUserLoginSize = selectedUserLogin.size();
		send(_clientSocket, reinterpret_cast<char*>(&selectedUserLoginSize), sizeof(selectedUserLoginSize), NULL);
		send(_clientSocket, selectedUserLogin.c_str(), selectedUserLoginSize, NULL);

		size_t dataSize = data.size();
		send(_clientSocket, reinterpret_cast<char*>(&dataSize), sizeof(dataSize), NULL);
		send(_clientSocket, data.c_str(), dataSize, NULL);
	}

	void Client::SendUserCredentialsPacket(const UserPacket& userCredentials) const noexcept
	{
		NetworkCore::ActionType type = userCredentials.actionType;
		send(_clientSocket, reinterpret_cast<char*>(&type), sizeof(type), NULL);

		size_t nameLength = userCredentials.name.size();
		send(_clientSocket, reinterpret_cast<char*>(&nameLength), sizeof(nameLength), NULL);
		send(_clientSocket, userCredentials.name.c_str(), nameLength, NULL);

		size_t loginLength = userCredentials.login.size();
		send(_clientSocket, reinterpret_cast<char*>(&loginLength), sizeof(loginLength), NULL);
		send(_clientSocket, userCredentials.login.c_str(), loginLength, NULL);

		send(_clientSocket, reinterpret_cast<char*>(&const_cast<size_t&>(userCredentials.password)), sizeof(userCredentials.password), NULL);
	}

	void Client::SendChatInfoPacket(const ChatPacket& chatInfo) const noexcept
	{
		NetworkCore::ActionType type = chatInfo.actionType;
		send(_clientSocket, reinterpret_cast<char*>(&type), sizeof(type), NULL);

		size_t chatUserLoginLength = chatInfo.chatUserLogin.size();
		send(_clientSocket, reinterpret_cast<char*>(&chatUserLoginLength), sizeof(chatUserLoginLength), NULL);
		send(_clientSocket, chatInfo.chatUserLogin.c_str(), chatUserLoginLength, NULL);

		send(_clientSocket, reinterpret_cast<char*>(&const_cast<size_t&>(chatInfo.chatId)), sizeof(chatInfo.chatId), NULL);
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
				constexpr const size_t receiveMessageSize = 4096;
				char receiveMessage[receiveMessageSize + 1];
				char userLogin[50];

				recv(_clientSocket, userLogin, 50, NULL);
				if (userLogin == currentUser.GetUserLogin())
				{
					recv(_clientSocket, receiveMessage, receiveMessageSize, NULL);
				
					//TODO: add callback	
					MessageBuffer::messageBuffer.push_back(MessageBuffer::MessageNode(MessageBuffer::MessageStatus::kReceived, std::string(receiveMessage)));
				}

				break;
			}
			case NetworkCore::ActionType::kAddUserCredentialsToDatabase:
			{
				break;
			}
			case NetworkCore::ActionType::kCheckUserExistence:
			{
				recv(_clientSocket, reinterpret_cast<char*>(&NetworkCore::serverResponse), sizeof(bool), NULL);

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
			case NetworkCore::ActionType::kFindUsersByLogin:
			{
				size_t foundUsersCount = 0;
				recv(_clientSocket, reinterpret_cast<char*>(&foundUsersCount), sizeof(foundUsersCount), NULL);

				std::vector<UserData::User> foundUsersVector;

				for (size_t i = 0; foundUsersCount > 0; ++i, --foundUsersCount)
				{
					char userLogin[50];
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
			case NetworkCore::ActionType::kGetAvailableChats:
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

				while (messageCount > 0)
				{
					 size_t receiveMessageSize ;
					char receiveMessage[4096 + 1];

					recv(_clientSocket, reinterpret_cast<char*>(&receiveMessageSize), sizeof(receiveMessageSize), NULL);
					recv(_clientSocket, receiveMessage, receiveMessageSize, NULL);
					receiveMessage[receiveMessageSize] = '\0';
					{
						MessageBuffer::MessageStatus messageType;
						recv(_clientSocket, reinterpret_cast<char*>(&messageType), sizeof(messageType), NULL);

						if (messageType == MessageBuffer::MessageStatus::kReceived)
						{
							MessageBuffer::messageBuffer.push_back(MessageBuffer::MessageNode(messageType, std::string(receiveMessage)));
						}
						else if(messageType == MessageBuffer::MessageStatus::kSend)
						{
							MessageBuffer::messageBuffer.push_back(MessageBuffer::MessageNode(messageType, std::string(receiveMessage)));
						}
					}

					messageCount--;

					if ((MessageBuffer::messageBuffer.size() + 1) % 15 == 0 || messageCount == 0)
					{
						NetworkCore::serverResponse = true; //TODO
						_conditionalVariable.notify_one();
					}

				}
				
				break;
			}
			default:
			{
				break;
			}
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
#endif

			exit(SOCKET_ERROR);
		}

		_socketAddress.sin_family = AF_INET;
		_socketAddress.sin_addr.s_addr = inet_addr(NetworkCore::ipAddress.c_str());
		_socketAddress.sin_port = htons(NetworkCore::port);

		if ((_clientSocket = socket(AF_INET, SOCK_STREAM, NULL)) == SOCKET_ERROR)
		{
#ifndef NDEBUG
			std::cout << "socket error: " << WSAGetLastError() << std::endl;
#endif

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
#endif
			return false;
		}

		if (connect(_clientSocket, reinterpret_cast<SOCKADDR*>(&_socketAddress), sizeof(_socketAddress)) != 0)
		{
#ifndef NDEBUG
			std::cout << "connect error: " << WSAGetLastError() << std::endl;
#endif
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
#endif

				exit(SOCKET_ERROR);
			}

			if (WSACleanup() == SOCKET_ERROR)
			{
#ifndef NDEBUG
				std::cout << "WSAcleanup error: " << WSAGetLastError() << std::endl;
#endif

				exit(SOCKET_ERROR);
			}

			_currentClientState = ClientState::kClientDisconnected;
		}
	}

} // !namespace ClientNetworking
