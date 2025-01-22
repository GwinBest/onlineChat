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

        PreparePage();

        connect(_ui->signInButton, &QPushButton::clicked, this, &SignInPage::OnSignInButtonPressed);
        connect(_ui->loginButton, &QPushButton::clicked, this, &SignInPage::OnLoginButtonPressed);
    }

    SignInPage::~SignInPage()
    {
        delete _ui;
    }

    void SignInPage::PreparePage() const noexcept
    {
        _ui->nameInput->setStyleSheet(SetInputStyleSheet(colorBlue));
        _ui->nameInput->clear();

        _ui->loginInput->setStyleSheet(SetInputStyleSheet(colorBlue));
        _ui->loginInput->clear();

        _ui->passwordInput->setStyleSheet(SetInputStyleSheet(colorBlue));
        _ui->passwordInput->clear();

        _ui->userExistLabel->setVisible(false);
    }

    void SignInPage::OnSignInButtonPressed() const noexcept
    {
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

        const std::optional<bool> isUserExist = UserData::UserRepository::IsUserExist(currentUser);

        if (!isUserExist.has_value())
        {
            QMessageBox::critical(nullptr, "Error", "Cant connect to the server");
            return;
        }

        if (isUserExist.value())
        {
            _ui->userExistLabel->setVisible(true);
            return;
        }
        else
        {
            _ui->userExistLabel->setVisible(false);
        }

        const std::optional<bool> isPushSuccessful = UserData::UserRepository::PushUserCredentialsToDatabase(currentUser);

        if (!isPushSuccessful.has_value())
        {
            QMessageBox::critical(nullptr, "Error", "Cant connect to the server");
            return;
        }

        if (!isPushSuccessful.value())
        {
            QMessageBox::critical(nullptr, "Error", "Cant register user on the server");
            return;
        };

        const std::optional<size_t> userId = UserData::UserRepository::GetUserIdFromDatabase(login);

        if (!userId.has_value())
        {
            QMessageBox::critical(nullptr, "Error", "Cant connect to the server");
            return;
        }

        currentUser.SetUserId(userId.value());

        if (UserData::UserCredentialsFile::CreateNewFile())
        {
            UserData::UserCredentialsFile::WriteCredentials(currentUser);
            UserData::UserCredentialsFile::CloseFile();
        }

        emit SignInSuccessful();
    }

    void SignInPage::OnLoginButtonPressed() const noexcept
    {
        emit DisplayLoginPage();
    }
} // !namespace Gui