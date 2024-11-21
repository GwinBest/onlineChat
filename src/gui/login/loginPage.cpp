#include "loginPage.h"
#include "ui_loginPage.h"

#include <string>

#include <QMessageBox>

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

    void LoginPage::OnLoginButtonClicked() const noexcept
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

        if (isFieldEmpty) return;

        currentUser.SetUserLogin(login);
        currentUser.SetUserPassword(std::hash<std::string>{}(password));

        std::optional<bool> isUserExist = UserData::UserRepository::IsUserExist(currentUser);

        if (!isUserExist.has_value())
        {
            QMessageBox::critical(nullptr, "Error", "Cant connect to the server");
            return;
        }

        if (!isUserExist.value())
        {
            _ui->userNotFound->setVisible(true);
            return;
        }
        else
        {
            _ui->userNotFound->setVisible(false);
        }

        std::optional<size_t> userId = UserData::UserRepository::GetUserIdFromDatabase(login);
        std::optional<std::string> userName = UserData::UserRepository::GetUserNameFromDatabase(login);

        if (!userId.has_value() || !userName.has_value())
        {
            QMessageBox::critical(nullptr, "Error", "Cant connect to the server");
            return;
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

} // !namespace Gui