#pragma once

#include <QWidget>

#include "coroutineUtils/coroutineUtils.h"
#include "gui/colors/colors.h"

// forward declaration
class Ui::SignInPage;

namespace Gui
{
    class SignInPage final : public QWidget
    {
        Q_OBJECT

    public:
        explicit SignInPage(QWidget* parent = nullptr);
        ~SignInPage() override;

        void PreparePage() const noexcept;

    signals:
        void SignInSuccessful() const;
        void DisplayLoginPage() const;

    private slots:
        CoroutineUtils::coroutine_void OnSignInButtonPressed();
        void OnLoginButtonPressed() const noexcept;

    private:
        inline QString SetInputStyleSheet(const QString& primaryColor) const noexcept
        {
            return "QLineEdit { "
                   "background-color: "
                 + QString(colorTransparent) + ";"
                 + "border: none; "
                   "border-bottom: 1px solid "
                 + primaryColor + ";}"
                 + "QLineEdit:focus { "
                   "background-color: "
                 + QString(colorTransparent) + "; "
                 + "border: none; "
                   "border-bottom: 1px solid "
                 + QString(colorLightGreen) + ";}";
        }

    private:
        Ui::SignInPage* _ui = nullptr;
    };
}   // namespace Gui