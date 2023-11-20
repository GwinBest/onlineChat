#ifndef _ONLINE	CHAT_CLIENT_CLIENT_H_
#define _ONLINE_CHAT_CLIENT_CLIENT_H_

#include <winsock2.h>

#include <cstdint>
#include <string>
#include <thread>

#include "../messageBuffer/messageBuffer.h"

#pragma comment (lib, "ws2_32.lib")
#pragma warning (disable:4996)

namespace Network
{
	class Client
	{
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
		void Disconnect() noexcept;

	private:
		enum class ClientStatusCode : uint8_t
		{
			kClientDisconnected = 0,
			kCLientInited = 1,
			kClientConnected = 2,
		};

		static constexpr WORD _dllVersion = MAKEWORD(2, 2);
		WSADATA _wsaData;
		SOCKET _clientSocket;
		SOCKADDR_IN _socketAddress;
		ClientStatusCode _clientStatus = ClientStatusCode::kClientDisconnected;

		size_t _clientId;
		const std::string _ipAddress = "192.168.43.208";
		static constexpr uint32_t _port = 8080;
	};

} // !namespase Network

#endif // !_ONLINE_CHAT_CLIENT_CLIENT_H_