#include "client.h"

#include "../userData/userData.h"
#include "../chat/chat.h"

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
		//TODO: do not sand name and login length

		ActionType type = ActionType::kSendUserChatMessage;
		send(_clientSocket, reinterpret_cast<char*>(&type), sizeof(type), NULL);

		size_t currentUserLoginSize = currentUserLogin.size();
		send(_clientSocket, reinterpret_cast<char*>(&currentUserLoginSize), sizeof(currentUserLoginSize), NULL);
		send(_clientSocket, currentUserLogin.c_str(), currentUserLoginSize, NULL);

		size_t userLoginSize = selectedUserLogin.size();
		send(_clientSocket, reinterpret_cast<char*>(&userLoginSize), sizeof(userLoginSize), NULL);
		send(_clientSocket, selectedUserLogin.c_str(), userLoginSize, NULL);

		size_t dataSize = data.size();
		send(_clientSocket, reinterpret_cast<char*>(&dataSize), sizeof(dataSize), NULL);
		send(_clientSocket, data.c_str(), dataSize, NULL);
	}

	void Client::SendUserCredentialsPacket(const UserPacket& userCredentials) const noexcept
	{
		//TODO: do not sand name and login length

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
		//TODO: do not sand name length

		ActionType type = chatInfo.actionType;
		send(_clientSocket, reinterpret_cast<char*>(&type), sizeof(type), NULL);

		size_t currentUserLoginLength = chatInfo.currentUserLogin.size();
		send(_clientSocket, reinterpret_cast<char*>(&currentUserLoginLength), sizeof(currentUserLoginLength), NULL);
		send(_clientSocket, chatInfo.currentUserLogin.c_str(), currentUserLoginLength, NULL);

		size_t chatNameLength = chatInfo.chatName.size();
		send(_clientSocket, reinterpret_cast<char*>(&chatNameLength), sizeof(chatNameLength), NULL);
		send(_clientSocket, chatInfo.chatName.c_str(), chatNameLength, NULL);
		
		send(_clientSocket, reinterpret_cast<char*>(&const_cast<size_t&>(chatInfo.chatId)), sizeof(chatInfo.chatId), NULL);
	}
	
	void Client::ReceiveThread() const noexcept
	{
		while (true)
		{
			ActionType type = ActionType::kActionUndefined;
			recv(_clientSocket, reinterpret_cast<char*>(&type), sizeof(type), NULL);

			switch (type)
			{
			case ActionType::kSendUserChatMessage:
			{
				constexpr const size_t receiveMessageSize = 4096;
				char receiveMessage[4097];

				if((recv(_clientSocket, receiveMessage, receiveMessageSize, NULL)) > 0)
				{
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

				std::vector<UserData::User*> foundUsersVector;

				for (size_t i = 0; foundUsersCount > 0; i++, foundUsersCount--)
				{
					char userLogin[50];
					size_t userLoginLength;
					UserData::User* foundUser = new UserData::User;

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

				std::vector<Chat::Chat*> availableChatsVector;

				for (size_t i = 0; availableChatsCount > 0; i++, availableChatsCount--)
				{
					char chatName[50];
					size_t chatNameLength;
					Chat::Chat* foundChat= new Chat::Chat;

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
				size_t messagesCount;
				recv(_clientSocket, reinterpret_cast<char*>(&messagesCount), sizeof(messagesCount), NULL);

				while (messagesCount > 0)
				{
					constexpr const size_t messageSize = 4096;
					char message[4097];

					recv(_clientSocket, message, messageSize, NULL);

					size_t msgType;
					recv(_clientSocket, reinterpret_cast<char*>(&msgType), sizeof(msgType), NULL);
					if (msgType == 2)//TODO : recv message type instead of number
					{
						Buffer::MessageBuffer::getInstance().pushFront(Buffer::MessageType::kReceived, message);
					}
					else
					{
						Buffer::MessageBuffer::getInstance().pushFront(Buffer::MessageType::kSend, message);

					}

					messagesCount--;
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

	void Client::ReceiveAllMessagesFromSelectedChat(std::string currentUser, size_t chatId) const noexcept
	{
		//TODO: do not sand name length

		ActionType type = ActionType::kReceiveAllMessagesForSelectedChat;
		send(_clientSocket, reinterpret_cast<char*>(&type), sizeof(type), NULL);

		send(_clientSocket, reinterpret_cast<char*>(&chatId), sizeof(chatId), NULL);

		size_t currentUserSize = currentUser.size();
		send(_clientSocket, reinterpret_cast<char*>(&currentUserSize), sizeof(currentUserSize), NULL);
		send(_clientSocket, currentUser.c_str(), currentUserSize, NULL);

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
