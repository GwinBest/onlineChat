#include "userRepository.h"

#include "client/client.h"

#include "networkCore/networkCore.h"

#ifdef GetUserName
#undef GetUserName
#endif // GetUserName

namespace UserData
{
    bool UserRepository::PushUserCredentialsToDatabase(const User& user) noexcept
    {
        const NetworkCore::UserPacket request =
        {
            .actionType = NetworkCore::ActionType::kAddUserCredentialsToDatabase,
            .name = user.GetUserName(),
            .login = user.GetUserLogin(),
            .password = user.GetUserPassword()
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

    bool UserRepository::IsUserDataFromFileValid(const User& user) noexcept
    {
        const NetworkCore::UserPacket request =
        {
            .actionType = NetworkCore::ActionType::kCheckIsUserDataFromFileValid,
            .name = user.GetUserName(),
            .login = user.GetUserLogin(),
            .password = user.GetUserPassword(),
            .id = user.GetUserId()
        };

        ClientNetworking::Client::GetInstance().SendUserCredentialsPacket(request);

        return ClientNetworking::Client::GetInstance().GetServerResponse<bool>();
    }

    bool UserRepository::IsUserExist(const User& user) noexcept
    {
        const NetworkCore::UserPacket request =
        {
            .actionType = NetworkCore::ActionType::kCheckUserExistence,
            .login = user.GetUserLogin(),
            .password = user.GetUserPassword()
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

