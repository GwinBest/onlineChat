#pragma once

#include <QMainWindow>

#include "gui/chat/chatPage.h"
#include "gui/login/loginPage.h"
#include "gui/signIn/signInPage.h"

// forward declaration
namespace Ui
{
    class MainWindowClass;
};

namespace Gui
{
    class MainWindow : public QMainWindow
    {
        Q_OBJECT

    public:
        explicit MainWindow(QWidget *parent = nullptr);
        ~MainWindow() override;

    private slots:
        void DisplaySignInPage() const noexcept;
        void DisplayLoginPage() const noexcept;
        void DisplayChatPage() const noexcept;

    private:
        CoroutineUtils::coroutine_void PreparePage() noexcept;

    private:
        Ui::MainWindowClass *_ui = nullptr;

        LoginPage *_loginPage = nullptr;
        SignInPage *_signInPage = nullptr;
        ChatPage *_chatPage = nullptr;
    };
} // !namespace Gui