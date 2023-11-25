#include "../src/gui/glfwWindow.h"
#include "../src/gui/chatWindow.h"
#include "../src/gui/loginWindow.h"
#include "../src/gui/signUpWindow.h"

int main()
{
	Gui::GlfwWindow window;
	window.Init();

	while (!glfwWindowShouldClose(window.GetGlfwWindow()))
	{
		if (!Gui::LoginWindow::IsLoginButtonPressed() && !Gui::LoginWindow::IsSignUpLabelPressed())
		{
			window.PushWindow(std::make_unique<Gui::LoginWindow>());

			window.Draw();

			window.PopWindow();
		}

		if (Gui::LoginWindow::IsSignUpLabelPressed() && !Gui::SignUpWindow::IsSignUpButtonPressed())
		{
			window.PushWindow(std::make_unique<Gui::SignUpWindow>());

			window.Draw();

			window.PopWindow();
		}

		if (Gui::SignUpWindow::IsSignUpButtonPressed() || Gui::LoginWindow::IsLoginButtonPressed())
		{
			window.PushWindow(std::make_unique<Gui::ChatWindow>());
			
			window.Draw();

			window.PopWindow();
		}
	}

	window.Cleanup();

	return 0;
}