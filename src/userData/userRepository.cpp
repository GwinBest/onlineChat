#include "userRepository.h"

#include "client/client.h"

#include "networkCore/networkCore.h"

#ifdef GetUserName
#undef GetUserName
#endif // GetUserName

namespace UserData
{
    std::optional<bool> UserRepository::PushUserCredentialsToDatabase(const User& user) noexcept
    {
        const NetworkCore::UserPacket request =
        {
            .actionType = NetworkCore::ActionType::kAddUserCredentialsToDatabase,
            .name = user.GetUserName(),
            .login = user.GetUserLogin(),
            .password = user.GetUserPassword()
        };

        std::optional<ClientNetworking::Client*> instance = ClientNetworking::Client::GetInstance();
        if (!instance.has_value()) return std::nullopt;

        instance.value()->SendUserCredentialsPacket(request);

        return instance.value()->GetServerResponse<bool>();
    }

    std::optional<std::string> UserRepository::GetUserNameFromDatabase(const std::string& userLogin) noexcept
    {
        const NetworkCore::UserPacket request =
        {
            .actionType = NetworkCore::ActionType::kGetUserNameFromDatabase,
            .login = userLogin
        };

        std::optional<ClientNetworking::Client*> instance = ClientNetworking::Client::GetInstance();
        if (!instance.has_value()) return std::nullopt;

        instance.value()->SendUserCredentialsPacket(request);

        return instance.value()->GetServerResponse<std::string>();
    }

    std::optional<size_t> UserRepository::GetUserIdFromDatabase(const std::string& userLogin) noexcept
    {
        const NetworkCore::UserPacket request =
        {
            .actionType = NetworkCore::ActionType::kGetUserIdFromDatabase,
            .login = userLogin
        };

        std::optional<ClientNetworking::Client*> instance = ClientNetworking::Client::GetInstance();
        if (!instance.has_value()) return std::nullopt;

        instance.value()->SendUserCredentialsPacket(request);

        return instance.value()->GetServerResponse<size_t>();
    }

    std::optional<bool> UserRepository::IsUserDataFromFileValid(const User& user) noexcept
    {
        const NetworkCore::UserPacket request =
        {
            .actionType = NetworkCore::ActionType::kCheckIsUserDataFromFileValid,
            .name = user.GetUserName(),
            .login = user.GetUserLogin(),
            .password = user.GetUserPassword(),
            .id = user.GetUserId()
        };

        std::optional<ClientNetworking::Client*> instance = ClientNetworking::Client::GetInstance();
        if (!instance.has_value()) return std::nullopt;

        instance.value()->SendUserCredentialsPacket(request);

        return instance.value()->GetServerResponse<bool>();
    }

    std::optional<bool> UserRepository::IsUserExist(const User& user) noexcept
    {
        const NetworkCore::UserPacket request =
        {
            .actionType = NetworkCore::ActionType::kCheckUserExistence,
            .login = user.GetUserLogin(),
            .password = user.GetUserPassword()
        };

        std::optional<ClientNetworking::Client*> instance = ClientNetworking::Client::GetInstance();
        if (!instance.has_value()) return std::nullopt;

        instance.value()->SendUserCredentialsPacket(request);

        return instance.value()->GetServerResponse<bool>();
    }

    std::optional<std::vector<User>> UserRepository::FindUsersByLogin(const std::string& userLogin) noexcept
    {
        const NetworkCore::UserPacket request =
        {
            .actionType = NetworkCore::ActionType::kFindUsersByLogin,
            .login = userLogin,
        };

        std::optional<ClientNetworking::Client*> instance = ClientNetworking::Client::GetInstance();
        if (!instance.has_value()) return std::nullopt;

        instance.value()->SendUserCredentialsPacket(request);

        return instance.value()->GetServerResponse<std::vector<User>>();
    }

    std::optional<std::vector<ChatSystem::Chat>> UserRepository::GetAvailableChatsForUser(const size_t userId) noexcept
    {
        const NetworkCore::UserPacket request =
        {
            .actionType = NetworkCore::ActionType::kGetAvailableChatsForUser,
            .id = userId,
        };

        std::optional<ClientNetworking::Client*> instance = ClientNetworking::Client::GetInstance();
        if (!instance.has_value()) return std::nullopt;

        instance.value()->SendUserCredentialsPacket(request);

        return instance.value()->GetServerResponse<std::vector<ChatSystem::Chat>>();
    }

} // !namespace UserData

