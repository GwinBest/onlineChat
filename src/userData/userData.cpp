#include "UserData.h"

namespace UserData
{
	void User::PushUserCredentialsToDatabase(const std::string& name, const std::string& login, const size_t password) noexcept
	{
		Network::UserRequest request =
		{
			.actionType = Network::ActionType::kAddUserCredentialsToDatabase,
			.name = name,
			.login = login,
			.password = password
		};

		Network::Client::GetInstance().SendUserCredentials(request);
	}

	std::string User::GetUserNameFromDatabase(const std::string& login, const size_t password) noexcept
	{
		Network::UserRequest request =
		{
			.actionType = Network::ActionType::kGetUSerNameFromDatabase,
			.name  = "",
			.login = login,
			.password = password
		};

		Network::Client::GetInstance().SendUserCredentials(request);
		std::string serverResponse = Network::Client::GetInstance().ReceiveServerResponse();

		return serverResponse;
	}

	bool User::IsUserExist(const std::string& login, const size_t password) noexcept
	{
		Network::UserRequest request =
		{
			.actionType = Network::ActionType::kCheckUserExistence,
			.name = "",
			.login = login,
			.password = password
		};

		Network::Client::GetInstance().SendUserCredentials(request);
		std::string serverResponse = Network::Client::GetInstance().ReceiveServerResponse();
		
		if (serverResponse == "Exist")
		{
			return true;
		}

		return false;
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

	void User::SetUserName(const std::string& name) noexcept
	{
		_userName = name;
	}

	void User::SetUserLogin(const std::string& login) noexcept
	{
		_userLogin = login;
	}

	void User::SetUserPassword(const size_t password) noexcept
	{
		_userPassword = password;
	}

} // !namespace UserData