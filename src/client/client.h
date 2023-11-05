#ifndef _ONLINE	CHAT_CLIENT_CLIENT_H_
#define _ONLINE_CHAT_CLIENT_CLIENT_H_

#pragma comment (lib, "ws2_32.lib")
#pragma warning (disable:4996)

#include <winsock2.h>

#include <iostream>


namespace Network
{
	class Client
	{
	private:
		enum clientStatusCode : uint8_t
		{
			kClientDisconnected = 0,
			kCLientInited = 1,
			kClientConnected = 2,
		};

		static constexpr WORD _dllVersion = MAKEWORD(2, 2);
		WSADATA _wsaData;
		SOCKET _clientSocket;
		SOCKADDR_IN _socketAddress;
		uint8_t _clientStatus = kClientDisconnected;

		size_t _clientId;
		const std::string _ipAddress = "127.0.0.1";
		static constexpr uint32_t _port = 1111;

	public:
		Client(const Client&) = delete;
		void operator= (const Client&) = delete;

		static Client& GetInstance() noexcept;

		void Send(size_t userId, const char* data, size_t dataSize) noexcept;
		void Receive() noexcept;

		~Client();

	private:
		Client() noexcept;

		bool Connect() noexcept;
		bool Disconnect() noexcept;
	};

} //!namespase Network

#endif // !_ONLINE_CHAT_CLIENT_CLIENT_H_