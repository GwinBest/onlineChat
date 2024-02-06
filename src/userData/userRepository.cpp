#include "userRepository.h"

#include "../client/client.h"
#include "../networkCore/networkCore.h"

namespace UserData
{
	bool UserRepository::PushUserCredentialsToDatabase(const std::string& userName, const std::string& userLogin, const size_t userPassword) noexcept
	{
		const NetworkCore::UserPacket request =
		{
			.actionType = NetworkCore::ActionType::kAddUserCredentialsToDatabase,
			.name = userName,
			.login = userLogin,
			.password = userPassword
		};

		ClientNetworking::Client::GetInstance().SendUserCredentialsPacket(request);

		return ClientNetworking::Client::GetInstance().GetServerResponse<bool>();
	}

	std::string UserRepository::GetUserNameFromDatabase(const std::string& userLogin) noexcept
	{
		const NetworkCore::UserPacket request =
		{
			.actionType = NetworkCore::ActionType::kGetUserNameFromDatabase,
			.login = userLogin
		};

		ClientNetworking::Client::GetInstance().SendUserCredentialsPacket(request);

		return ClientNetworking::Client::GetInstance().GetServerResponse<std::string>();
	}

	size_t UserRepository::GetUserIdFromDatabase(const std::string& userLogin) noexcept
	{
		const NetworkCore::UserPacket request =
		{
			.actionType = NetworkCore::ActionType::kGetUserIdFromDatabase,
			.login = userLogin
		};

		ClientNetworking::Client::GetInstance().SendUserCredentialsPacket(request);

		return ClientNetworking::Client::GetInstance().GetServerResponse<size_t>();
	}

	bool UserRepository::IsUserDataFromFileValid(const std::string& userName, const std::string& userLogin, const size_t userPassword, const size_t userId) noexcept
	{
		const NetworkCore::UserPacket request =
		{
			.actionType = NetworkCore::ActionType::kCheckIsUserDataFromFileValid,
			.name = userName,
			.login = userLogin,
			.password = userPassword,
			.id = userId
		};

		ClientNetworking::Client::GetInstance().SendUserCredentialsPacket(request);

		return ClientNetworking::Client::GetInstance().GetServerResponse<bool>();
	}

	bool UserRepository::IsUserExist(const std::string& userLogin, const size_t userPassword) noexcept
	{
		const NetworkCore::UserPacket request =
		{
			.actionType = NetworkCore::ActionType::kCheckUserExistence,
			.login = userLogin,
			.password = userPassword,
		};

		ClientNetworking::Client::GetInstance().SendUserCredentialsPacket(request);

		return ClientNetworking::Client::GetInstance().GetServerResponse<bool>();
	}

	std::vector<User> UserRepository::FindUsersByLogin(const std::string& userLogin) noexcept
	{
		const NetworkCore::UserPacket request =
		{
			.actionType = NetworkCore::ActionType::kFindUsersByLogin,
			.login = userLogin,
		};

		ClientNetworking::Client::GetInstance().SendUserCredentialsPacket(request);

		return ClientNetworking::Client::GetInstance().GetServerResponse<std::vector<User>>();
	}

	std::vector<ChatSystem::Chat> UserRepository::GetAvailableChatsForUser(const size_t userId) noexcept
	{
		const NetworkCore::UserPacket request =
		{
			.actionType = NetworkCore::ActionType::kGetAvailableChatsForUser,
			.id = userId,
		};

		ClientNetworking::Client::GetInstance().SendUserCredentialsPacket(request);

		return ClientNetworking::Client::GetInstance().GetServerResponse<std::vector<ChatSystem::Chat>>();
	}

} // !namespace UserData

