#pragma once

#include <string>
#include <vector>

#include "user.h"

namespace UserData
{
	class User final
	{
	public:
		const std::string& GetUserName() const noexcept;
		const std::string& GetUserLogin() const noexcept;
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
