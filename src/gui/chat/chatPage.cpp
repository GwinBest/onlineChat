#include "chatPage.h"

#include <QDateTime>
#include <QKeyEvent>
#include <QPropertyAnimation>
#include <QRegularExpression>
#include <QScrollBar>

#include "chatSystem/chatInfo.h"
#include "client/client.h"
#include "common/common.h"
#include "gui/chat/delegate/availableChatsDelegate.h"
#include "gui/chat/delegate/messagesDelegate.h"
#include "gui/chat/model/availableChatsModel.h"
#include "gui/chat/model/messagesModel.h"
#include "gui/chat/scrollArea/chatScrollArea.h"
#include "gui/chat/widget/sideBarWidget.h"
#include "ui_chatPage.h"
#include "userData/user.h"
#include "userData/userRepository.h"

#ifdef SendMessage
#undef SendMessage
#endif   // SendMessage

#ifdef GetUserName
#undef GetUserName
#endif   // GetUserName

extern UserData::User currentUser;

namespace Gui
{
    using namespace CoroutineUtils;

    bool viewAtBottom = true;

    ChatPage::ChatPage(QWidget* parent)
        : QWidget(parent)
        , _ui(new Ui::ChatPage())
        , _model(new Model::AvailableChatsModel())
        , _delegate(new Delegate::AvailableChatsDelegate(_ui->availableChatsList))
        , _messagesContainer(new QWidget())
        , _messageDelegate(new Delegate::MessageDelegate(_ui->messageView))
    {
        _ui->setupUi(this);

        _sideBarWidget = new Widget::SideBarWidget(this);
        _sideBarWidget->setVisible(false);

        _messageModel = new Model::MessageModel(this);
        _ui->messageView->setModel(_messageModel);
        _ui->messageView->setItemDelegate(_messageDelegate);
        _ui->messageView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        _ui->messageView->setStyleSheet(R"(
    QScrollBar:vertical {
        background-color: #2A2A2A;
        width: 8px;
        margin: 0;
        border-radius: 4px;
    }

    QScrollBar::handle:vertical {
        background-color: #666666;
        min-height: 30px;
        border-radius: 4px;
    }

    QScrollBar::handle:vertical:hover {
        background-color: #7A7A7A;
    }

    QScrollBar::handle:vertical:pressed {
        background-color: #888888;
    }

    QScrollBar::add-line:vertical,
    QScrollBar::sub-line:vertical {
        height: 0px;
    }

    QScrollBar::add-page:vertical,
    QScrollBar::sub-page:vertical {
        background: none;
    }

    QScrollBar:horizontal {
        background-color: #2A2A2A;
        height: 8px;
        margin: 0;
        border-radius: 4px;
    }

    QScrollBar::handle:horizontal {
        background-color: #666666;
        min-width: 30px;
        border-radius: 4px;
    }

    QScrollBar::handle:horizontal:hover {
        background-color: #7A7A7A;
    }

    QScrollBar::handle:horizontal:pressed {
        background-color: #888888;
    }

    QScrollBar::add-line:horizontal,
    QScrollBar::sub-line:horizontal {
        width: 0px;
    }

    QScrollBar::add-page:horizontal,
    QScrollBar::sub-page:horizontal {
        background: none;
    }
)");

        _messagesContainerLayout = new QVBoxLayout(_messagesContainer);
        _messagesContainer->setLayout(_messagesContainerLayout);

        _ui->availableChatsList->setMouseTracking(true);
        _ui->availableChatsList->setUniformItemSizes(true);
        _ui->availableChatsList->setResizeMode(QListView::Adjust);

        _ui->messageInput->installEventFilter(this);

        connect(_sideBarWidget,
                &Widget::SideBarWidget::HideSideBar,
                this,
                &ChatPage::ToggleSideMenu);
        connect(_sideBarWidget, &Widget::SideBarWidget::LogOutButtonPressed, this, [this] {
            emit LogOutButtonPressed();
            ToggleSideMenu();
        });

        connect(_ui->messageView->model(), &QAbstractItemModel::rowsAboutToBeInserted, this, [&] {
            const auto* bar = _ui->messageView->verticalScrollBar();
            if (bar) viewAtBottom = (bar->value() == bar->maximum());
        });

        connect(_ui->messageView->model(), &QAbstractItemModel::rowsInserted, this, [&] {
            const auto* bar = _ui->messageView->verticalScrollBar();
            if (bar && !viewAtBottom) _ui->messageView->scrollToBottom();
        });

        connect(_ui->menuButton, &QPushButton::clicked, this, &ChatPage::ToggleSideMenu);

        connect(_ui->searchInput,
                &QLineEdit::textChanged,
                this,
                &ChatPage::OnSearchInputTextChanged);

        connect(_ui->availableChatsList, &QListView::clicked, this, &ChatPage::OnChatSelected);

        connect(_ui->sendMessageButton,
                &QPushButton::clicked,
                this,
                &ChatPage::OnSendButtonPressed);

        if (const auto clientInstance = ClientNetworking::Client::GetInstance();
            clientInstance.has_value())
        {
            clientInstance.value().get().RegisterReceiveMessageCallback(
                [this](const size_t chatId, const MessageBuffer::MessageNode& message) {
                    QMetaObject::invokeMethod(
                        this,
                        [this, chatId, message] { HandleReceivedMessage(chatId, message); },
                        Qt::QueuedConnection);
                });
        }
    }

