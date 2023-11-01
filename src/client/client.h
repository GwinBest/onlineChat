#pragma once
#pragma comment (lib, "ws2_32.lib")
#pragma warning (disable:4996)

#include <iostream>

#include <winsock2.h>

#include "../buffer.h"

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

		std::string _ipAddress;
		uint32_t _port;
		DataBuffer_t _Buffer;

	public:
		Client(const std::string ipAddress, const uint32_t port);
		~Client();

		bool Connect();
		bool Disconnect();

		void Send(const char* data, size_t dataSize);
		void StartReceiveThread();

	private:
		static void ReceiveThread(LPVOID lpParam);
		void Receive();
	};
}