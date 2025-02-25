#pragma once

#include <QWidget>

#include "coroutineUtils/coroutineUtils.h"
#include "messageBuffer/messageBuffer.h"
#include "userData/user.h"

class QVBoxLayout;

// forward declaration
namespace Ui
{
    class ChatPage;
}   // namespace Ui

namespace Gui
{
    namespace Model
    {
        class AvailableChatsModel;
        class MessageModel;
    }   // namespace Model

    namespace Delegate
    {
        class AvailableChatsDelegate;
        class MessageDelegate;
    }   // namespace Delegate

    namespace Widget
    {
        class SideBarWidget;
    }   // namespace Widget
}   // namespace Gui

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
        void OnChatSelected();
        CoroutineUtils::coroutine_void OnSendButtonPressed();

    private:
        void resizeEvent(QResizeEvent* event) override;

        void keyPressEvent(QKeyEvent* event) override;
        void mousePressEvent(QMouseEvent* event) override;

        bool eventFilter(QObject* watched, QEvent* event) override;

        void ToggleUiElements() const;

        void AddLastMessage(const MessageBuffer::MessageNode& message);

        void HandleReceivedMessage(const size_t chatId, const MessageBuffer::MessageNode& message);

        static void
            SendMessage(const size_t chatId, const size_t senderUserId, const char* const data);

    private:
        Ui::ChatPage* _ui = nullptr;

        Model::AvailableChatsModel* _model = nullptr;
        Delegate::AvailableChatsDelegate* _delegate = nullptr;
        static inline bool _isChatPageVisible = false;

        Widget::SideBarWidget* _sideBarWidget = nullptr;
        static inline bool _isSideBarVisible = false;

        Model::MessageModel* _messageModel = nullptr;
        Delegate::MessageDelegate* _messageDelegate = nullptr;
        QWidget* _messagesContainer = nullptr;
        QVBoxLayout* _messagesContainerLayout = nullptr;

        static inline int lastSelectedRow = -1;
    };
}   // namespace Gui