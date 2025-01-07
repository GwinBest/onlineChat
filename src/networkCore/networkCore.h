#pragma once

#include <winsock2.h>

#include <cstdint>
#include <string>
#include <vector>

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

    constexpr WORD dllVersion = MAKEWORD(2, 2);

    const std::string ipAddress = "127.0.0.1";
    constexpr uint32_t port = 8080;

    constexpr size_t serverResponseSize = 255;
} // !namespace NetworkCore