#pragma once

#include <string>
#include <vector>

#include "user.h"
#include "../chatSystem/chat.h"
#include "../networkCore/networkCore.h"

namespace UserData
{
	class UserRepository final 
	{
	public:
		UserRepository() = delete;
		UserRepository(const UserRepository&) = delete;
		UserRepository& operator=(const UserRepository&) = delete;
		UserRepository(const UserRepository&&) = delete;
		UserRepository& operator=(const UserRepository&&) = delete;
		~UserRepository() = delete;

		[[nodiscard]] static bool PushUserCredentialsToDatabase(const std::string& userName, const std::string& userLogin, const size_t userPassword) noexcept;

		static std::string GetUserNameFromDatabase(const std::string& userLogin) noexcept;

		static size_t GetUserIdFromDatabase(const std::string& userLogin) noexcept;

		[[nodiscard]] static bool IsUserExist(const std::string& userLogin, const size_t userPassword) noexcept;

		static std::vector<User> FindUsersByLogin(const std::string& userLogin) noexcept;

		static std::vector<ChatSystem::Chat> GetAvailableChatsForUser(const size_t userId) noexcept;
	};

} // !namespace UserData