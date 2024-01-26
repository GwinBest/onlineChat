#pragma once

#include <string>

namespace ChatSystem
{
	class Chat final
	{
	public:
		void SetChatName(std::string_view chatName) noexcept;
		void SetChatId(const size_t chatId) noexcept;

		const std::string& GetChatName() const noexcept;
		size_t GetChatId() const noexcept;

	private:
		std::string _chatName;
		size_t _chatId;
	};

} // !namespace ChatSystem