#include "mainWindow.h"

#include "ui_mainWindow.h"

#include "gui/chat/model/availableChatsModel.h"
#include "userData/userCredentialsFile.h"
#include "userData/userRepository.h"

extern UserData::User currentUser;

namespace Gui
{
    MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent), _ui(new Ui::MainWindowClass()), _loginPage(new Gui::LoginPage()), _signInPage(new Gui::SignInPage()), _chatPage(new Gui::ChatPage())
    {
        _ui->setupUi(this);

        _ui->stackedWidget->addWidget(_loginPage);
        _ui->stackedWidget->addWidget(_signInPage);
        _ui->stackedWidget->addWidget(_chatPage);

        connect(_loginPage, &LoginPage::DisplaySignInPage, this, &MainWindow::DisplaySignInPage);
        connect(_loginPage, &LoginPage::LoginSuccessful, this, &MainWindow::DisplayChatPage);

        connect(_signInPage, &SignInPage::DisplayLoginPage, this, &MainWindow::DisplayLoginPage);
        connect(_signInPage, &SignInPage::SignInSuccessful, this, &MainWindow::DisplayChatPage);

        connect(_chatPage, &ChatPage::LogOutButtonPressed, this, &MainWindow::DisplayLoginPage);

        PreparePage();
    }

    MainWindow::~MainWindow()
    {
        delete _ui;

        delete _loginPage;
        delete _signInPage;
        delete _chatPage;
    }

    void MainWindow::DisplaySignInPage() const noexcept
    {
        _signInPage->PreparePage();
        _ui->stackedWidget->setCurrentWidget(_signInPage);
    }

    void MainWindow::DisplayLoginPage() const noexcept
    {
        _loginPage->PreparePage();
        _ui->stackedWidget->setCurrentWidget(_loginPage);
    }

    void MainWindow::DisplayChatPage() const noexcept
    {
        _chatPage->PreparePage();
        _ui->stackedWidget->setCurrentWidget(_chatPage);
    }

    CoroutineUtils::coroutine_void MainWindow::PreparePage() noexcept
    {
        if (!UserData::UserCredentialsFile::IsFileExists())
            DisplayLoginPage();

        const UserData::User user = UserData::UserCredentialsFile::ReadCredentials();
        auto isUserExist = co_await UserData::UserRepository::IsUserExistAsync(user);
        if (!isUserExist.value_or(false))
            DisplayLoginPage();

        currentUser = std::move(user);

        QMetaObject::invokeMethod(this, [this]
                                  { DisplayChatPage(); }, Qt::QueuedConnection);
    }
} // !namespace Gui