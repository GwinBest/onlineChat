#include "chatRepository.h"

#include "../client/client.h" 

namespace ChatSystem
{
	std::vector<Chat> ChatRepository::GetAvailableChatsFromDatabase(const std::string& chatUserLogin) noexcept
	{
		const ClientNetworking::ChatPacket request =
		{
			.actionType = NetworkCore::ActionType::kGetAvailableChats,
			.chatUserLogin = chatUserLogin,
		};

		ClientNetworking::Client::GetInstance().SendChatInfoPacket(request);

		return ClientNetworking::Client::GetInstance().GetServerResponse<std::vector<Chat>>();
	}

} // !namespace ChatSystem