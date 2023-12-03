#pragma once

#include <winsock2.h>

#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <string>
#include <thread>
#include <variant>

#include "../messageBuffer/messageBuffer.h"

#pragma comment (lib, "ws2_32.lib")
#pragma warning (disable:4996)

// forward declaration
namespace UserData 
{
	class User; 
}

namespace Network
{
	enum class ActionType : uint32_t
	{
		kActionUndefined				= 0,
		kUserChatMessage				= 1,
		kAddUserCredentialsToDatabase	= 2,
		kCheckUserExistence				= 3,
		kGetUserNameFromDatabase		= 4,
		kFindUsersByLogin				= 5
	};

	struct UserRequest
	{
		ActionType actionType;
		std::string name;
		std::string login;
		size_t password;
	};

	class Client final
	{
	public:
		using ServerResponse = std::variant<std::string, UserData::User*>;

		Client(const Client&) = delete;
		void operator= (const Client&) = delete;

		static Client& GetInstance() noexcept;

		void SendUserMessage(size_t userId, const char* data, size_t dataSize) const noexcept;
		void SendUserCredentials(UserRequest& userCredentials) const noexcept;

		[[noreturn]] void ReceiveThread() const noexcept;
		template<typename T>
		T GetServerResponse() const noexcept
		{
			std::unique_lock<std::mutex> lock(mutex);
			conditionalVariable.wait(lock);

			return std::get<T>(_serverResponse);
		}

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

		mutable std::mutex mutex;
		mutable std::condition_variable conditionalVariable;

		mutable ServerResponse _serverResponse;

		size_t _clientId;
		const std::string _ipAddress = "127.0.0.1";
		static constexpr uint32_t _port = 8080;
	};

} // !namespase Network
