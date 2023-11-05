#include "../src/gui/gui.h"

using namespace Network;

#include <thread>

int main()
{
	Gui::MainWindow window;
	window.Init();
	window.Draw();
	window.Cleanup();
	return 0;
}