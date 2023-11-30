#pragma once

#include "../client/client.h"
#include "userCredentialsFile.h"

// defined in Windows.h
#ifdef GetUserName
#undef GetUserName
#endif

namespace UserData
{
	class User final
	{
	public:
		static void PushUserCredentialsToDatabase(const std::string& name, const std::string& login, const std::string& password) noexcept;

		static std::string GetUserNameFromDatabase(const std::string& login, const std::string& password) noexcept;

		static bool IsUserExist(const std::string& login, const std::string& password) noexcept;

		std::string GetUserName() const noexcept;
		std::string GetUserLogin() const noexcept;
		std::string GetUserPassword() const noexcept;

		void SetUserName(std::string& name) noexcept;
		void SetUserLogin(std::string& login) noexcept;
		void SetUserPassword(std::string& password) noexcept;

	private:
		std::string _userName;
		std::string _userLogin;
		std::string _userPassword;
	};

} // !namespace UserData