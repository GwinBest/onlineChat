#include "client.h"

namespace Network 
{
	Client& Client::GetInstance() noexcept
	{
		static Client instance;
		if (instance._clientStatus != kClientConnected)
			instance.Connect();

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
				std::cout << receiveMessage << std::endl;

				delete[] receiveMessage;
			}
		}
	}

	Client::~Client() noexcept
	{
		if (this->_clientStatus != kClientDisconnected)
			Client::Disconnect();
	}

	Client::Client() noexcept
	{
		WSAStartup(this->_dllVersion, &this->_wsaData);

		this->_socketAddress.sin_family = AF_INET;
		this->_socketAddress.sin_addr.s_addr = inet_addr(this->_ipAddress.c_str());
		this->_socketAddress.sin_port = htons(this->_port);

		socket(AF_INET, SOCK_STREAM, NULL);
			
		this->_clientStatus = kCLientInited;
	}

	bool Client::Connect() noexcept
	{
		if (this->_clientStatus != kCLientInited)
		{
			std::cout << "Client already connected" << std::endl;
			return false;
		}

		if (connect(this->_clientSocket, reinterpret_cast<SOCKADDR*>(&this->_socketAddress), sizeof(this->_socketAddress)) != 0)
		{
			std::cout << "connect error " << GetLastError() << std::endl;
			return false;
		}

		recv(this->_clientSocket, reinterpret_cast<char*>(&this->_clientId), sizeof(size_t), NULL);			//receive client id
		std::cout << this->_clientId;

		this->_clientStatus = kClientConnected;

		return true;
	}

	bool Client::Disconnect() noexcept
	{
		if (this->_clientStatus == kClientDisconnected)
		{
			std::cout << "Client already disconnected " << std::endl;
			return false;
		}

		if (WSACleanup() == SOCKET_ERROR)
		{
			std::cout << "wsaCleanup error " << GetLastError() << std::endl;
			return false;
		}

		if (closesocket(this->_clientSocket) == SOCKET_ERROR)
		{
			std::cout << "closesocket error " << GetLastError() << std::endl;
			return false;
		}

		this->_clientStatus = kClientDisconnected;

		return true;
	}

}// !namespace Network
