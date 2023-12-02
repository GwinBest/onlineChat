#pragma once

#include "iImGuiWindow.h"
#include "../client/client.h"
#include "../messageBuffer/messageBuffer.h"

namespace Gui
{
	class ChatWindow final : public IImGuiWindow
	{
	public:
		ChatWindow() noexcept = default;

	private:
		void DrawGui() noexcept override;

	private:
		std::string _inputBuffer;
	};

} // !namespace Gui