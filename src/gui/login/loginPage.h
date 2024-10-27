#pragma once

#include <QWidget>

// forward declaration
namespace Ui { class LoginPage; }

namespace Gui
{
    class LoginPage final : public QWidget
    {
        Q_OBJECT

    public:
        explicit LoginPage(QWidget* parent = nullptr);
        ~LoginPage() override;

    signals:
        void LoginSuccessful() const;
        void DisplaySignInPage() const;

    private slots:
        void OnLoginButtonClicked() const noexcept;
        void OnSignInButtonClicked() const noexcept;

    private:
        inline QString GetInputStyleSheet(const QString& primaryColor) const noexcept
        {
            return "QLineEdit { \
                    background-color: rgba(0, 0, 0, 0); \
                    border: none; \
                    border-bottom: 1px solid " + primaryColor + ";}" +
                "QLineEdit:focus {\
                    background-color: rgba(0,0,0,0);\
                    border: none;\
                    border-bottom: 1px solid rgb(0, 255, 132);}";
        }

    private:
        Ui::LoginPage* _ui = nullptr;
    };
} // !namespace Gui