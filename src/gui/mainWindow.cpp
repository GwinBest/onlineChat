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

    connect(_loginPage, &Gui::LoginPage::DisplaySignInPage, this, [this]()
        {
            _loginPage->ResetUiStyle();
            _signInPage->ResetUiStyle();
            _ui->stackedWidget->setCurrentWidget(_signInPage);
        }
    );
    connect(_loginPage, &Gui::LoginPage::LoginSuccessful, this, [this]()
        {
            _loginPage->ResetUiStyle();
            _signInPage->ResetUiStyle();
            _chatPage->PreparePage();
            _ui->stackedWidget->setCurrentWidget(_chatPage);
        }
    );

    connect(_signInPage, &Gui::SignInPage::DisplayLoginPage, this, [this]()
        {
            _loginPage->ResetUiStyle();
            _signInPage->ResetUiStyle();
            _ui->stackedWidget->setCurrentWidget(_loginPage);
        }
    );
    connect(_signInPage, &Gui::SignInPage::SignInSuccessful, this, [this]()
        {
            _loginPage->ResetUiStyle();
            _signInPage->ResetUiStyle();
            _chatPage->PreparePage();
            _ui->stackedWidget->setCurrentWidget(_chatPage);
        }
    );
}

MainWindow::~MainWindow()
{
    delete _ui;

    delete _loginPage;
    delete _signInPage;
    delete _chatPage;
}