#pragma once

#include "../client/client.h"
#include "userCredentialsFile.h"

// defined in Windows.h
#ifdef GetUserName
#undef GetUserName
#endif

namespace UserData
{
	class User
	{
	public:
		static void PushUserCredentialsToDatabase(const std::string& name, const std::string& login, const std::string& password) noexcept;

		static bool IsUserExist(const std::string& login, const std::string& password) noexcept;

		std::string GetUserName() const noexcept;
		std::string GetUserLogin() const noexcept;
		std::string GetUserPassword() const noexcept;

	private:
		std::string _userName;
		std::string _userLogin;
		std::string _userPassword;

		friend UserCredentialsFile;
	};

} // !namespace UserData