    ChatPage::~ChatPage()
    {
        delete _ui;
        delete _model;
        delete _delegate;
        delete _sideBarWidget;
        delete _messagesContainerLayout;
        delete _messagesContainer;
        delete _messageModel;
        delete _messageDelegate;

        _ui = nullptr;
        _model = nullptr;
        _delegate = nullptr;
        _sideBarWidget = nullptr;
        _messagesContainer = nullptr;
        _messagesContainerLayout = nullptr;
        _messageModel = nullptr;
        _messageDelegate = nullptr;
    }

    void ChatPage::PreparePage() const noexcept
    {
        _ui->searchInput->clear();

        _model->SetAllAvailableChats();

        _ui->availableChatsList->setModel(_model);

        _ui->availableChatsList->setItemDelegate(_delegate);

        _sideBarWidget->UpdateUserName(currentUser.GetUserName());

        ToggleUiElements();
    }

    void ChatPage::ToggleSideMenu() const noexcept
    {
        auto* const animation = new (std::nothrow) QPropertyAnimation(_sideBarWidget, "geometry");
        animation->setDuration(1);

        if (_isSideBarVisible)
        {
            animation->setStartValue(QRect(0, 0, QWIDGETSIZE_MAX, this->height()));
            animation->setEndValue(
                QRect(-Widget::SideBarWidget::sideBarWidth, 0, QWIDGETSIZE_MAX, this->height()));
            _sideBarWidget->setVisible(false);
        }
        else
        {
            _sideBarWidget->setVisible(true);
            animation->setStartValue(
                QRect(-Widget::SideBarWidget::sideBarWidth, 0, QWIDGETSIZE_MAX, this->height()));
            animation->setEndValue(QRect(0, 0, QWIDGETSIZE_MAX, this->height()));
        }

        animation->start(QAbstractAnimation::DeleteWhenStopped);
        _isSideBarVisible = !_isSideBarVisible;
    }

    void ChatPage::OnSearchInputTextChanged() const noexcept
    {
        const QString userSearch = _ui->searchInput->text();
        if (userSearch.trimmed().isEmpty())
        {
            _model->SetAllAvailableChats();
            return;
        }

        _model->SetMatchingChats(userSearch.toStdString());
    }

    void ChatPage::OnChatSelected()
    {
        const QModelIndex index = _ui->availableChatsList->currentIndex();

        if (lastSelectedRow == index.row()) return;

        _ui->userName->setText(
            index.data(Model::AvailableChatsModel::AvailableChatsRole::kChatNameRole).toString());

        _ui->messageInput->clear();

        _ui->rightStack->setCurrentWidget(_ui->chatPage);

        _messageModel->GetAllMessages(
            index.data(Model::AvailableChatsModel::AvailableChatsRole::kChatIdRole).toInt(),
            currentUser);

        lastSelectedRow = index.row();

        _isChatPageVisible = true;
    }

