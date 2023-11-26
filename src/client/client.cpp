#include "client.h"

namespace Network 
{
	Client& Client::GetInstance() noexcept
	{
		static Client instance;
		if (instance._clientState != ClientState::kClientConnected)
		{
			while (!instance.Connect())
			{
				//TODO: add !connect handle
			};

			static std::thread receiveThread(&Network::Client::Receive, &Network::Client::GetInstance());
			receiveThread.detach();
		}

		return instance;
	}

	void Client::Send(size_t userId, const char* data, size_t dataSize) noexcept
	{
		send(_clientSocket, reinterpret_cast<char*>(&userId), sizeof(size_t), NULL);								// send the receiver's id
		send(_clientSocket, reinterpret_cast<char*>(&dataSize), sizeof(size_t), NULL);								// send the size of the message
		send(_clientSocket, data, dataSize, NULL);																	// send the message
	}

	void Client::Receive() noexcept
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

	Client::~Client() 
	{
		if (_clientState != ClientState::kClientDisconnected)
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

		_clientState = ClientState::kCLientInited;
	}

	bool Client::Connect() noexcept
	{
		if (_clientState != ClientState::kCLientInited)
		{
			return false;
		}

		if (connect(_clientSocket, reinterpret_cast<SOCKADDR*>(&_socketAddress), sizeof(_socketAddress)) != 0)
		{
			return false;
		}

		recv(_clientSocket, reinterpret_cast<char*>(&_clientId), sizeof(size_t), NULL);								//receive client id

		_clientState = ClientState::kClientConnected;

		return true;
	}

	void Client::Disconnect() noexcept
	{
		if (_clientState != ClientState::kClientDisconnected)
		{
			if (closesocket(_clientSocket) == SOCKET_ERROR) 
			{
				exit(SOCKET_ERROR);
			}

			if (WSACleanup() == SOCKET_ERROR)
			{
				exit(SOCKET_ERROR);
			}

			_clientState = ClientState::kClientDisconnected;
		}

	}

} // !namespace Network
