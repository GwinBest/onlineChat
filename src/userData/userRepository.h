#pragma once

#include <optional>
#include <string>
#include <vector>

#include "userData/user.h"

#include "chatSystem/chat.h"

namespace UserData
{
    class UserRepository final
    {
    public:
        [[nodiscard]] static std::optional<bool> PushUserCredentialsToDatabase(const User& user) noexcept;
        [[nodiscard]] static std::optional<std::string> GetUserNameFromDatabase(const std::string& userLogin) noexcept;
        [[nodiscard]] static std::optional<size_t> GetUserIdFromDatabase(const std::string& userLogin) noexcept;
        [[nodiscard]] static std::optional<bool> IsUserDataFromFileValid(const User& user) noexcept;
        [[nodiscard]] static std::optional<bool> IsUserExist(const User& user) noexcept;
        [[nodiscard]] static std::optional<std::vector<User>> FindUsersByLogin(const std::string& userLogin) noexcept;
        [[nodiscard]] static std::optional<std::vector<ChatSystem::Chat>> GetAvailableChatsForUser(const size_t userId) noexcept;
    };

} // !namespace UserData