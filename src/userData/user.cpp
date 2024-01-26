#include "user.h"

namespace UserData
{
	const std::string& User::GetUserName() const noexcept
	{
		return _userName;
	}

	const std::string& User::GetUserLogin() const noexcept
	{
		return _userLogin;
	}

	size_t User::GetUserPassword() const noexcept
	{
		return _userPassword;
	}

	void User::SetUserName(std::string_view userName) noexcept
	{
		_userName = userName;
	}

	void User::SetUserLogin(std::string_view userLogin) noexcept
	{
		_userLogin = userLogin;
	}

	void User::SetUserPassword(const size_t userPassword) noexcept
	{
		_userPassword = userPassword;
	}

} // !namespace UserData