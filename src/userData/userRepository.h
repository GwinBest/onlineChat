#pragma once

#include <optional>
#include <string>
#include <vector>

#include "chatSystem/chatInfo.h"
#include "coroutineUtils/coroutineUtils.h"
#include "messageBuffer/messageBuffer.h"
#include "userData/user.h"

namespace UserData
{
    using namespace CoroutineUtils;

    class UserRepository final
    {
    public:
        [[nodiscard]] static Awaitable<std::optional<bool>>
            PushUserCredentialsToDatabaseAsync(const User& user);
        [[nodiscard]] static Awaitable<std::optional<std::string>>
            GetUserNameFromDatabaseAsync(const std::string& userLogin);
        [[nodiscard]] static Awaitable<std::optional<size_t>>
            GetUserIdFromDatabaseAsync(const std::string& userLogin);
        [[nodiscard]] static Awaitable<std::optional<bool>>
            IsUserDataFromFileValidAsync(const User& user);
        [[nodiscard]] static Awaitable<std::optional<bool>> IsUserExistAsync(const User& user);
        [[nodiscard]] static Awaitable<std::optional<std::vector<ChatSystem::ChatInfo>>>
            FindMatchingChatsAsync(const size_t currentUserId, const std::string& pattern);
        [[nodiscard]] static Awaitable<std::optional<std::vector<ChatSystem::ChatInfo>>>
            GetAvailableChatsForUserAsync(const size_t userId);
        [[nodiscard]] static Awaitable<std::optional<std::vector<MessageBuffer::MessageNode>>>
            GetAvailableChatMessagesAsync(size_t userId, size_t chatId);
        [[nodiscard]] static Awaitable<std::optional<size_t>>
            CreateNewPersonalChatAsync(size_t senderUserId, const std::string& receiverUserName);
    };
}   // namespace UserData