#include <functional>


#include "../src/userData/user.h"
#include "../src/userData/userCredentialsFile.h"
#include "../src/userData/userRepository.h"


#include "../src/gui/login/loginPage.h"

#include <QApplication>

#include "gui/mainWindow.h"

UserData::User currentUser;

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    app.setStyle("fusion");
    MainWindow window;
    window.show();

    return app.exec();


    /*Gui::LoginPage page;
    page.show();*/
    /*Gui::GlfwWindow window;
    WindowState currentWindowState = WindowState::kLogin;

    std::string userName;
    std::string userLogin;
    size_t userPassword = 0;
    size_t userId = 0;

    if (UserData::UserCredentialsFile::IsFileExists())
    {
        UserData::UserCredentialsFile::ReadCredentials(userName, userLogin, userPassword, userId);
        UserData::UserCredentialsFile::CloseFile();

        if (UserData::UserRepository::IsUserDataFromFileValid(userName, userLogin, userPassword, userId))
        {
            currentUser.SetUserName(userName);
            currentUser.SetUserLogin(userLogin);
            currentUser.SetUserPassword(userPassword);
            currentUser.SetUserId(userId);

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
                userLogin = Gui::LoginWindow::GetLogin();
                std::string stringUserPassword = Gui::LoginWindow::GetPassword();

                userPassword = std::hash<std::string>{}(stringUserPassword.c_str());

                if (UserData::UserRepository::IsUserExist(userLogin, userPassword))
                {
                    userName = UserData::UserRepository::GetUserNameFromDatabase(userLogin);
                    userId = UserData::UserRepository::GetUserIdFromDatabase(userLogin);

                    currentUser.SetUserName(userName);
                    currentUser.SetUserLogin(userLogin);
                    currentUser.SetUserPassword(userPassword);
                    currentUser.SetUserId(userId);

                    if (UserData::UserCredentialsFile::CreateNewFile())
                    {
                        UserData::UserCredentialsFile::WriteCredentials(userName, userLogin, userPassword, userId);
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
                userName = Gui::SignUpWindow::GetName();
                userLogin = Gui::SignUpWindow::GetLogin();
                std::string stringUserPassword = Gui::SignUpWindow::GetPassword();

                userPassword = std::hash<std::string>{}(stringUserPassword);

                if (UserData::UserRepository::IsUserExist(userLogin, userPassword))
                {
                    Gui::SignUpWindow::SetShowUserAlreadyExistMessage(true);
                }
                else
                {
                    if (UserData::UserCredentialsFile::CreateNewFile())
                    {
                        UserData::UserCredentialsFile::WriteCredentials(userName, userLogin, userPassword, userId);
                        UserData::UserCredentialsFile::CloseFile();
                    }

                    if (!UserData::UserRepository::PushUserCredentialsToDatabase(userName, userLogin, userPassword))
                    {
                        exit(-1);
                    }

                    userId = UserData::UserRepository::GetUserIdFromDatabase(userLogin);

                    currentUser.SetUserName(userName);
                    currentUser.SetUserLogin(userLogin);
                    currentUser.SetUserPassword(userPassword);
                    currentUser.SetUserId(userId);

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
    }*/

}