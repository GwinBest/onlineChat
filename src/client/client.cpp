#include "client.h"

#include "../userData/userData.h"

extern UserData::User currentUser;

namespace Network
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

			static std::thread receiveThread(&Network::Client::ReceiveThread, &Network::Client::GetInstance());
			receiveThread.detach();
		}

		return instance;
	}

	void Client::SendUserMessage(const std::string& currentUserLogin, const std::string& selectedUserLogin, const std::string data) const noexcept
	{
		//TODO: do not send name and login length
		ActionType type = ActionType::kSendChatMessage;
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
		//TODO: do not send name and login length

		ActionType type = userCredentials.actionType;
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
		//TODO: add send functions

		ActionType type = chatInfo.actionType;
		send(_clientSocket, reinterpret_cast<char*>(&type), sizeof(type), NULL);

		size_t currentUserLoginLength = chatInfo.currentUserLogin.size();
		send(_clientSocket, reinterpret_cast<char*>(&currentUserLoginLength), sizeof(currentUserLoginLength), NULL);
		send(_clientSocket, chatInfo.currentUserLogin.c_str(), currentUserLoginLength, NULL);
	}

	void Client::ReceiveThread() const noexcept
	{
		while (true)
		{
			ActionType type = ActionType::kActionUndefined;
			recv(_clientSocket, reinterpret_cast<char*>(&type), sizeof(type), NULL);

			switch (type)
			{
			case ActionType::kSendChatMessage:
			{
				constexpr const size_t receiveMessageSize = 4096;
				char receiveMessage[receiveMessageSize + 1];
				char userLogin[50];

				recv(_clientSocket, userLogin, 50, NULL);
				if (userLogin == currentUser.GetUserLogin())
				{
					recv(_clientSocket, receiveMessage, receiveMessageSize, NULL);
				
					Buffer::MessageBuffer::getInstance().pushFront(Buffer::MessageType::kReceived, receiveMessage);
				}

				break;
			}
			case ActionType::kAddUserCredentialsToDatabase:
			{
				break;
			}
			case ActionType::kCheckUserExistence:
			case ActionType::kGetUserNameFromDatabase:
			{
				//TODO: do not receive size
				size_t responseSize;
				char response[255];

				recv(_clientSocket, reinterpret_cast<char*>(&responseSize), sizeof(responseSize), NULL);
				recv(_clientSocket, response, responseSize, NULL);
				response[responseSize] = '\0';

				_serverResponse = response;
				_conditionalVariable.notify_one();

				break;
			}
			case ActionType::kFindUsersByLogin:
			{
				size_t foundUsersCount = 0;
				recv(_clientSocket, reinterpret_cast<char*>(&foundUsersCount), sizeof(foundUsersCount), NULL);

				std::vector<std::shared_ptr<UserData::User>> foundUsersVector;

				for (size_t i = 0; foundUsersCount > 0; ++i, --foundUsersCount)
				{
					// TODO: do not receive user login length  
					char userLogin[50];
					size_t userLoginLength;
					std::shared_ptr<UserData::User> foundUser = std::make_shared<UserData::User>();

					recv(_clientSocket, reinterpret_cast<char*>(&userLoginLength), sizeof(userLoginLength), NULL);
					recv(_clientSocket, userLogin, userLoginLength, NULL);
					userLogin[userLoginLength] = '\0';

					foundUser->SetUserLogin(userLogin);
					foundUsersVector.push_back(foundUser);
				}

				_serverResponse = foundUsersVector;
				_conditionalVariable.notify_one();

				break;
			}
			case ActionType::kGetAvailableChatsForUser:
			{
				size_t availableChatsCount = 0;
				recv(_clientSocket, reinterpret_cast<char*>(&availableChatsCount), sizeof(availableChatsCount), NULL);

				std::vector<std::shared_ptr<Chat::Chat>> availableChatsVector;

				for (size_t i = 0; availableChatsCount > 0; ++i, --availableChatsCount)
				{
					//TODO: do not receive length
					char chatName[50];
					size_t chatNameLength;
					std::shared_ptr<Chat::Chat> foundChat = std::make_shared<Chat::Chat>();

					size_t chatId;
					recv(_clientSocket, reinterpret_cast<char*>(&chatId), sizeof(chatId), NULL);

					recv(_clientSocket, reinterpret_cast<char*>(&chatNameLength), sizeof(chatNameLength), NULL);
					recv(_clientSocket, chatName, chatNameLength, NULL);
					chatName[chatNameLength] = '\0';

					foundChat->SetChatName(chatName);
					foundChat->SetChatId(chatId);

					availableChatsVector.push_back(foundChat);

				}

				_serverResponse = availableChatsVector;
				_conditionalVariable.notify_one();

				break;
			}
			case ActionType::kReceiveAllMessagesForSelectedChat:
			{
				size_t messageCount;
				recv(_clientSocket, reinterpret_cast<char*>(&messageCount), sizeof(messageCount), NULL);

				while (messageCount > 0)
				{
					constexpr const size_t receiveMessageSize = 4096;
					char receiveMessage[receiveMessageSize + 1];

					recv(_clientSocket, receiveMessage, receiveMessageSize, NULL);
					{
						//TODO:: set variable type Buffer::MessageType insted of size_t
						size_t messageType;
						recv(_clientSocket, reinterpret_cast<char*>(&messageType), sizeof(messageType), NULL);

						if (messageType == 2)
						{
							Buffer::MessageBuffer::getInstance().pushFront(Buffer::MessageType::kReceived, receiveMessage);
						}
						else
						{
							Buffer::MessageBuffer::getInstance().pushFront(Buffer::MessageType::kSend, receiveMessage);

						}
					}

					messageCount--;
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

	void Client::ReceiveAllMessagesFromSelectedChat(std::string author, size_t chatId) const noexcept
	{
		//TODO: do not send message length

		Buffer::MessageBuffer::getInstance().~MessageBuffer();

		ActionType type = ActionType::kReceiveAllMessagesForSelectedChat;
		send(_clientSocket, reinterpret_cast<char*>(&type), sizeof(type), NULL);

		send(_clientSocket, reinterpret_cast<char*>(&chatId), sizeof(chatId), NULL);

		size_t authorSize = author.size();
		send(_clientSocket, reinterpret_cast<char*>(&authorSize), sizeof(authorSize), NULL);
		send(_clientSocket, author.c_str(), authorSize, NULL);

	}

	Client::~Client()
	{
		if (_currentClientState != ClientState::kClientDisconnected)
			Client::Disconnect();
	}

	Client::Client() noexcept
	{
		if (WSAStartup(_dllVersion, &_wsaData))
		{
			exit(SOCKET_ERROR);
		}

		_socketAddress.sin_family = AF_INET;
		_socketAddress.sin_addr.s_addr = inet_addr(_ipAddress.c_str());
		_socketAddress.sin_port = htons(_port);

		if ((_clientSocket = socket(AF_INET, SOCK_STREAM, NULL)) == SOCKET_ERROR)
		{
			exit(SOCKET_ERROR);
		}

		_currentClientState = ClientState::kCLientInited;
	}

	bool Client::Connect() noexcept
	{
		if (_currentClientState != ClientState::kCLientInited)
		{
			return false;
		}

		if (connect(_clientSocket, reinterpret_cast<SOCKADDR*>(&_socketAddress), sizeof(_socketAddress)) != 0)
		{
			return false;
		}

		recv(_clientSocket, reinterpret_cast<char*>(&_clientId), sizeof(_clientId), NULL);								//receive client id

		_currentClientState = ClientState::kClientConnected;

		return true;
	}

	void Client::Disconnect() noexcept
	{
		if (_currentClientState != ClientState::kClientDisconnected)
		{
			if (closesocket(_clientSocket) == SOCKET_ERROR)
			{
				exit(SOCKET_ERROR);
			}

			if (WSACleanup() == SOCKET_ERROR)
			{
				exit(SOCKET_ERROR);
			}

			_currentClientState = ClientState::kClientDisconnected;
		}
	}

} // !namespace Network
