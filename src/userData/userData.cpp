#include "UserData.h"

namespace UserData
{
	void User::PushUserCredentialsToDatabase(const std::string& name, const std::string& login, const std::string& password) noexcept
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

	bool User::IsUserExist(const std::string& login, const std::string& password) noexcept
	{
		Network::UserRequest request =
		{
			.actionType = Network::ActionType::kCheckUserExistence,
			.login = login,
			.password = password
		};

		Network::Client::GetInstance().SendUserCredentials(request);
		const std::string serverResponse = Network::Client::GetInstance().ReceiveServerResponse();

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

	std::string User::GetUserPassword() const noexcept
	{
		return _userPassword;
	}

} // !namespace UserData