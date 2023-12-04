#pragma once

#include <memory>
#include <string>
#include <vector>

#include "../client/client.h"

namespace Chat
{
	class Chat final
	{
	public:
		static std::vector< std::shared_ptr<Chat>> GetAvailableChatsForUser(const std::string& currentUserLogin) noexcept;

		void SetChatName(const std::string& chatName) noexcept;
		void SetChatId(const size_t chatId) noexcept;

		std::string GetChatName() const noexcept;
		size_t GetChatId() const noexcept;

	private:
		std::string _chatName;
		size_t _chatId;
	};

} // !namespace Chat