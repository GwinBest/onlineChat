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
		kActionUndefined					= 0,
		kSendChatMessage,					
		//kReceiveChatMessage,					
		kAddUserCredentialsToDatabase,		
		kCheckUserExistence,					
		kGetUserNameFromDatabase,			
		kFindUsersByLogin,					
		kGetAvailableChatsForUser,
		kReceiveAllMessagesForSelectedChat
	};

	constexpr WORD dllVersion = MAKEWORD(2, 2);

	const std::string ipAddress = "192.168.0.102";
	constexpr uint32_t port = 8080;

	constexpr size_t serverResponseSize = 255;

	using ServerResponse = std::variant<bool, std::string, std::vector<UserData::User>, std::vector<ChatSystem::Chat>>;
	inline ServerResponse serverResponse;

} // !namespace NetworkCore