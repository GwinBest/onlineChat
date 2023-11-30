#include "client.h"

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

			static std::thread receiveThread(&Network::Client::ReceiveUserMessageThread, &Network::Client::GetInstance());
			receiveThread.detach();
		}

		return instance;
	}

	void Client::SendUserMessage(size_t userId, const char* data, size_t dataSize) noexcept
	{
		send(_clientSocket, reinterpret_cast<char*>(&userId), sizeof(size_t), NULL);								// send the receiver's id
		send(_clientSocket, reinterpret_cast<char*>(&dataSize), sizeof(size_t), NULL);								// send the size of the message
		send(_clientSocket, data, dataSize, NULL);																	// send the message
	}

	void Client::ReceiveUserMessageThread() noexcept
	{
		size_t receiveMessageSize;

		while (true) 
		{
			if (recv(_clientSocket, reinterpret_cast<char*>(&receiveMessageSize), sizeof(size_t), NULL) > 0)
			{
				char* receiveMessage = new char[receiveMessageSize + 1];
				receiveMessage[receiveMessageSize] = '\0';

				recv(_clientSocket, receiveMessage, receiveMessageSize, NULL);
				
				Buffer::MessageBuffer::getInstance().pushFront(Buffer::MessageType::kReceived, receiveMessage);

				delete[] receiveMessage;
			}
		}
	}

		void Client::SendUserCredentials(UserRequest& userCredentials) const noexcept
		{
			// TODO: static name and login array instead of std::string

			ActionType type = userCredentials.actionType;
			send(_clientSocket, reinterpret_cast<char*>(&type), sizeof(type), NULL);

			size_t nameLength = userCredentials.name.size();
			send(_clientSocket, reinterpret_cast<char*>(&nameLength), sizeof(nameLength), NULL);
			send(_clientSocket, userCredentials.name.c_str(), nameLength, NULL);

			size_t loginLength = userCredentials.login.size();
			send(_clientSocket, reinterpret_cast<char*>(&loginLength), sizeof(loginLength), NULL);
			send(_clientSocket, userCredentials.login.c_str(), loginLength, NULL);

			size_t passwordLength = userCredentials.password.size();
			send(_clientSocket, reinterpret_cast<char*>(&passwordLength), sizeof(passwordLength), NULL);
			send(_clientSocket, userCredentials.password.c_str(), passwordLength, NULL);
		}

		std::string Client::ReceiveServerResponse() noexcept
		{
			ActionType type;
			size_t responseSize;
			std::string response;

			if (recv(_clientSocket, reinterpret_cast<char*>(&type), sizeof(type), NULL) != SOCKET_ERROR)
			{
				recv(_clientSocket, reinterpret_cast<char*>(&responseSize), sizeof(responseSize), NULL);
				recv(_clientSocket, const_cast<char*>(response.c_str()), responseSize, NULL);
			}

			return response;
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

		recv(_clientSocket, reinterpret_cast<char*>(&_clientId), sizeof(size_t), NULL);								//receive client id

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
