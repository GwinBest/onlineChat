#pragma once

#include "../chatSystem/chatSystem.h"
#include "../client/client.h"
#include "../common/common.h"
#include "../messageBuffer/messageBuffer.h"
#include "../userData/userData.h"
#include "iImGuiWindow.h"

#include <memory>

extern UserData::User currentUser;
extern std::list<MessageBuffer::MessageNode> MessageBuffer::messageBuffer;

namespace Gui
{
	class ChatWindow final : public IImGuiWindow
	{
	public:
		ChatWindow() noexcept = default;

	private:
		void DrawGui() noexcept override;

	private:
		static inline char _inputBuffer[Common::maxInputBufferSize] = "";
	};

} // !namespace Gui