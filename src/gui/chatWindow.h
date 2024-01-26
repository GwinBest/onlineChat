#pragma once

#include "../common/common.h"
#include "iImGuiWindow.h"

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