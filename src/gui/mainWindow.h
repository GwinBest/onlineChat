#pragma once

#include <QMainWindow>

#include "gui/chat/chatPage.h"
#include "gui/login/loginPage.h"
#include "gui/signIn/signInPage.h"

namespace Ui { class MainWindowClass; };

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindowClass* _ui = nullptr;

    Gui::LoginPage* _loginPage = nullptr;
    Gui::SignInPage* _signInPage = nullptr;
    Gui::ChatPage* _chatPage = nullptr;
};
