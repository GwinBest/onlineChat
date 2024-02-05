#pragma once

#include <winsock2.h>

#include <cstdint>
#include <string>
#include <variant>
#include <vector>

#include "../chatSystem/chat.h"
#include "../userData/user.h"

// forward declaration
namespace UserData
{
	class User;
}
namespace ChatSystem
{
	class Chat;
}

namespace NetworkCore
{
	enum class ActionType : uint8_t
	{
		kActionUndefined								= 0,
		kSendChatMessage,					
		kAddUserCredentialsToDatabase,		
		kCheckUserExistence,					
		kGetUserNameFromDatabase,
		kGetUserIdFromDatabase,
		kFindUsersByLogin,					
		kGetAvailableChatsForUser,
		kReceiveAllMessagesForSelectedChat,
		kServerError,
	};

	struct UserPacket
	{
		ActionType actionType = ActionType::kActionUndefined;
		std::string name = "";
		std::string login = "";
		size_t password = 0;
		size_t id = 0;
	};

	struct ChatPacket
	{
		ActionType actionType = ActionType::kActionUndefined;
		std::string chatName = "";
		std::string chatUserLogin = "";
		size_t chatId = 0;
	};

	constexpr WORD dllVersion = MAKEWORD(2, 2);

	const std::string ipAddress = "192.168.0.102";
	constexpr uint32_t port = 8080;

	constexpr size_t serverResponseSize = 255;

	using ServerResponse = std::variant<bool, size_t, std::string, std::vector<UserData::User>, std::vector<ChatSystem::Chat>>;
	inline ServerResponse serverResponse;

} // !namespace NetworkCore