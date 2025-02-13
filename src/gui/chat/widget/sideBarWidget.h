#pragma once

#include <QWidget>
#include <string>

// forward declaration
class Ui::SideBarWidget;

namespace Gui::Widget
{
    class SideBarWidget final : public QWidget
    {
        Q_OBJECT

    public:
        explicit SideBarWidget(QWidget* parent = nullptr);
        ~SideBarWidget() override;

        void UpdateUserName(const std::string_view name) const noexcept;

    signals:
        void LogOutButtonPressed() const;
        void HideSideBar() const;

    private slots:
        void OnLogOutButtonPressed() const noexcept;

    private:
        bool eventFilter(QObject* obj, QEvent* event) override;

    public:
        static constexpr uint16_t sideBarWidth = 270;

    private:
        Ui::SideBarWidget* _ui = nullptr;
    };
}   // namespace Gui::Widget