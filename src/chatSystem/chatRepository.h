#pragma once

#include <string>
#include <vector>

#include "chat.h"

namespace ChatSystem
{
	class ChatRepository final
	{
	public:
		ChatRepository() = delete;
		ChatRepository(const ChatRepository&) = delete;
		ChatRepository& operator=(const ChatRepository&) = delete;
		ChatRepository(const ChatRepository&&) = delete;
		ChatRepository& operator=(const ChatRepository&&) = delete;
		~ChatRepository() = delete;

		static std::vector<Chat> GetAvailableChatsFromDatabase(const std::string& chatUserLogin) noexcept;
	};

} // !namespace ChatSystem