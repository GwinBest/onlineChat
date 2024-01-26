#pragma once

#include <condition_variable>
#include <mutex>

#ifndef NDEBUG
#include <iostream>
#endif // NDEBUG

#include "../networkCore/networkCore.h"

#pragma comment (lib, "ws2_32.lib")
#pragma warning (disable:4996)

namespace ClientNetworking
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

		Client(const Client&) = delete;
		void operator= (const Client&) = delete;

		static Client& GetInstance() noexcept;

		void SendUserMessage(const std::string& sender, const std::string& receiver, const char* data) const noexcept;
		void SendUserCredentialsPacket(const UserPacket& userCredentials) const noexcept;
		void SendChatInfoPacket(const ChatPacket& chatInfo) const noexcept;

		void ReceiveThread() const noexcept;

		template<typename T>
		const T& GetServerResponse() const noexcept
		{
			std::unique_lock<std::mutex> lock(_mutex);
			_conditionalVariable.wait(lock);

			return std::get<T>(NetworkCore::serverResponse);
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
			kCLientInited,
			kClientConnected,
		};

		WSADATA _wsaData;
		SOCKET _clientSocket;
		SOCKADDR_IN _socketAddress;
		ClientState _currentClientState = ClientState::kClientDisconnected;

		mutable std::mutex _mutex;
		mutable std::condition_variable _conditionalVariable;
	};

} // !namespace ClientNetworking
