#include "userRepository.h"

#include "client/client.h"

#include "messageBuffer/messageBuffer.h"

#include "networkCore/networkCore.h"

#ifdef GetUserName
#undef GetUserName
#endif // GetUserName

namespace UserData
{
    std::optional<bool> UserRepository::PushUserCredentialsToDatabase(const User& user)
    {
        const NetworkCore::UserPacket request =
        {
            .actionType = NetworkCore::ActionType::kAddUserCredentialsToDatabase,
            .name = user.GetUserName(),
            .login = user.GetUserLogin(),
            .password = user.GetUserPassword()
        };

        const std::optional<std::reference_wrapper<ClientNetworking::Client>> instance = ClientNetworking::Client::GetInstance();
        if (!instance.has_value()) return std::nullopt;

        instance.value().get().SendUserCredentialsPacket(request);

        return instance.value().get().GetServerResponse<bool>();
    }

    std::optional<std::string> UserRepository::GetUserNameFromDatabase(const std::string& userLogin)
    {
        const NetworkCore::UserPacket request =
        {
            .actionType = NetworkCore::ActionType::kGetUserNameFromDatabase,
            .login = userLogin
        };

        const std::optional<std::reference_wrapper<ClientNetworking::Client>> instance = ClientNetworking::Client::GetInstance();
        if (!instance.has_value()) return std::nullopt;

        instance.value().get().SendUserCredentialsPacket(request);

        return instance.value().get().GetServerResponse<std::string>();
    }

    std::optional<size_t> UserRepository::GetUserIdFromDatabase(const std::string& userLogin)
    {
        const NetworkCore::UserPacket request =
        {
            .actionType = NetworkCore::ActionType::kGetUserIdFromDatabase,
            .login = userLogin
        };

        const std::optional<std::reference_wrapper<ClientNetworking::Client>> instance = ClientNetworking::Client::GetInstance();
        if (!instance.has_value()) return std::nullopt;

        instance.value().get().SendUserCredentialsPacket(request);

        return instance.value().get().GetServerResponse<size_t>();
    }

    std::optional<bool> UserRepository::IsUserDataFromFileValid(const User& user)
    {
        const NetworkCore::UserPacket request =
        {
            .actionType = NetworkCore::ActionType::kCheckIsUserDataFromFileValid,
            .name = user.GetUserName(),
            .login = user.GetUserLogin(),
            .password = user.GetUserPassword(),
            .id = user.GetUserId()
        };

        const std::optional<std::reference_wrapper<ClientNetworking::Client>> instance = ClientNetworking::Client::GetInstance();
        if (!instance.has_value()) return std::nullopt;

        instance.value().get().SendUserCredentialsPacket(request);

        return instance.value().get().GetServerResponse<bool>();
    }

    std::optional<bool> UserRepository::IsUserExist(const User& user)
    {
        const NetworkCore::UserPacket request =
        {
            .actionType = NetworkCore::ActionType::kCheckUserExistence,
            .login = user.GetUserLogin(),
            .password = user.GetUserPassword()
        };

        const std::optional<std::reference_wrapper<ClientNetworking::Client>> instance = ClientNetworking::Client::GetInstance();
        if (!instance.has_value()) return std::nullopt;

        instance.value().get().SendUserCredentialsPacket(request);

        return instance.value().get().GetServerResponse<bool>();
    }

    std::optional<std::vector<ChatSystem::ChatInfo>> UserRepository::FindMatchingChats(const size_t currentUserId, const std::string& pattern)
    {
        const NetworkCore::UserPacket request =
        {
            .actionType = NetworkCore::ActionType::kFindMatchingChats,
            .login = pattern,
            .id = currentUserId,
        };

        const std::optional<std::reference_wrapper<ClientNetworking::Client>> instance = ClientNetworking::Client::GetInstance();
        if (!instance.has_value()) return std::nullopt;

        instance.value().get().SendUserCredentialsPacket(request);

        return instance.value().get().GetServerResponse<std::vector<ChatSystem::ChatInfo>>();
    }

    std::optional<std::vector<ChatSystem::ChatInfo>> UserRepository::GetAvailableChatsForUser(const size_t userId)
    {
        const NetworkCore::UserPacket request =
        {
            .actionType = NetworkCore::ActionType::kGetAvailableChatsForUser,
            .id = userId,
        };

        const std::optional<std::reference_wrapper<ClientNetworking::Client>> instance = ClientNetworking::Client::GetInstance();
        if (!instance.has_value()) return std::nullopt;

        instance.value().get().SendUserCredentialsPacket(request);

        return instance.value().get().GetServerResponse<std::vector<ChatSystem::ChatInfo>>();
    }

    std::optional<std::vector<MessageBuffer::MessageNode>> UserRepository::GetAvailableChatMessages(const size_t userId, const size_t chatId)
    {
        const NetworkCore::ChatPacket request =
        {
            .actionType = NetworkCore::ActionType::kReceiveAllMessagesForSelectedChat,
            .chatUserId = userId,
            .id = chatId
        };

        const std::optional<std::reference_wrapper<ClientNetworking::Client>> instance = ClientNetworking::Client::GetInstance();
        if (!instance.has_value()) return std::nullopt;

        instance.value().get().SendChatInfoPacket(request);

        return instance.value().get().GetServerResponse<std::vector<MessageBuffer::MessageNode>>();
    }
} // !namespace UserData

