#pragma once

#include <winsock2.h>

#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <string>
#include <thread>
#include <variant>
#include <vector>

#include "../messageBuffer/messageBuffer.h"

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

//TODO add a core for client and server, const response size
namespace Network
{
	enum class ActionType : uint32_t
	{
		kActionUndefined					= 0,
		kSendUserChatMessage				= 1,
		kAddUserCredentialsToDatabase		= 2,
		kCheckUserExistence					= 3,
		kGetUserNameFromDatabase			= 4,
		kFindUsersByLogin					= 5,

		kGetAvailableChatsForUser			= 6,
		kReceiveAllMessagesForSelectedChat	= 7
	};

	struct UserPacket
	{
		ActionType actionType	= ActionType::kActionUndefined;
		std::string name		= "";
		std::string login		= "";
		size_t password			= 0;
	};

	struct ChatPacket
	{
		ActionType actionType			= ActionType::kActionUndefined;
		std::string chatName			= "";
		std::string currentUserLogin	= "";
		size_t chatId					= 0;
	};

	class Client final
	{
	public:
		using ServerResponse = std::variant<std::string, std::vector<UserData::User*>, std::vector<Chat::Chat*>>;

		Client(const Client&) = delete;
		void operator= (const Client&) = delete;

		static Client& GetInstance() noexcept;

		void SendUserMessage(const std::string& currentUserLogin, const std::string& selectedUserLogin, const std::string data) const noexcept;
		void SendUserCredentialsPacket(const UserPacket& userCredentials) const noexcept;
		void SendChatInfoPacket(const ChatPacket& chatInfo) const noexcept;

		[[noreturn]] void ReceiveThread() const noexcept;
		void ReceiveAllMessagesFromSelectedChat(std::string author, size_t chatId) const noexcept;

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

		static constexpr WORD _dllVersion = MAKEWORD(2, 2);
		WSADATA _wsaData;
		SOCKET _clientSocket;
		SOCKADDR_IN _socketAddress;
		ClientState _currentClientState = ClientState::kClientDisconnected;

		mutable std::mutex _mutex;
		mutable std::condition_variable _conditionalVariable;

		mutable ServerResponse _serverResponse;

		const std::string _ipAddress = "127.0.0.1";
		static constexpr const uint32_t _port = 8080;
	};

} // !namespase Network
