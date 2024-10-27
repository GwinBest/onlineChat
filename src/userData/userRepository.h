#pragma once

#include <string>
#include <vector>

#include "user.h"

#include "chatSystem/chat.h"

namespace UserData
{
    class UserRepository final
    {
    public:
        [[nodiscard]] static bool PushUserCredentialsToDatabase(const User& user) noexcept;
        [[nodiscard]] static std::string GetUserNameFromDatabase(const std::string& userLogin) noexcept;
        [[nodiscard]] static size_t GetUserIdFromDatabase(const std::string& userLogin) noexcept;
        [[nodiscard]] static bool IsUserDataFromFileValid(const User& user) noexcept;
        [[nodiscard]] static bool IsUserExist(const User& user) noexcept;
        [[nodiscard]] static std::vector<User> FindUsersByLogin(const std::string& userLogin) noexcept;
        [[nodiscard]] static std::vector<ChatSystem::Chat> GetAvailableChatsForUser(const size_t userId) noexcept;
    };

} // !namespace UserData