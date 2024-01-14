#pragma once

#include <condition_variable>
#include <mutex>
#include <thread>
#include <variant>
#include <vector>

#ifndef NDEBUG
#include <iostream>
#endif // NDEBUG

#include "../chat/chat.h"
#include "../messageBuffer/messageBuffer.h"
#include "../networkCore/networkCore.h"

#pragma comment (lib, "ws2_32.lib")
#pragma warning (disable:4996)

// forward declaration
namespace UserData
{
	class User;
}
namespace Chat
{
	class Chat;
}

extern std::list<MessageBuffer::MessageNode> MessageBuffer::messageBuffer;

namespace Network
{
	struct UserPacket
	{
		NetworkCore::ActionType actionType				= NetworkCore::ActionType::kActionUndefined;
		std::string name								= "";
		std::string login								= "";
		size_t password									= 0;
	};

	struct ChatPacket
	{
		NetworkCore::ActionType actionType				= NetworkCore::ActionType::kActionUndefined;
		std::string chatName							= "";
		std::string chatUserLogin						= "";
		size_t chatId									= 0;
	};

	class Client final
	{
	public:
		using ServerResponse = std::variant<bool, std::string, std::vector<UserData::User>, std::vector<Chat::Chat>>;

		Client(const Client&) = delete;
		void operator= (const Client&) = delete;

		static Client& GetInstance() noexcept;

		void SendUserMessage(const std::string& currentUserLogin, const std::string& selectedUserLogin, const std::string data) const noexcept;
		void SendUserCredentialsPacket(const UserPacket& userCredentials) const noexcept;
		void SendChatInfoPacket(const ChatPacket& chatInfo) const noexcept;

		void ReceiveThread() const noexcept;

		template<typename T>
		T GetServerResponse() const noexcept
		{
			std::unique_lock<std::mutex> lock(_mutex);
			_conditionalVariable.wait(lock);

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

		WSADATA _wsaData;
		SOCKET _clientSocket;
		SOCKADDR_IN _socketAddress;
		ClientState _currentClientState = ClientState::kClientDisconnected;

		mutable std::mutex _mutex;
		mutable std::condition_variable _conditionalVariable;

		mutable ServerResponse _serverResponse;
	};

} // !namespace Network
