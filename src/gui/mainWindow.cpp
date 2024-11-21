#include "mainWindow.h"

#include "ui_mainWindow.h"

#include "gui/chat/model/availableChatsModel.h"


MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , _ui(new Ui::MainWindowClass())
    , _loginPage(new Gui::LoginPage())
    , _signInPage(new Gui::SignInPage())
    , _chatPage(new Gui::ChatPage())
{
    _ui->setupUi(this);

    _ui->stackedWidget->addWidget(_loginPage);
    _ui->stackedWidget->addWidget(_signInPage);
    _ui->stackedWidget->addWidget(_chatPage);

    connect(_loginPage, &Gui::LoginPage::DisplaySignInPage, this, &MainWindow::DisplaySignInPage);
    connect(_loginPage, &Gui::LoginPage::LoginSuccessful, this, &MainWindow::DisplayChatPage);

    connect(_signInPage, &Gui::SignInPage::DisplayLoginPage, this, &MainWindow::DisplayLoginPage);
    connect(_signInPage, &Gui::SignInPage::SignInSuccessful, this, &MainWindow::DisplayChatPage);
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
