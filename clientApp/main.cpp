#include "../src/gui/glfwWindow.h"
#include "../src/gui/chatWindow.h"

int main()
{
	Gui::GlfwWindow window;
	window.Init();
	window.PushWindow(std::make_unique<Gui::ChatWindow>());
	window.Draw();
	window.Cleanup();

	return 0;
}