#include "loginPage.h"

#include <QMessageBox>
#include <coroutine>
#include <string>

#include "ui_loginPage.h"
#include "userData/user.h"
#include "userData/userCredentialsFile.h"
#include "userData/userRepository.h"

extern UserData::User currentUser;

namespace Gui
{
    LoginPage::LoginPage(QWidget* parent)
        : QWidget(parent)
        , _ui(new Ui::LoginPage())
    {
        _ui->setupUi(this);

        PreparePage();

        connect(_ui->loginButton, &QPushButton::clicked, this, &LoginPage::OnLoginButtonClicked);
        connect(_ui->signInButton, &QPushButton::clicked, this, &LoginPage::OnSignInButtonClicked);
    }

    LoginPage::~LoginPage()
    {
        delete _ui;
    }

    void LoginPage::PreparePage() const noexcept
    {
        _ui->loginInput->setStyleSheet(SetInputStyleSheet(colorBlue));
        _ui->loginInput->clear();

        _ui->passwordInput->setStyleSheet(SetInputStyleSheet(colorBlue));
        _ui->passwordInput->clear();

        _ui->userNotFound->setVisible(false);
    }

    CoroutineUtils::coroutine_void LoginPage::OnLoginButtonClicked()
    {
        bool isFieldEmpty = false;

        const std::string login = _ui->loginInput->text().trimmed().toStdString();
        const std::string password = _ui->passwordInput->text().trimmed().toStdString();

        if (login.empty())
        {
            _ui->loginInput->setStyleSheet(SetInputStyleSheet(colorRed));
            isFieldEmpty = true;
        }
        else
        {
            _ui->loginInput->setStyleSheet(SetInputStyleSheet(colorBlue));
        }

        if (password.empty())
        {
            _ui->passwordInput->setStyleSheet(SetInputStyleSheet(colorRed));
            isFieldEmpty = true;
        }
        else
        {
            _ui->passwordInput->setStyleSheet(SetInputStyleSheet(colorBlue));
        }

        if (isFieldEmpty) co_return;

        currentUser.SetUserLogin(login);
        currentUser.SetUserPassword(std::hash<std::string> {}(password));

        auto isUserExist = co_await UserData::UserRepository::IsUserExistAsync(currentUser);

        if (!isUserExist.has_value())
        {
            QMetaObject::invokeMethod(
                this,
                [this] { QMessageBox::critical(nullptr, "Error", "Cant connect to the server"); },
                Qt::QueuedConnection);
            co_return;
        }

        if (!isUserExist.value())
        {
            QMetaObject::invokeMethod(
                this,
                [this] { _ui->userNotFound->setVisible(true); },
                Qt::QueuedConnection);
            co_return;
        }

        QMetaObject::invokeMethod(
            this,
            [this] { _ui->userNotFound->setVisible(false); },
            Qt::QueuedConnection);

        auto userId = co_await UserData::UserRepository::GetUserIdFromDatabaseAsync(login);
        auto userName = co_await UserData::UserRepository::GetUserNameFromDatabaseAsync(login);

        if (!userId.has_value() || !userName.has_value())
        {
            QMetaObject::invokeMethod(
                this,
                [this] { QMessageBox::critical(nullptr, "Error", "Cant connect to the server"); },
                Qt::QueuedConnection);
            co_return;
        }

        currentUser.SetUserId(userId.value());
        currentUser.SetUserName(userName.value());

        if (UserData::UserCredentialsFile::CreateNewFile())
        {
            UserData::UserCredentialsFile::WriteCredentials(currentUser);
            UserData::UserCredentialsFile::CloseFile();
        }

        emit LoginSuccessful();
    }

    void LoginPage::OnSignInButtonClicked() const noexcept
    {
        emit DisplaySignInPage();
    }
}   // namespace Gui