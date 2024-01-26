#pragma once

#include <string>
#include <vector>

#include "user.h"

namespace UserData
{
	class UserRepository final 
	{
	public:
		static void PushUserCredentialsToDatabase(const std::string& userName, const std::string& userLogin, const size_t userPassword) noexcept;

		static std::string GetUserNameFromDatabase(const std::string& userLogin, const size_t userPassword) noexcept;

		static bool IsUserExist(const std::string& userLogin, const size_t userPassword) noexcept;

		static std::vector<User> FindUsersByLogin(const std::string& userLogin) noexcept;
	};

} // !namespace UserData