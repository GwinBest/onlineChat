#pragma once
#pragma comment (lib, "ws2_32.lib")
#pragma warning (disable:4996)

#include <iostream>

#include <winsock2.h>

namespace Network
{
	class Client
	{
	private:
		enum clientStatusCode : uint8_t
		{
			kClientDisconnected = 0,
			kCLientInited		= 1,
			kClientConnected	= 2,
		};

		static constexpr WORD _dllVersion = MAKEWORD(2, 2);
		WSADATA _wsaData;
		SOCKET _clientSocket;
		SOCKADDR_IN _socketAddress;
		uint8_t _clientStatus = kClientDisconnected;

		size_t _clientId;
		std::string _ipAddress;
		uint32_t _port;

	public:
		Client(const std::string ipAddress, const uint32_t port);
		~Client();

		bool Connect();
		bool Disconnect();

		void Send(size_t userId, const char* data, size_t dataSize);
		void StartReceiveThread();

	private:
		static void ReceiveThread(LPVOID lpParam);
		void Receive();
		
	};
}