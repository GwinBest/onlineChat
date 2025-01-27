#pragma once

#include <QWidget>

#include "coroutineUtils/coroutineUtils.h"
#include "gui/colors/colors.h"

// forward declaration
namespace Ui
{
    class LoginPage;
}

namespace Gui
{
    class LoginPage final : public QWidget
    {
        Q_OBJECT

    public:
        explicit LoginPage(QWidget *parent = nullptr);
        ~LoginPage() override;

        void PreparePage() const noexcept;

    signals:
        void LoginSuccessful() const;
        void DisplaySignInPage() const;

    private slots:
        CoroutineUtils::coroutine_void OnLoginButtonClicked();
        void OnSignInButtonClicked() const noexcept;

    private:
        inline QString SetInputStyleSheet(const QString &primaryColor) const noexcept
        {
            return "QLineEdit { "
                   "background-color: " +
                   QString(colorTransparent) + ";" +
                   "border: none; "
                   "border-bottom: 1px solid " +
                   primaryColor + ";}" +
                   "QLineEdit:focus { "
                   "background-color: " +
                   QString(colorTransparent) + "; " +
                   "border: none; "
                   "border-bottom: 1px solid " +
                   QString(colorLightGreen) + ";}";
        }

    private:
        Ui::LoginPage *_ui = nullptr;
    };
} // !namespace Gui