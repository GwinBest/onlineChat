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
		static void PushUserCredentialsToDatabase(const std::string& name, const std::string& login, const size_t password) noexcept;

		static std::string GetUserNameFromDatabase(const std::string& login, const size_t password) noexcept;

		static bool IsUserExist(const std::string& login, const size_t password) noexcept;

		static std::vector<UserData::User*> FindUsersByLogin(const std::string& userLogin) noexcept;

		std::string GetUserName() const noexcept;
		std::string GetUserLogin() const noexcept;
		size_t GetUserPassword() const noexcept;

		void SetUserName(const std::string& name) noexcept;
		void SetUserLogin(const std::string& login) noexcept;
		void SetUserPassword(const size_t password) noexcept;

	private:
		std::string _userName;
		std::string _userLogin;
		size_t _userPassword;
	};

} // !namespace UserData
