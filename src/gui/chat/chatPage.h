#pragma once

#include <QWidget>

// forward declaration
namespace Ui { class ChatPage; }

namespace Gui::Model { class AvailableChatsModel; }

namespace Gui::Delegate { class AvailableChatsDelegate; }

namespace Gui::Widget { class SideBarWidget; }

namespace Gui
{
    class ChatPage final : public QWidget
    {
        Q_OBJECT

    public:
        explicit ChatPage(QWidget* parent = nullptr);
        ~ChatPage() override;

        void PreparePage() const noexcept;

    signals:
        void LogOutButtonPressed();

    private slots:
        void ToggleSideMenu() const noexcept;
        void OnSearchInputTextChanged() const noexcept;

    private:
        void resizeEvent(QResizeEvent* event) override;

        void keyPressEvent(QKeyEvent* event) override;
        void mousePressEvent(QMouseEvent* event) override;
    private:
        Ui::ChatPage* _ui = nullptr;

        Model::AvailableChatsModel* _model = nullptr;
        Delegate::AvailableChatsDelegate* _delegate = nullptr;

        Widget::SideBarWidget* _sideBarWidget = nullptr;
        static inline bool _isSideBarVisible = false;
    };
} // !namespace Gui