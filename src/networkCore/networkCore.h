#pragma once

#ifdef WIN32
#include <winsock2.h>
#endif // WIN32

#include <cstdint>
#include <string>
#include <string_view>

namespace NetworkCore
{
    enum class ActionType : uint8_t
    {
        kActionUndefined = 0,
        kSendChatMessage,
        kAddUserCredentialsToDatabase,
        kCheckUserExistence,
        kGetUserNameFromDatabase,
        kGetUserIdFromDatabase,
        kFindMatchingChats,
        kGetAvailableChatsForUser,
        kReceiveAllMessagesForSelectedChat,
        kCheckIsUserDataFromFileValid,
        kServerError,
        kCreateNewPersonalChat,
    };

    struct UserPacket final
    {
        ActionType actionType = ActionType::kActionUndefined;
        std::string name;
        std::string login;
        size_t password = 0;
        size_t id = 0;
    };

    struct ChatPacket final
    {
        ActionType actionType = ActionType::kActionUndefined;
        size_t chatUserId = 0;
        size_t id = 0;
    };

#ifdef WIN32
    constexpr WORD dllVersion = MAKEWORD(2, 2);
#endif // WIN32

    extern const std::string_view ipAddress;
    extern const uint32_t port;

    constexpr uint8_t serverResponseSize = UINT8_MAX;
} // !namespace NetworkCore