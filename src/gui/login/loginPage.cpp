#include "loginPage.h"

#include "ui_loginPage.h"

#include "userData/user.h"
#include "userData/userRepository.h"

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

    void LoginPage::OnLoginButtonClicked() const noexcept
    {
        bool isDataValid = true;
        bool isFieldEmpty = false;

        QString login = _ui->loginInput->text().trimmed();
        QString password = _ui->passwordInput->text().trimmed();

        if (login.isEmpty())
        {
            _ui->loginInput->setStyleSheet(GetInputStyleSheet(colorRed));
            isFieldEmpty = true;
        }
        else
        {
            _ui->loginInput->setStyleSheet(GetInputStyleSheet(colorBlue));
        }

        if (password.isEmpty())
        {
            _ui->passwordInput->setStyleSheet(GetInputStyleSheet(colorRed));
            isFieldEmpty = true;
        }
        else
        {
            _ui->passwordInput->setStyleSheet(GetInputStyleSheet(colorBlue));
        }

        if (!isFieldEmpty)
        {
            UserData::User currentUser;
            currentUser.SetUserLogin(login.toStdString());
            currentUser.SetUserPassword(password.toInt());

            //isDataValid = UserData::UserRepository::IsUserExist(currentUser);
        }

        if (isDataValid)
        {
            emit LoginSuccessful();
        }
    }

    void LoginPage::OnSignInButtonClicked() const noexcept
    {
        emit DisplaySignInPage();
    }
} // !namespace Gui