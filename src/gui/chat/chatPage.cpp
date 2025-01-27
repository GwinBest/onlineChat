#include "chatPage.h"

#include "ui_chatPage.h"

#include <QDateTime>
#include <QKeyEvent>
#include <QPropertyAnimation>
#include <QRegularExpression>

#include "client/client.h"
#include "common/common.h"
#include "gui/chat/delegate/availableChatsDelegate.h"
#include "gui/chat/model/availableChatsModel.h"
#include "gui/chat/scrollArea/chatScrollArea.h"
#include "gui/chat/widget/sideBarWidget.h"
#include "userData/user.h"
#include "userData/userRepository.h"

#ifdef SendMessage
#undef SendMessage
#endif // SendMessage

#ifdef GetUserName
#undef GetUserName
#endif // GetUserName

extern UserData::User currentUser;

namespace Gui
{
    using namespace CoroutineUtils;

    ChatPage::ChatPage(QWidget *parent)
        : QWidget(parent), _ui(new Ui::ChatPage()), _model(new Model::AvailableChatsModel()), _delegate(new Delegate::AvailableChatsDelegate(_ui->availableChatsList)), _messagesContainer(new QWidget())
    {
        _ui->setupUi(this);

        _sideBarWidget = new Widget::SideBarWidget(this);
        _sideBarWidget->setVisible(false);

        _messagesContainerLayout = new QVBoxLayout(_messagesContainer);
        _messagesContainer->setLayout(_messagesContainerLayout);
        _ui->scrollArea->setWidget(_messagesContainer);

        _ui->availableChatsList->setMouseTracking(true);
        _ui->availableChatsList->setUniformItemSizes(true);
        _ui->availableChatsList->setResizeMode(QListView::Adjust);

        _ui->messageInput->installEventFilter(this);

        connect(_sideBarWidget, &Widget::SideBarWidget::HideSideBar, this, &ChatPage::ToggleSideMenu);
        connect(_sideBarWidget, &Widget::SideBarWidget::LogOutButtonPressed, this, [this]
                {
                    emit LogOutButtonPressed();
                    ToggleSideMenu(); });

        connect(_ui->menuButton, &QPushButton::clicked, this, &ChatPage::ToggleSideMenu);

        connect(_ui->searchInput, &QLineEdit::textChanged, this, &ChatPage::OnSearchInputTextChanged);

        connect(_ui->availableChatsList, &QListView::clicked, this, &ChatPage::OnChatSelected);

        connect(_ui->sendMessageButton, &QPushButton::clicked, this, &ChatPage::OnSendButtonPressed);

        if (const auto clientInstance = ClientNetworking::Client::GetInstance();
            clientInstance.has_value())
        {
            clientInstance
                .value()
                .get()
                .RegisterReceiveMessageCallback([this](const MessageBuffer::MessageNode &message)
                                                { QMetaObject::invokeMethod(this, [this, message]
                                                                            { RenderLastMessage(message); }, Qt::QueuedConnection); });
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

        _ui = nullptr;
        _model = nullptr;
        _delegate = nullptr;
        _sideBarWidget = nullptr;
        _messagesContainer = nullptr;
        _messagesContainerLayout = nullptr;
    }

    void ChatPage::PreparePage() const noexcept
    {
        _model->SetAllAvailableChats();

        _ui->availableChatsList->setModel(_model);

        _ui->availableChatsList->setItemDelegate(_delegate);

        _sideBarWidget->UpdateUserName(currentUser.GetUserName());
    }

    void ChatPage::ToggleSideMenu() const noexcept
    {
        auto *const animation = new (std::nothrow) QPropertyAnimation(_sideBarWidget, "geometry");
        animation->setDuration(1);

        if (_isSideBarVisible)
        {
            animation->setStartValue(QRect(0, 0, QWIDGETSIZE_MAX, this->height()));
            animation->setEndValue(QRect(-Widget::SideBarWidget::sideBarWidth, 0,
                                         QWIDGETSIZE_MAX, this->height()));
            _sideBarWidget->setVisible(false);
        }
        else
        {
            _sideBarWidget->setVisible(true);
            animation->setStartValue(QRect(-Widget::SideBarWidget::sideBarWidth, 0,
                                           QWIDGETSIZE_MAX, this->height()));
            animation->setEndValue(QRect(0, 0,
                                         QWIDGETSIZE_MAX, this->height()));
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

        if (lastSelectedRow == index.row())
            return;

        _ui->userName->setText(index.data(
                                        Model::AvailableChatsModel::AvailableChatsRole::kChatNameRole)
                                   .toString());

        _ui->messageInput->clear();

        _ui->rightStack->setCurrentWidget(_ui->chatPage);

        FillMessageContainerLayout(index.data(
                                            Model::AvailableChatsModel::AvailableChatsRole::kChatIdRole)
                                       .toInt());

        lastSelectedRow = index.row();

        _isChatPageVisible = true;
    }

    coroutine_void ChatPage::OnSendButtonPressed()
    {
        const auto lastMessageText = _ui->messageInput->toPlainText();

        if (lastMessageText.trimmed().isEmpty())
            co_return;

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

            size_t chatId = index.data(Model::AvailableChatsModel::AvailableChatsRole::kChatIdRole).toUInt();

            if (chatId == ChatSystem::ChatInfo::chatUndefined)
            {
                std::optional<size_t> optionalChatId = co_await UserData::UserRepository::CreateNewPersonalChatAsync(
                    currentUser.GetUserId(),
                    index.data(Model::AvailableChatsModel::AvailableChatsRole::kChatNameRole).toString().toStdString());

                _model->setData(index,
                                optionalChatId.value_or(ChatSystem::ChatInfo::chatUndefined),
                                Model::AvailableChatsModel::AvailableChatsRole::kChatIdRole);
            }

            if (chatId == ChatSystem::ChatInfo::chatUndefined)
                co_return;

            SendMessage(chatId,
                        currentUser.GetUserId(), messageChunk.data.c_str());
            RenderLastMessage(messageChunk);
        }

        QMetaObject::invokeMethod(this, [this]
                                  { _ui->messageInput->clear(); }, Qt::QueuedConnection);
    }

    void ChatPage::resizeEvent(QResizeEvent *event)
    {
        QWidget::resizeEvent(event);

        _sideBarWidget->setFixedHeight(this->height());
    }

    void ChatPage::keyPressEvent(QKeyEvent *event)
    {
        if (event->key() == Qt::Key_Escape)
        {
            ToggleUiElements();
            return;
        }

        QWidget::keyPressEvent(event);
    }

    void ChatPage::mousePressEvent(QMouseEvent *event)
    {
        if (event->buttons() == Qt::BackButton)
        {
            ToggleUiElements();
            return;
        }

        QWidget::mousePressEvent(event);
    }

    bool ChatPage::eventFilter(QObject *watched, QEvent *event)
    {
        if (watched->inherits("QTextEdit") && event->type() == QEvent::KeyPress)
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter)
            {
                if (keyEvent->modifiers() & Qt::ShiftModifier)
                    return false;

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

    coroutine_void ChatPage::FillMessageContainerLayout(const size_t chatId)
    {
        ClearMessageLayout();

        if (chatId == 0)
            co_return;

        const std::optional<std::vector<MessageBuffer::MessageNode>> chatMessage =
            co_await UserData::UserRepository::GetAvailableChatMessagesAsync(
                currentUser.GetUserId(), chatId);

        if (!chatMessage.has_value())
            co_return;

        QDateTime previousDateTime = {};
        const QLocale locale = QLocale::English;

        for (const auto &message : chatMessage.value())
        {
            const QDateTime currentDateTime = QDateTime::fromString(message.sendTime.c_str(),
                                                                    "yyyy-MM-dd HH:mm:ss");
            if (currentDateTime.date() != previousDateTime.date())
            {
                const QString currentSendTime = locale.toString(currentDateTime, "MMMM dd, yyyy");
                QMetaObject::invokeMethod(this, [this, currentSendTime]
                                          { _messagesContainerLayout->addWidget(ScrollArea::CreateDateDivider(currentSendTime)); }, Qt::QueuedConnection);
            }

            QMetaObject::invokeMethod(this, [this, message]
                                      { _messagesContainerLayout->addWidget(ScrollArea::CreateMessage(message)); }, Qt::QueuedConnection);

            previousDateTime = currentDateTime;
        }

        QMetaObject::invokeMethod(this, [this]
                                  { _messagesContainerLayout->addSpacerItem(
                                        new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding)); }, Qt::QueuedConnection);
    }

    void ChatPage::ClearMessageLayout() const
    {
        while (_messagesContainerLayout->count() > 0)
        {
            const QLayoutItem *item = _messagesContainerLayout->takeAt(0);
            delete item->widget();
            delete item;
        }
    }

    void ChatPage::RemoveLastSpacerItem(QVBoxLayout *layout)
    {
        if (const int count = layout->count();
            count > 0)
        {
            const QLayoutItem *item = layout->takeAt(count - 1);
            delete item;
        }
    }

    QString ChatPage::ExtractDateTimeFromMessageWidget(const QWidget *messageWidget)
    {
        const auto *label = messageWidget->findChild<QLabel *>();
        if (label == nullptr)
            return {};

        const QString htmlContent = label->text();
        const QRegularExpression regexTime("<div style='color: #[0-9A-Fa-f]{6};font-size: 10px; text-align: right;'>(\\d{2}:\\d{2})</div>");
        const QRegularExpression regexDate("<div style='font-size: 14px;' data-date='(\\d{4}-\\d{2}-\\d{2})'>");

        const QRegularExpressionMatch matchTime = regexTime.match(htmlContent);
        const QRegularExpressionMatch matchDate = regexDate.match(htmlContent);

        QString extractedTime;
        QString extractedDate;

        if (matchTime.hasMatch())
            extractedTime = matchTime.captured(1);
        if (matchDate.hasMatch())
            extractedDate = matchDate.captured(1);

        return QString("%1 %2").arg(extractedDate).arg(extractedTime);
    }

    void ChatPage::SendMessage(const size_t chatId, const size_t senderUserId, const char *const data)
    {
        if (!ClientNetworking::Client::GetInstance().has_value())
            return;

        const auto clientInstance = ClientNetworking::Client::GetInstance().value();

        clientInstance.get().SendUserMessage(chatId, senderUserId, data);
    }

    void ChatPage::RenderLastMessage(const MessageBuffer::MessageNode &message) const
    {
        RemoveLastSpacerItem(_messagesContainerLayout);

        if (const int messageContainerSize = _messagesContainerLayout->count() - 1;
            messageContainerSize > 0)
        {
            const auto *lastRenderedMessage = _messagesContainerLayout->itemAt(messageContainerSize)->widget();

            const auto lastRenderedMessageSendDate = ExtractDateTimeFromMessageWidget(lastRenderedMessage);
            const auto lastDateTime = QDateTime::fromString(lastRenderedMessageSendDate,
                                                            "yyyy-MM-dd HH:mm");
            const auto currentDateTime = QDateTime::fromString(message.sendTime.c_str(),
                                                               "yyyy-MM-dd HH:mm:ss");

            if (lastDateTime.date() != currentDateTime.date())
            {
                const QLocale locale = QLocale::English;
                const QString currentSendTime = locale.toString(currentDateTime, "MMMM dd, yyyy");
                _messagesContainerLayout->addWidget(ScrollArea::CreateDateDivider(currentSendTime));
            }

            _messagesContainerLayout->addWidget(ScrollArea::CreateMessage(message));
            _messagesContainerLayout->addSpacerItem(
                new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));
        }
        else
        {
            const QLocale locale = QLocale::English;
            const auto currentDateTime = QDateTime::fromString(message.sendTime.c_str(),
                                                               "yyyy-MM-dd HH:mm:ss");
            const QString currentSendTime = locale.toString(currentDateTime, "MMMM dd, yyyy");

            _messagesContainerLayout->addWidget(ScrollArea::CreateDateDivider(currentSendTime));
            _messagesContainerLayout->addWidget(ScrollArea::CreateMessage(message));
            _messagesContainerLayout->addSpacerItem(
                new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));
        }

        if (const QModelIndex index = _ui->availableChatsList->currentIndex();
            index.isValid())
        {
            _model->setData(index, message.data.c_str(),
                            Model::AvailableChatsModel::AvailableChatsRole::kLastMessageRole);
            _model->setData(index, message.sendTime.c_str(),
                            Model::AvailableChatsModel::AvailableChatsRole::kLastMessageSendTimeRole);
            emit _model->dataChanged(index, index);
        }
    }
} // !namespace Gui
