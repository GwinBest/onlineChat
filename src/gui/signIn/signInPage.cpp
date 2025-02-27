#include "signInPage.h"

#include <QMessageBox>
#include <string>

#include "ui_signInPage.h"
#include "userData/user.h"
#include "userData/userCredentialsFile.h"
#include "userData/userRepository.h"

extern UserData::User currentUser;

namespace Gui
{
    using namespace CoroutineUtils;

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

    coroutine_void SignInPage::OnSignInButtonPressed()
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

        if (isFieldEmpty) co_return;

        currentUser.SetUserName(name);
        currentUser.SetUserLogin(login);
        currentUser.SetUserPassword(std::hash<std::string> {}(password));

        const std::optional<bool> isUserExist = co_await UserData::UserRepository::IsUserExistAsync(
            currentUser);

        if (!isUserExist.has_value())
        {
            QMetaObject::invokeMethod(
                this,
                [this] { QMessageBox::critical(nullptr, "Error", "Cant connect to the server"); },
                Qt::QueuedConnection);
            co_return;
        }

        if (isUserExist.value())
        {
            QMetaObject::invokeMethod(
                this,
                [this] { _ui->userExistLabel->setVisible(true); },
                Qt::QueuedConnection);

            co_return;
        }

        QMetaObject::invokeMethod(
            this,
            [this] { _ui->userExistLabel->setVisible(false); },
            Qt::QueuedConnection);

        const std::optional<bool> isPushSuccessful = co_await UserData::UserRepository::
            PushUserCredentialsToDatabaseAsync(currentUser);

        if (!isPushSuccessful.has_value())
        {
            QMetaObject::invokeMethod(
                this,
                [this] { QMessageBox::critical(nullptr, "Error", "Cant connect to the server"); },
                Qt::QueuedConnection);
            co_return;
        }

        if (!isPushSuccessful.value())
        {
            QMetaObject::invokeMethod(
                this,
                [this] {
                    QMessageBox::critical(nullptr, "Error", "Cant register user on the server");
                },
                Qt::QueuedConnection);
            co_return;
        };

        const std::optional<size_t>
            userId = co_await UserData::UserRepository::GetUserIdFromDatabaseAsync(login);

        if (!userId.has_value())
        {
            QMetaObject::invokeMethod(
                this,
                [this] { QMessageBox::critical(nullptr, "Error", "Cant connect to the server"); },
                Qt::QueuedConnection);
            co_return;
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
}   // namespace Gui