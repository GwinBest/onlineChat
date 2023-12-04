#pragma once

#include <vector>
#include <string>

#include "../client/client.h"
#include "../userData/userData.h"

namespace Chat
{
	class Chat final
	{
	public:
		static std::vector<Chat*> GetAvailableChatsForUser(const std::string& chatName) noexcept;

		void SetChatName(const std::string& chatName) noexcept;
		void SetChatId(const size_t chatId) noexcept;

		std::string GetChatName() const noexcept;
		size_t GetChatId() const noexcept;

	private:
		std::string _chatName;
		size_t _chatId;
	};

} // !namespace Chat