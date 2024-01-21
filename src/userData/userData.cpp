#include "UserData.h"

namespace UserData
{
	void User::PushUserCredentialsToDatabase(const std::string& userName, const std::string& userLogin, const size_t userPassword) noexcept
	{
		const ClientNetworking::UserPacket request =
		{
			.actionType = NetworkCore::ActionType::kAddUserCredentialsToDatabase,
			.name = userName,
			.login = userLogin,
			.password = userPassword
		};

		ClientNetworking::Client::GetInstance().SendUserCredentialsPacket(request);
	}

	std::string User::GetUserNameFromDatabase(const std::string& userLogin, const size_t userPassword) noexcept
	{
		//TODO: send only login
		const ClientNetworking::UserPacket request =
		{
			.actionType = NetworkCore::ActionType::kGetUserNameFromDatabase,
			.name = "",
			.login = userLogin,
			.password = userPassword
		};

		ClientNetworking::Client::GetInstance().SendUserCredentialsPacket(request);

		return ClientNetworking::Client::GetInstance().GetServerResponse<std::string>();
	}

	bool User::IsUserExist(const std::string& userLogin, const size_t userPassword) noexcept
	{
		//TODO: send only login
		const ClientNetworking::UserPacket request =
		{
			.actionType = NetworkCore::ActionType::kCheckUserExistence,
			.name = "",
			.login = userLogin,
			.password = userPassword
		};

		ClientNetworking::Client::GetInstance().SendUserCredentialsPacket(request);

		return ClientNetworking::Client::GetInstance().GetServerResponse<bool>();
	}

	std::vector<User> User::FindUsersByLogin(const std::string& userLogin) noexcept
	{
		const ClientNetworking::UserPacket request =
		{
			.actionType = NetworkCore::ActionType::kFindUsersByLogin,
			.name = "",
			.login = userLogin,
			.password = 0
		};

		ClientNetworking::Client::GetInstance().SendUserCredentialsPacket(request);

		return ClientNetworking::Client::GetInstance().GetServerResponse<std::vector<User>>();
	}

	std::string User::GetUserName() const noexcept
	{
		return _userName;
	}

	std::string User::GetUserLogin() const noexcept
	{
		return _userLogin;
	}

	size_t User::GetUserPassword() const noexcept
	{
		return _userPassword;
	}

	void User::SetUserName(const std::string& userName) noexcept
	{
		_userName = userName;
	}

	void User::SetUserLogin(const std::string& userLogin) noexcept
	{
		_userLogin = userLogin;
	}

	void User::SetUserPassword(const size_t userPassword) noexcept
	{
		_userPassword = userPassword;
	}

} // !namespace UserData