#pragma once

#include <string>
#include <vector>

#include "../client/client.h" 

namespace ChatSystem
{
	class Chat final
	{
	public:
		static std::vector<Chat> GetAvailableChatsFromDatabase(const std::string& chatUserLogin) noexcept;

		void SetChatName(const std::string& chatName) noexcept;
		void SetChatId(const size_t chatId) noexcept;

		std::string GetChatName() const noexcept;
		size_t GetChatId() const noexcept;

	private:
		std::string _chatName;
		size_t _chatId;
	};

} // !namespace ChatSystem