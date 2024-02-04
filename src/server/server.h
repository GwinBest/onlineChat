#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>

#include "../networkCore/networkCore.h"

#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable: 4996)

namespace ServerNetworking
{
	class Server final
	{
	public:
		void Start() noexcept;
		[[noreturn]] void Run() noexcept;

	private:
		void ClientHandler(int index);

	private:
		WSADATA _wsaData;
		SOCKET _serverSocket;
		SOCKADDR_IN _socketAddress;

		std::unordered_map< SOCKET, std::string> _connet;
		SOCKET _connections[100];
		uint8_t _connectionsCurrentCount = 0;
	};

} // !namespace ServerNetworking

