#include "ñhatSystem.h"

namespace ChatSystem
{
	std::vector<Chat> Chat::GetAvailableChats(const std::string& chatUserLogin) noexcept
	{
		//TODO: add error handle

		ClientNetworking::ChatPacket request =
		{
			.actionType = NetworkCore::ActionType::kGetAvailableChats,
			.chatUserLogin = chatUserLogin,
		};

		ClientNetworking::Client::GetInstance().SendChatInfoPacket(request);

		return ClientNetworking::Client::GetInstance().GetServerResponse<std::vector<Chat>>();
	}

	void Chat::SetChatName(const std::string& chatName) noexcept
	{
		_chatName = chatName;
	}

	void Chat::SetChatId(const size_t chatId) noexcept
	{
		_chatId = chatId;
	}

	std::string Chat::GetChatName() const noexcept
	{
		return _chatName;
	}

	size_t Chat::GetChatId() const noexcept
	{
		return _chatId;
	}

} // !namespace ChatSystem