#pragma once

#include <winsock2.h>

#include <cstdint>
#include <string>
#include <variant>
#include <vector>

#include "../chat/chat.h"
#include "../userData/userData.h"

namespace NetworkCore
{
	enum class ActionType : uint8_t
	{
		kActionUndefined					= 0,
		kSendChatMessage					= 1,
		kAddUserCredentialsToDatabase		= 2,
		kCheckUserExistence					= 3,
		kGetUserNameFromDatabase			= 4,
		kFindUsersByLogin					= 5,
		kGetAvailableChats					= 6,
		kReceiveAllMessagesForSelectedChat	= 7
	};

	constexpr WORD dllVersion = MAKEWORD(2, 2);

	const std::string ipAddress = "192.168.0.102";
	constexpr uint32_t port = 8080;

	constexpr size_t serverResponseSize = 255;

} // !namespace NetworkCore