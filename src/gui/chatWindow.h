#pragma once

#include "iImGuiWindow.h"
#include "../client/client.h"
#include "../messageBuffer/messageBuffer.h"
#include "../userData/userData.h"

namespace Gui
{
	class ChatWindow final : public IImGuiWindow
	{
	public:
		ChatWindow() noexcept = default;

	private:
		void DrawGui() noexcept override;

	private:
		static inline std::string _inputBuffer;
	};

} // !namespace Gui