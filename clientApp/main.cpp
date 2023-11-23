#include "../src/gui/chatWindow.h"

int main()
{
	Gui::MainWindow* window = new Gui::ChatWindow;
	if (!window->Init())
		return 1;

	window->Draw();
	window->Cleanup();

	delete window;

	return 0;
}