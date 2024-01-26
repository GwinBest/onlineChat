#pragma once

#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <iostream>
#include <WS2tcpip.h>
#include <string>
#include <thread>

#include "mysql_connection.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

#pragma warning(disable: 4996)

class Server
{
public:
	void Start() noexcept;
	[[noreturn]] void Run() noexcept;

private:
	void ClientHandler(int index);

private:
	enum class ActionType : uint8_t
	{
		kActionUndefined = 0,
		kSendChatMessage = 1,
		kAddUserCredentialsToDatabase = 2,
		kCheckUserExistence = 3,
		kGetUserNameFromDatabase = 4,
		kFindUsersByLogin = 5,

		kGetAvailableChatsForUser = 6,
		kReceiveAllMessagesForSelectedChat = 7
	};

	static constexpr WORD _dllVersion = MAKEWORD(2, 1);
	WSADATA _wsaData;
	SOCKET _serverSocket;
	SOCKADDR_IN _socketAddress;

	SOCKET _connections[100];
	int _connectionsCurrentCount = 0;


	const std::string _ipAddress = "192.168.0.102";
	static constexpr uint32_t _port = 8080;

};

