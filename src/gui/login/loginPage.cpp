#include "loginPage.h"
#include "ui_loginPage.h"

#include <string>

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

        connect(_ui->loginButton, &QPushButton::clicked, this, &LoginPage::OnLoginButtonClicked);
        connect(_ui->signInButton, &QPushButton::clicked, this, &LoginPage::OnSignInButtonClicked);
    }

    LoginPage::~LoginPage()
    {
        delete _ui;
    }

    void LoginPage::ResetUiStyle() const noexcept
    {
        _ui->loginInput->setStyleSheet(SetInputStyleSheet(colorBlue));
        _ui->loginInput->clear();

        _ui->passwordInput->setStyleSheet(SetInputStyleSheet(colorBlue));
        _ui->passwordInput->clear();
    }

    void LoginPage::OnLoginButtonClicked() const noexcept
    {
        bool isDataValid = true;
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

        isDataValid = UserData::UserRepository::IsUserExist(currentUser);

        if (!isDataValid) return;

        currentUser.SetUserId(UserData::UserRepository::GetUserIdFromDatabase(login));
        currentUser.SetUserName(UserData::UserRepository::GetUserNameFromDatabase(login));

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