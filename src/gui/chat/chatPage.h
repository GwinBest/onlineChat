#pragma once

#include <QWidget>

class QVBoxLayout;

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
        void OnChatSelected() const;

    private:
        void resizeEvent(QResizeEvent* event) override;

        void keyPressEvent(QKeyEvent* event) override;
        void mousePressEvent(QMouseEvent* event) override;

        void ToggleUiElements() const;

        void FillMessageContainerLayout(const size_t chatId) const;

    private:
        Ui::ChatPage* _ui = nullptr;

        Model::AvailableChatsModel* _model = nullptr;
        Delegate::AvailableChatsDelegate* _delegate = nullptr;
        static inline bool _isChatPageVisible = false;

        Widget::SideBarWidget* _sideBarWidget = nullptr;
        static inline bool _isSideBarVisible = false;

        QWidget* _messagesContainer = nullptr;
        QVBoxLayout* _messagesContainerLayout = nullptr;
    };
} // !namespace Gui