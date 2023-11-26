#include "../src/gui/glfwWindow.h"
#include "../src/gui/chatWindow.h"
#include "../src/gui/loginWindow.h"
#include "../src/gui/signUpWindow.h"

enum class WindowState : uint8_t
{
	kLogin,
	kSignUp,
	kChat
};

int main()
{
	Gui::GlfwWindow window;
	if (!window.Init())
	{
		return 1;
	}

    WindowState currentWindowState = WindowState::kLogin;

	while (!glfwWindowShouldClose(window.GetGlfwWindow()))
	{
        switch (currentWindowState)
        {
        case WindowState::kLogin:
        {
            window.PushWindow(std::make_unique<Gui::LoginWindow>());
            window.Draw();
            window.PopWindow();

            if (Gui::LoginWindow::IsSignUpLabelPressed() || Gui::SignUpWindow::IsBackToLoginButtonPressed()) 
            {
                currentWindowState = WindowState::kSignUp;
            }
            else if (Gui::LoginWindow::IsLoginButtonPressed())
            {
        
                currentWindowState = WindowState::kChat;
            }
            break;

        }
        case WindowState::kSignUp:
        {
            window.PushWindow(std::make_unique<Gui::SignUpWindow>());

            window.Draw();

            window.PopWindow();
            if (Gui::SignUpWindow::IsBackToLoginButtonPressed())
            {
                currentWindowState = WindowState::kLogin;
            }
            else if (Gui::SignUpWindow::IsSignUpButtonPressed()) 
            {
             
                currentWindowState = WindowState::kChat;
            }
            break;
        }
        case WindowState::kChat:
        {
            window.PushWindow(std::make_unique<Gui::ChatWindow>());

            window.Draw();

            window.PopWindow();
            break;
        }
        }

	}

	window.Cleanup();

	return 0;
}