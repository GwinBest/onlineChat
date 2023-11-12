#include "../src/gui/mainWindow.h"

#include <thread>

using namespace Network;

int main()
{
	Gui::MainWindow window;
	if (!window.Init())
		return 1;

	std::thread receiveThread(&Network::Client::Receive, &Network::Client::GetInstance());
	receiveThread.detach();

	window.Draw();
	window.Cleanup();

	return 0;
}