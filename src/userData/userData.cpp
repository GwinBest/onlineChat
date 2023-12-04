#include "UserData.h"

namespace UserData
{
	void User::PushUserCredentialsToDatabase(const std::string& userName, const std::string& userLogin, const size_t userPassword) noexcept
	{
		Network::UserPacket request =
		{
			.actionType = Network::ActionType::kAddUserCredentialsToDatabase,
			.name = userName,
			.login = userLogin,
			.password = userPassword
		};

		Network::Client::GetInstance().SendUserCredentialsPacket(request);
	}

	std::string User::GetUserNameFromDatabase(const std::string& userLogin, const size_t password) noexcept
	{
		//TODO: send only login
		Network::UserPacket request =
		{
			.actionType = Network::ActionType::kGetUserNameFromDatabase,
			.name = "",
			.login = userLogin,
			.password = password
		};

		Network::Client::GetInstance().SendUserCredentialsPacket(request);
		std::string serverResponse = Network::Client::GetInstance().GetServerResponse<std::string>();

		return serverResponse;
	}

	bool User::IsUserExist(const std::string& userLogin, const size_t password) noexcept
	{
		//TODO: send only login
		Network::UserPacket request =
		{
			.actionType = Network::ActionType::kCheckUserExistence,
			.name = "",
			.login = userLogin,
			.password = password
		};

		Network::Client::GetInstance().SendUserCredentialsPacket(request);
		std::string serverResponse = Network::Client::GetInstance().GetServerResponse<std::string>();

		if (serverResponse == "Exist")	//TODO return value bool instead of string
		{
			return true;
		}

		return false;
	}

	std::vector< std::shared_ptr<User>> User::FindUsersByLogin(const std::string& userLogin) noexcept
	{
		Network::UserPacket request =
		{
			.actionType = Network::ActionType::kFindUsersByLogin,
			.login = userLogin
		};

		Network::Client::GetInstance().SendUserCredentialsPacket(request);
		std::vector< std::shared_ptr<User>> serverResponse = Network::Client::GetInstance().GetServerResponse<std::vector<std::shared_ptr<User>>>();

		return serverResponse;
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