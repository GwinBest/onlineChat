#pragma once

#include <cstdint>

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
		
		//void ReceiveUserMessage(UserData::User& sender, const UserData::User& receiver, const char* data) const noexcept;
		NetworkCore::UserPacket ReceiveUserCredentialsPacket(size_t index) const noexcept;
		//void ReceiveChatInfoPacket(const ChatPacket& chatInfo) const noexcept;

		void SendServerErrorMessage(const size_t index, const std::string& errorMessage) const noexcept;

	private:
		WSADATA _wsaData;
		SOCKET _serverSocket;
		SOCKADDR_IN _socketAddress;

		SOCKET _connections[100];
		uint8_t _connectionsCurrentCount = 0;
	};

} // !namespace ServerNetworking

