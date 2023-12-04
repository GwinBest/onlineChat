#pragma once

#include "iImGuiWindow.h"
#include "../client/client.h"
#include "../messageBuffer/messageBuffer.h"
#include "../userData/userData.h"
#include "../chat/chat.h"

extern UserData::User currentUser;

namespace Gui
{
	class ChatWindow final : public IImGuiWindow
	{
	public:
		ChatWindow() noexcept = default;

	private:
		void DrawGui() noexcept override;

		static size_t GetChatId() noexcept;
	private:
		static inline std::string _inputBuffer;
	};

} // !namespace Gui