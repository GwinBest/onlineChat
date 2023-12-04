#include "chat.h"

namespace Chat
{
	std::vector< std::shared_ptr<Chat>> Chat::GetAvailableChatsForUser(const std::string& currentUserLogin) noexcept
	{
		Network::ChatPacket request =
		{
			.actionType = Network::ActionType::kGetAvailableChatsForUser,
			.currentUserLogin = currentUserLogin,
		};

		Network::Client::GetInstance().SendChatInfoPacket(request);
		std::vector< std::shared_ptr<Chat>> serverResponse = Network::Client::GetInstance().GetServerResponse<std::vector<std::shared_ptr<Chat>>>();

		return serverResponse;
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

} // !namespace Chat