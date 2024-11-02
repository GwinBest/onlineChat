#include "signInPage.h"
#include "ui_signInPage.h"

#include <string>

#include <QMessageBox>

#include "userData/user.h"
#include "userData/userCredentialsFile.h"
#include "userData/userRepository.h"

extern UserData::User currentUser;

namespace Gui
{
    SignInPage::SignInPage(QWidget* parent)
        : QWidget(parent)
        , _ui(new Ui::SignInPage())
    {
        _ui->setupUi(this);

        _ui->userNotFoundLabel->setVisible(false);

        connect(_ui->signInButton, &QPushButton::clicked, this, &SignInPage::OnSignInButtonPressed);
        connect(_ui->loginButton, &QPushButton::clicked, this, &SignInPage::OnLoginButtonPressed);
    }

    SignInPage::~SignInPage()
    {
        delete _ui;
    }

    void SignInPage::ResetUiStyle() const noexcept
    {
        _ui->nameInput->setStyleSheet(SetInputStyleSheet(colorBlue));
        _ui->nameInput->clear();

        _ui->loginInput->setStyleSheet(SetInputStyleSheet(colorBlue));
        _ui->loginInput->clear();

        _ui->passwordInput->setStyleSheet(SetInputStyleSheet(colorBlue));
        _ui->passwordInput->clear();
    }

    void SignInPage::OnSignInButtonPressed() const noexcept
    {
        bool isDataValid = true;
        bool isFieldEmpty = false;

        const std::string name = _ui->nameInput->text().trimmed().toStdString();
        const std::string login = _ui->loginInput->text().trimmed().toStdString();
        const std::string password = _ui->passwordInput->text().trimmed().toStdString();

        if (name.empty())
        {
            _ui->nameInput->setStyleSheet(SetInputStyleSheet(colorRed));
            isFieldEmpty = true;
        }
        else
        {
            _ui->nameInput->setStyleSheet(SetInputStyleSheet(colorBlue));
        }

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

        currentUser.SetUserName(name);
        currentUser.SetUserLogin(login);
        currentUser.SetUserPassword(std::hash<std::string>{}(password));

        if (UserData::UserRepository::IsUserExist(currentUser))
        {
            _ui->userNotFoundLabel->setVisible(true);
            return;
        }
        else
        {
            _ui->userNotFoundLabel->setVisible(false);
        }

        if (UserData::UserCredentialsFile::CreateNewFile())
        {
            UserData::UserCredentialsFile::WriteCredentials(currentUser);
            UserData::UserCredentialsFile::CloseFile();
        }

        if (!UserData::UserRepository::PushUserCredentialsToDatabase(currentUser))
        {
            QMessageBox::critical(nullptr, "Error", "Cant register user to database");
            return;
        };

        currentUser.SetUserId(UserData::UserRepository::GetUserIdFromDatabase(login));

        emit SignInSuccessful();
    }

    void SignInPage::OnLoginButtonPressed() const noexcept
    {
        emit DisplayLoginPage();
    }

} // !namespace Gui