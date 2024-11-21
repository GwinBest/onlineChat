#pragma once

#include <optional>
#include <string>
#include <vector>

#include "chatSystem/chatInfo.h"

#include "userData/user.h"

namespace UserData
{
    class UserRepository final
    {
    public:
        [[nodiscard]] static std::optional<bool> PushUserCredentialsToDatabase(const User& user);
        [[nodiscard]] static std::optional<std::string> GetUserNameFromDatabase(const std::string& userLogin);
        [[nodiscard]] static std::optional<size_t> GetUserIdFromDatabase(const std::string& userLogin);
        [[nodiscard]] static std::optional<bool> IsUserDataFromFileValid(const User& user);
        [[nodiscard]] static std::optional<bool> IsUserExist(const User& user);
        [[nodiscard]] static std::optional<std::vector<ChatSystem::ChatInfo>> FindMatchingChats(const size_t currentUserId, const std::string& pattern);
        [[nodiscard]] static std::optional<std::vector<ChatSystem::ChatInfo>> GetAvailableChatsForUser(const size_t userId);
    };
} // !namespace UserData