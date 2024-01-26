#include "userRepository.h"

#include "../client/client.h"

namespace UserData
{
	void UserRepository::PushUserCredentialsToDatabase(const std::string& userName, const std::string& userLogin, const size_t userPassword) noexcept
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

	std::string UserRepository::GetUserNameFromDatabase(const std::string& userLogin, const size_t userPassword) noexcept
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

	bool UserRepository::IsUserExist(const std::string& userLogin, const size_t userPassword) noexcept
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

	std::vector<User> UserRepository::FindUsersByLogin(const std::string& userLogin) noexcept
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

} // !namespace UserData

