#pragma once

#include <vector>

#include "../client/client.h"

// defined in Windows.h
#ifdef GetUserName
#undef GetUserName
#endif

namespace UserData
{
	class User final
	{
	public:
		static void PushUserCredentialsToDatabase(const std::string& userName, const std::string& userLogin, const size_t userPassword) noexcept;

		static std::string GetUserNameFromDatabase(const std::string& userLogin, const size_t userPassword) noexcept;

		static bool IsUserExist(const std::string& userLogin, const size_t userPassword) noexcept;

		static std::vector<User> FindUsersByLogin(const std::string& userLogin) noexcept;

		std::string GetUserName() const noexcept;
		std::string GetUserLogin() const noexcept;
		size_t GetUserPassword() const noexcept;

		void SetUserName(const std::string& userName) noexcept;
		void SetUserLogin(const std::string& userLogin) noexcept;
		void SetUserPassword(const size_t userPassword) noexcept;

	private:
		std::string _userName;
		std::string _userLogin;
		size_t _userPassword;
	};

} // !namespace UserData
