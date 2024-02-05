#pragma once

#include <string>
#include <vector>

#ifdef GetUserName
#undef GetUserName
#endif // GetUserName

namespace UserData
{
	class User final
	{
	public:
		const std::string& GetUserName() const noexcept;
		const std::string& GetUserLogin() const noexcept;
		const size_t GetUserPassword() const noexcept;
		const size_t GetUserId() const noexcept;

		void SetUserName(std::string_view userName) noexcept;
		void SetUserLogin(std::string_view userLogin) noexcept;
		void SetUserPassword(const size_t userPassword) noexcept;
		void SetUserId(const size_t userId) noexcept;


	private:
		size_t _userId;
		std::string _userName;
		std::string _userLogin;
		size_t _userPassword;
	};

} // !namespace UserData
