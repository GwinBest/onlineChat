#include "client.h"

#include "../userData/userData.h"

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

	void Client::SendUserMessage(size_t userId, const char* data, size_t dataSize) const noexcept
	{
		ActionType type = ActionType::kUserChatMessage;
		send(_clientSocket, reinterpret_cast<char*>(&type), sizeof(type), NULL);

		send(_clientSocket, reinterpret_cast<char*>(&userId), sizeof(userId), NULL);									// send the receiver's id
		send(_clientSocket, reinterpret_cast<char*>(&dataSize), sizeof(dataSize), NULL);								// send the size of the message
		send(_clientSocket, data, dataSize, NULL);																		// send the message
	}

	void Client::SendUserCredentials(UserRequest& userCredentials) const noexcept
	{
		ActionType type = userCredentials.actionType;
		send(_clientSocket, reinterpret_cast<char*>(&type), sizeof(type), NULL);

		size_t nameLength = userCredentials.name.size();
		send(_clientSocket, reinterpret_cast<char*>(&nameLength), sizeof(nameLength), NULL);
		send(_clientSocket, userCredentials.name.c_str(), nameLength, NULL);

		size_t loginLength = userCredentials.login.size();
		send(_clientSocket, reinterpret_cast<char*>(&loginLength), sizeof(loginLength), NULL);
		send(_clientSocket, userCredentials.login.c_str(), loginLength, NULL);

		send(_clientSocket, reinterpret_cast<char*>(&userCredentials.password), sizeof(userCredentials.password), NULL);
	}
	
	void Client::ReceiveThread() const noexcept
	{
		while (true)
		{
			ActionType type = ActionType::kActionUndefined;
			recv(_clientSocket, reinterpret_cast<char*>(&type), sizeof(type), NULL);

			switch (type)
			{
			case ActionType::kUserChatMessage:
			{
				constexpr const size_t receiveMessageSize = 4096;
				char receiveMessage[4097];
				uint32_t receivedSize;
				if((receivedSize = recv(_clientSocket, receiveMessage, receiveMessageSize, NULL)) > 0)
				{
					receiveMessage[receivedSize] = '\0';
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
				conditionalVariable.notify_one();

				break;
			}
			case ActionType::kFindUsersByLogin:
			{
				size_t foundUsersCount = 0;
				recv(_clientSocket, reinterpret_cast<char*>(&foundUsersCount), sizeof(foundUsersCount), NULL);

				std::vector<UserData::User*> foundUsersVector;

				size_t i = 0;
				while (foundUsersCount > 0)
				{
					char userLogin[50];
					size_t userLoginLength;
					UserData::User* foundUser = new UserData::User;

					recv(_clientSocket, reinterpret_cast<char*>(&userLoginLength), sizeof(userLoginLength), NULL);
					recv(_clientSocket, userLogin, userLoginLength, NULL);
					userLogin[userLoginLength] = '\0';

					foundUser->SetUserLogin(userLogin);
					foundUsersVector.push_back(foundUser);

					foundUsersCount--;
				}

				_serverResponse = foundUsersVector;
				conditionalVariable.notify_one();

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
