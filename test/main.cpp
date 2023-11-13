#include "../src/gui/mainWindow.h"

using namespace Network;

int main()
{
	Gui::MainWindow window;
	if (!window.Init())
		return 1;

	window.Draw();
	window.Cleanup();

	return 0;
}