    coroutine_void ChatPage::OnSendButtonPressed()
    {
        const auto lastMessageText = _ui->messageInput->toPlainText();

        if (lastMessageText.trimmed().isEmpty()) co_return;

        auto remainingText = lastMessageText;

        while (!remainingText.isEmpty())
        {
            const auto chunk = remainingText.left(Common::maxInputBufferSize - 1);
            remainingText = remainingText.mid(Common::maxInputBufferSize - 1);

            const MessageBuffer::MessageNode messageChunk(
                MessageBuffer::MessageStatus::kSend,
                chunk.toStdString(),
                QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss").toStdString());

            const QModelIndex index = _ui->availableChatsList->currentIndex();

            size_t chatId = index.data(Model::AvailableChatsModel::AvailableChatsRole::kChatIdRole)
                                .toUInt();

            if (chatId == ChatSystem::ChatInfo::chatUndefined)
            {
                const auto chatName = index
                                          .data(Model::AvailableChatsModel::AvailableChatsRole::
                                                    kChatNameRole)
                                          .toString()
                                          .toStdString();

                std::optional<size_t>
                    optionalChatId = co_await UserData::UserRepository::CreateNewPersonalChatAsync(
                        currentUser.GetUserId(),
                        chatName);

                _model->setData(index,
                                optionalChatId.value_or(ChatSystem::ChatInfo::chatUndefined),
                                Model::AvailableChatsModel::AvailableChatsRole::kChatIdRole);
            }

            if (chatId == ChatSystem::ChatInfo::chatUndefined) co_return;

            SendMessage(chatId, currentUser.GetUserId(), messageChunk.data.c_str());

            AddLastMessage(messageChunk);
        }

        QMetaObject::invokeMethod(
            this,
            [this] { _ui->messageInput->clear(); },
            Qt::QueuedConnection);
    }

    void ChatPage::resizeEvent(QResizeEvent* event)
    {
        QWidget::resizeEvent(event);

        _sideBarWidget->setFixedHeight(this->height());
    }

    void ChatPage::keyPressEvent(QKeyEvent* event)
    {
        if (event->key() == Qt::Key_Escape)
        {
            ToggleUiElements();
            return;
        }

        QWidget::keyPressEvent(event);
    }

    void ChatPage::mousePressEvent(QMouseEvent* event)
    {
        if (event->buttons() == Qt::BackButton)
        {
            ToggleUiElements();
            return;
        }

        QWidget::mousePressEvent(event);
    }

    bool ChatPage::eventFilter(QObject* watched, QEvent* event)
    {
        if (watched->inherits("QTextEdit") && event->type() == QEvent::KeyPress)
        {
            QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
            if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter)
            {
                if (keyEvent->modifiers() & Qt::ShiftModifier) return false;

                OnSendButtonPressed();
                return true;
            }
        }
        return QWidget::eventFilter(watched, event);
    }

    void ChatPage::ToggleUiElements() const
    {
        if (_isSideBarVisible)
        {
            ToggleSideMenu();
            return;
        }

        if (_isChatPageVisible)
        {
            _ui->rightStack->setCurrentWidget(_ui->emptyPage);
            _ui->availableChatsList->selectionModel()->clearSelection();
            lastSelectedRow = -1;
        }
    }

    void ChatPage::AddLastMessage(const MessageBuffer::MessageNode& message)
    {
        _messageModel->AddMessage(message);

        if (const QModelIndex index = _ui->availableChatsList->currentIndex(); index.isValid())
        {
            _model->setData(index,
                            message.data.c_str(),
                            Model::AvailableChatsModel::AvailableChatsRole::kLastMessageRole);
            _model->setData(
                index,
                message.sendTime.c_str(),
                Model::AvailableChatsModel::AvailableChatsRole::kLastMessageSendTimeRole);
            emit _model->dataChanged(index, index);
        }
    }

    void ChatPage::HandleReceivedMessage(const size_t chatId,
                                         const MessageBuffer::MessageNode& message)
    {
        if (!_model->IsChatInModel(chatId)) _model->SetAllAvailableChats();

        const QModelIndex currentIndex = _ui->availableChatsList->currentIndex();
        const size_t
            currentChatId = currentIndex
                                .data(Model::AvailableChatsModel::AvailableChatsRole::kChatIdRole)
                                .toUInt();

        if (currentChatId == chatId) _messageModel->AddMessage(message);

        for (int row = 0; row < _model->rowCount(); ++row)
        {
            QModelIndex index = _model->index(row, 0);

            const size_t receiverChatId = index
                                              .data(Model::AvailableChatsModel::AvailableChatsRole::
                                                        kChatIdRole)
                                              .toUInt();

            if (receiverChatId == chatId)
            {
                _model->setData(index,
                                message.data.c_str(),
                                Model::AvailableChatsModel::AvailableChatsRole::kLastMessageRole);

                _model->setData(
                    index,
                    message.sendTime.c_str(),
                    Model::AvailableChatsModel::AvailableChatsRole::kLastMessageSendTimeRole);

                emit _model->dataChanged(index, index);
            }
        }
    }

    void ChatPage::SendMessage(const size_t chatId,
                               const size_t senderUserId,
                               const char* const data)
    {
        if (!ClientNetworking::Client::GetInstance().has_value()) return;

        const auto clientInstance = ClientNetworking::Client::GetInstance().value();

        clientInstance.get().SendUserMessage(chatId, senderUserId, data);
    }
}   // namespace Gui
