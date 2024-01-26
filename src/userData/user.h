#pragma once

#include <string>
#include <vector>

namespace UserData
{
	class User final
	{
	public:
		const std::string& GetUserName() const noexcept;
		const std::string& GetUserLogin() const noexcept;
		size_t GetUserPassword() const noexcept;

		void SetUserName(std::string_view userName) noexcept;
		void SetUserLogin(std::string_view userLogin) noexcept;
		void SetUserPassword(const size_t userPassword) noexcept;

	private:
		std::string _userName;
		std::string _userLogin;
		size_t _userPassword;
	};

} // !namespace UserData
