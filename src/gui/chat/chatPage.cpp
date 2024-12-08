#include "chatPage.h"

#include "ui_chatPage.h"

#include <QKeyEvent>
#include <QPropertyAnimation>

#include "gui/chat/delegate/availableChatsDelegate.h"
#include "gui/chat/model/availableChatsModel.h"
#include "gui/chat/widget/sideBarWidget.h"
#include "userData/user.h"

extern UserData::User currentUser;

namespace Gui
{
    ChatPage::ChatPage(QWidget* parent)
        : QWidget(parent)
        , _ui(new Ui::ChatPage())
        , _model(new Model::AvailableChatsModel())
        , _delegate(new Delegate::AvailableChatsDelegate(_ui->availableChatsList))
    {
        _ui->setupUi(this);

        _sideBarWidget = new Widget::SideBarWidget(this);
        _sideBarWidget->setVisible(false);

        _ui->availableChatsList->setMouseTracking(true);
        _ui->availableChatsList->setUniformItemSizes(true);
        _ui->availableChatsList->setResizeMode(QListView::Adjust);

        connect(_sideBarWidget, &Widget::SideBarWidget::HideSideBar, this, [this] { ToggleSideMenu(); });
        connect(_sideBarWidget, &Widget::SideBarWidget::LogOutButtonPressed, this, [this]
                {
                    emit LogOutButtonPressed();
                    ToggleSideMenu();
                });

        connect(_ui->menuButton, &QPushButton::clicked, this, &ChatPage::ToggleSideMenu);

        connect(_ui->searchInput, &QLineEdit::textChanged, this, &ChatPage::OnSearchInputTextChanged);

        connect(_ui->availableChatsList, &QListView::clicked, this, &ChatPage::OnChatSelected);
    }

    ChatPage::~ChatPage()
    {
        delete _ui;
        delete _model;
        delete _delegate;
        delete _sideBarWidget;
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
        const auto animation = new(std::nothrow) QPropertyAnimation(_sideBarWidget, "geometry");
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

    void ChatPage::OnChatSelected() const noexcept
    {
        const QModelIndex index = _ui->availableChatsList->currentIndex();

        _ui->userName->setText(index.data(Model::AvailableChatsModel::AvailableChatsRole::kChatNameRole).toString());
        _ui->messageInput->clear();

        _ui->rightStack->setCurrentWidget(_ui->chatPage);
        _isChatPageVisible = true;
    }

    void ChatPage::resizeEvent(QResizeEvent* event)
    {
        QWidget::resizeEvent(event);

        _sideBarWidget->setFixedHeight(this->height());
    }

    void ChatPage::keyPressEvent(QKeyEvent* event)
    {
        if (event->key() == Qt::Key_Escape) ToggleUiElements();

        QWidget::keyPressEvent(event);
    }

    void ChatPage::mousePressEvent(QMouseEvent* event)
    {
        if (event->buttons() == Qt::BackButton) ToggleUiElements();

        QWidget::mousePressEvent(event);
    }

    inline void ChatPage::ToggleUiElements() const
    {
        if (_isSideBarVisible) ToggleSideMenu();
        if (_isChatPageVisible) _ui->rightStack->setCurrentWidget(_ui->emptyPage);
    }
} // !namespace Gui
