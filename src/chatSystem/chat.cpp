#include "chat.h"

namespace ChatSystem
{
	void Chat::SetChatName(std::string_view chatName) noexcept
	{
		_chatName = chatName;
	}

	void Chat::SetChatId(const size_t chatId) noexcept
	{
		_chatId = chatId;
	}

	const std::string& Chat::GetChatName() const noexcept
	{
		return _chatName;
	}

	size_t Chat::GetChatId() const noexcept
	{
		return _chatId;
	}

} // !namespace ChatSystem