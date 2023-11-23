#ifndef _OLINECHAT_GUI_CHAT_WINDOW_H
#define _OLINECHAT_GUI_CHAT_WINDOW_H

#include "mainWindow.h"
#include "../client/client.h"
#include "../messageBuffer/messageBuffer.h"

namespace Gui
{
	class ChatWindow final : public MainWindow 
	{
	public:
		ChatWindow() noexcept = default;
		~ChatWindow();

	private:
		void GenerateControls() noexcept override;

	private:
		std::string _inputBuffer;
	};

} // !namespace Gui

#endif // !_OLINECHAT_GUI_CHAT_WINDOW_H
