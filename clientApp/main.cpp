#include "../src/gui/glfwWindow.h"
#include "../src/gui/chatWindow.h"
#include "../src/gui/loginWindow.h"
#include "../src/gui/signUpWindow.h"
#include "../src/userData/userCredentialsFile.h"

enum class WindowState : uint8_t
{
	kLogin,
	kSignUp,
	kChat
};

int main()
{
	Gui::GlfwWindow window;
    WindowState currentWindowState;

    UserData::User user;
    std::string userName;
    std::string userLogin;
    std::string userPassword;

    if (UserData::UserCredentialsFile::IsFileExists())
    {
        UserData::UserCredentialsFile::ReadCredentials(userName, userLogin, userPassword);

        if (UserData::User::IsUserExist(userLogin, userPassword))
        {
            userName = UserData::User::GetUserNameFromDatabase(userLogin, userPassword);
            user.SetUserName(userName);
            user.SetUserLogin(userLogin);
            user.SetUserPassword(userPassword);
            currentWindowState = WindowState::kChat;
        }
        else
        {
            currentWindowState = WindowState::kLogin;
        }
    }
    else
    {
        currentWindowState = WindowState::kLogin;
    }

	while (!glfwWindowShouldClose(window.GetGlfwWindow()))
	{
        switch (currentWindowState)
        {
        case WindowState::kLogin:
        {
            window.PushWindow(std::make_unique<Gui::LoginWindow>());
            window.Draw();
            window.PopWindow();

            if (Gui::LoginWindow::IsSignUpLabelPressed()) 
            {
                currentWindowState = WindowState::kSignUp;
            }
            else if (Gui::LoginWindow::IsLoginButtonPressed())
            {
                userLogin       = Gui::LoginWindow::GetLogin();
                userPassword    = Gui::LoginWindow::GetPassword();

                if (UserData::User::IsUserExist(userLogin, userPassword))
                {
                    if (UserData::UserCredentialsFile::CreateNewFile())
                    {
                        userName = UserData::User::GetUserNameFromDatabase(userLogin, userPassword);
                        user.SetUserName(userName);
                        user.SetUserLogin(userLogin);
                        user.SetUserPassword(userPassword);

                        UserData::UserCredentialsFile::WriteCredentials(userName, userLogin, userPassword);
                        UserData::UserCredentialsFile::CloseFile();
                    }

                    currentWindowState = WindowState::kChat;
                }
                else
                {
                    Gui::LoginWindow::SetShowUserNotFoundMessage(true);
                }
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
                std::string userName    = Gui::SignUpWindow::GetName();
                userLogin               = Gui::SignUpWindow::GetLogin();
                userPassword            = Gui::SignUpWindow::GetPassword();
                
                if (UserData::User::IsUserExist(userLogin, userPassword))
                {
                    Gui::SignUpWindow::SetShowUserAlreadyExistMessage(true);
                }
                else
                {
                    if (UserData::UserCredentialsFile::CreateNewFile())
                    {
                        UserData::UserCredentialsFile::WriteCredentials(userName, userLogin, userPassword);
                        UserData::UserCredentialsFile::CloseFile();
                    }

                    currentWindowState = WindowState::kChat;
                }
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

	return 0;
}