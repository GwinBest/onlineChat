#pragma once

#include <winsock2.h>

#include <cstdint>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "../messageBuffer/messageBuffer.h"
#include "../userData/userData.h"

#pragma comment (lib, "ws2_32.lib")
#pragma warning (disable:4996)

namespace Network
{
	enum class ActionType : uint32_t
	{
		kUserChatMessage				= 0,
		kAddUserCredentialsToDatabase	= 1,
		kCheckUserExistence				= 2,
		kGetUserNameFromDatabase		= 3
	};

	struct UserRequest
	{
		ActionType actionType;
		std::string name;
		std::string login;
		size_t password;
	};

	class Client
	{
	public:
		Client(const Client&) = delete;
		void operator= (const Client&) = delete;

		static Client& GetInstance() noexcept;

		void SendUserMessage(size_t userId, const char* data, size_t dataSize) noexcept;
		[[noreturn]] void ReceiveUserMessageThread() noexcept;

		void SendUserCredentials(UserRequest& userCredentials) const noexcept;
		std::string ReceiveServerResponse() noexcept;

		~Client();

	private:
		Client() noexcept;

		bool Connect() noexcept;
		void Disconnect() noexcept;

	private:
		enum class ClientState : uint8_t
		{
			kClientDisconnected = 0,
			kCLientInited		= 1,
			kClientConnected	= 2,
		};

		static constexpr WORD _dllVersion = MAKEWORD(2, 2);
		WSADATA _wsaData;
		SOCKET _clientSocket;
		SOCKADDR_IN _socketAddress;
		ClientState _currentClientState = ClientState::kClientDisconnected;

		std::mutex mutex;
		std::condition_variable conditionalVariable;

		static inline char _serverResponse[255];

		size_t _clientId;
		const std::string _ipAddress = "127.0.0.1";
		static constexpr uint32_t _port = 8080;
	};

} // !namespase Network
