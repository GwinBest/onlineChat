#pragma once

#include <QWidget>

#include "coroutineUtils/coroutineUtils.h"
#include "messageBuffer/messageBuffer.h"
#include "userData/user.h"

class QVBoxLayout;

// forward declaration
class Ui::ChatPage;
class Gui::Model::AvailableChatsModel;
class Gui::Delegate::AvailableChatsDelegate;
class Gui::Widget::SideBarWidget;

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

        CoroutineUtils::coroutine_void FillMessageContainerLayout(const size_t chatId);
        void ClearMessageLayout() const;

        static void RemoveLastSpacerItem(QVBoxLayout* layout);
        static QString ExtractDateTimeFromMessageWidget(const QWidget* messageWidget);

        static void
            SendMessage(const size_t chatId, const size_t senderUserId, const char* const data);
        void RenderLastMessage(const MessageBuffer::MessageNode& message) const;

    private:
        Ui::ChatPage* _ui = nullptr;

        Model::AvailableChatsModel* _model = nullptr;
        Delegate::AvailableChatsDelegate* _delegate = nullptr;
        static inline bool _isChatPageVisible = false;

        Widget::SideBarWidget* _sideBarWidget = nullptr;
        static inline bool _isSideBarVisible = false;

        QWidget* _messagesContainer = nullptr;
        QVBoxLayout* _messagesContainerLayout = nullptr;

        static inline int lastSelectedRow = -1;
    };
}   // namespace Gui