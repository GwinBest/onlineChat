#include "userRepository.h"

#include "client/client.h"
#include "messageBuffer/messageBuffer.h"
#include "networkCore/networkCore.h"

#ifdef GetUserName
#undef GetUserName
#endif   // GetUserName

namespace UserData
{
    Awaitable<std::optional<bool>>
        UserRepository::PushUserCredentialsToDatabaseAsync(const User user)
    {
        auto future = std::async(std::launch::async, [user]() -> std::optional<bool> {
            const NetworkCore::UserPacket request = {
                .actionType = NetworkCore::ActionType::kAddUserCredentialsToDatabase,
                .name = user.GetUserName(),
                .login = user.GetUserLogin(),
                .password = user.GetUserPassword()};

            const std::optional<std::reference_wrapper<ClientNetworking::Client>>
                instance = ClientNetworking::Client::GetInstance();
            if (!instance.has_value()) return std::nullopt;

            instance.value().get().SendUserCredentialsPacket(request);
            return instance.value().get().GetServerResponse<bool>();
        });

        return {std::move(future)};
    }

    Awaitable<std::optional<std::string>>
        UserRepository::GetUserNameFromDatabaseAsync(const std::string userLogin)
    {
        auto future = std::async(std::launch::async, [userLogin]() -> std::optional<std::string> {
            const NetworkCore::UserPacket request = {
                .actionType = NetworkCore::ActionType::kGetUserNameFromDatabase,
                .login = userLogin};

            const std::optional<std::reference_wrapper<ClientNetworking::Client>>
                instance = ClientNetworking::Client::GetInstance();
            if (!instance.has_value()) return std::nullopt;

            instance.value().get().SendUserCredentialsPacket(request);

            return instance.value().get().GetServerResponse<std::string>();
        });

        return {std::move(future)};
    }

    Awaitable<std::optional<size_t>>
        UserRepository::GetUserIdFromDatabaseAsync(const std::string userLogin)
    {
        auto future = std::async(std::launch::async, [userLogin]() -> std::optional<size_t> {
            const NetworkCore::UserPacket request = {
                .actionType = NetworkCore::ActionType::kGetUserIdFromDatabase,
                .login = userLogin};

            const std::optional<std::reference_wrapper<ClientNetworking::Client>>
                instance = ClientNetworking::Client::GetInstance();
            if (!instance.has_value()) return std::nullopt;

            instance.value().get().SendUserCredentialsPacket(request);

            return instance.value().get().GetServerResponse<size_t>();
        });

        return {std::move(future)};
    }

    Awaitable<std::optional<bool>> UserRepository::IsUserDataFromFileValidAsync(const User user)
    {
        auto future = std::async(std::launch::async, [user]() -> std::optional<bool> {
            const NetworkCore::UserPacket request = {
                .actionType = NetworkCore::ActionType::kCheckIsUserDataFromFileValid,
                .name = user.GetUserName(),
                .login = user.GetUserLogin(),
                .password = user.GetUserPassword(),
                .id = user.GetUserId()};

            const std::optional<std::reference_wrapper<ClientNetworking::Client>>
                instance = ClientNetworking::Client::GetInstance();
            if (!instance.has_value()) return std::nullopt;

            instance.value().get().SendUserCredentialsPacket(request);

            return instance.value().get().GetServerResponse<bool>();
        });

        return {std::move(future)};
    }

    Awaitable<std::optional<bool>> UserRepository::IsUserExistAsync(const User user)
    {
        auto future = std::async(std::launch::async, [user]() -> std::optional<bool> {
            const NetworkCore::UserPacket request = {
                .actionType = NetworkCore::ActionType::kCheckUserExistence,
                .login = user.GetUserLogin(),
                .password = user.GetUserPassword()};

            const std::optional<std::reference_wrapper<ClientNetworking::Client>>
                instance = ClientNetworking::Client::GetInstance();
            if (!instance.has_value()) return std::nullopt;

            instance.value().get().SendUserCredentialsPacket(request);

            return instance.value().get().GetServerResponse<bool>();
        });

        return {std::move(future)};
    }

    Awaitable<std::optional<std::vector<ChatSystem::ChatInfo>>>
        UserRepository::FindMatchingChatsAsync(const size_t currentUserId, std::string pattern)
    {
        auto future = std::async(
            std::launch::async,
            [currentUserId, pattern]() -> std::optional<std::vector<ChatSystem::ChatInfo>> {
                const NetworkCore::UserPacket request = {
                    .actionType = NetworkCore::ActionType::kFindMatchingChats,
                    .login = pattern,
                    .id = currentUserId,
                };

                const std::optional<std::reference_wrapper<ClientNetworking::Client>>
                    instance = ClientNetworking::Client::GetInstance();
                if (!instance.has_value()) return std::nullopt;

                instance.value().get().SendUserCredentialsPacket(request);

                return instance.value()
                    .get()
                    .GetServerResponse<std::vector<ChatSystem::ChatInfo>>();
            });

        return {std::move(future)};
    }

    Awaitable<std::optional<std::vector<ChatSystem::ChatInfo>>>
        UserRepository::GetAvailableChatsForUserAsync(const size_t userId)
    {
        auto future = std::async(
            std::launch::async,
            [userId]() -> std::optional<std::vector<ChatSystem::ChatInfo>> {
                const NetworkCore::UserPacket request = {
                    .actionType = NetworkCore::ActionType::kGetAvailableChatsForUser,
                    .id = userId,
                };

                const std::optional<std::reference_wrapper<ClientNetworking::Client>>
                    instance = ClientNetworking::Client::GetInstance();
                if (!instance.has_value()) return std::nullopt;

                instance.value().get().SendUserCredentialsPacket(request);

                return instance.value()
                    .get()
                    .GetServerResponse<std::vector<ChatSystem::ChatInfo>>();
            });

        return {std::move(future)};
    }

    Awaitable<std::optional<std::vector<MessageBuffer::MessageNode>>>
        UserRepository::GetAvailableChatMessagesAsync(const size_t userId, const size_t chatId)
    {
        auto future = std::async(
            std::launch::async,
            [userId, chatId]() -> std::optional<std::vector<MessageBuffer::MessageNode>> {
                const NetworkCore::ChatPacket request = {
                    .actionType = NetworkCore::ActionType::kReceiveAllMessagesForSelectedChat,
                    .chatUserId = userId,
                    .id = chatId};

                const std::optional<std::reference_wrapper<ClientNetworking::Client>>
                    instance = ClientNetworking::Client::GetInstance();
                if (!instance.has_value()) return std::nullopt;

                instance.value().get().SendChatInfoPacket(request);

                return instance.value()
                    .get()
                    .GetServerResponse<std::vector<MessageBuffer::MessageNode>>();
            });

        return {std::move(future)};
    }

    Awaitable<std::optional<size_t>>
        UserRepository::CreateNewPersonalChatAsync(const size_t senderUserId,
                                                   const std::string receiverUserName)
    {
        auto future = std::async(
            std::launch::async,
            [senderUserId, &receiverUserName]() -> std::optional<size_t> {
                const std::optional<std::reference_wrapper<ClientNetworking::Client>>
                    instance = ClientNetworking::Client::GetInstance();
                if (!instance.has_value()) return std::nullopt;

                instance.value().get().CreateNewPersonalChat(senderUserId,
                                                             receiverUserName.c_str());

                return instance.value().get().GetServerResponse<size_t>();
            });

        return {std::move(future)};
    }
}   // namespace UserData
