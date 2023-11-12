#include "client.h"

namespace Network 
{
	Client& Client::GetInstance() noexcept
	{
		static Client instance;
		if (instance._clientStatus != ClientStatusCode::kClientConnected)
		{
			while (!instance.Connect()) {};
		}

		return instance;
	}

	void Client::Send(size_t userId, const char* data, size_t dataSize) noexcept
	{
		send(this->_clientSocket, reinterpret_cast<char*>(&userId), sizeof(size_t), NULL);			// send the receiver's id
		send(this->_clientSocket, reinterpret_cast<char*>(&dataSize), sizeof(size_t), NULL);		// send the size of the message
		send(this->_clientSocket, data, dataSize, NULL);											// send the message
	}

	void Client::Receive() noexcept
	{
		size_t receiveMessageSize;

		while (true) 
		{
			if (recv(this->_clientSocket, reinterpret_cast<char*>(&receiveMessageSize), sizeof(size_t), NULL) > 0)
			{
				char* receiveMessage = new char[receiveMessageSize + 1];
				receiveMessage[receiveMessageSize] = '\0';

				recv(this->_clientSocket, receiveMessage, receiveMessageSize, NULL);
				
				Buffer::MessageBuffer::GetInstance().PushFront(Buffer::MessageType::kReceived, receiveMessage);
			}
		}
	}

	Client::~Client() noexcept
	{
		if (this->_clientStatus != ClientStatusCode::kClientDisconnected)
			Client::Disconnect();
	}

	Client::Client() noexcept
	{
		if (WSAStartup(this->_dllVersion, &this->_wsaData))
		{
			exit(SOCKET_ERROR);
		}
	
		this->_socketAddress.sin_family = AF_INET;
		this->_socketAddress.sin_addr.s_addr = inet_addr(this->_ipAddress.c_str());
		this->_socketAddress.sin_port = htons(this->_port);

		if ((this->_clientSocket = socket(AF_INET, SOCK_STREAM, NULL)) == SOCKET_ERROR)
		{
			exit(SOCKET_ERROR);
		}

		this->_clientStatus = ClientStatusCode::kCLientInited;
	}

	bool Client::Connect() noexcept
	{
		if (this->_clientStatus != ClientStatusCode::kCLientInited)
		{
			return false;
		}

		if (connect(this->_clientSocket, reinterpret_cast<SOCKADDR*>(&this->_socketAddress), sizeof(this->_socketAddress)) != 0)
		{
			return false;
		}

		recv(this->_clientSocket, reinterpret_cast<char*>(&this->_clientId), sizeof(size_t), NULL);			//receive client id

		this->_clientStatus = ClientStatusCode::kClientConnected;

		return true;
	}

	void Client::Disconnect() noexcept
	{
		if (_clientStatus != ClientStatusCode::kClientDisconnected)
		{
			if (closesocket(_clientSocket) == SOCKET_ERROR) 
			{
				exit(SOCKET_ERROR);
			}

			if (WSACleanup() == SOCKET_ERROR)
			{
				exit(SOCKET_ERROR);
			}

			this->_clientStatus = ClientStatusCode::kClientDisconnected;
		}

	}

}// !namespace Network
