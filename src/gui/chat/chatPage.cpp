#include "chatPage.h"

#include "ui_chatPage.h"

#include "gui/chat/delegate/availableChatsDelegate.h"
#include "gui/chat/model/availableChatsModel.h"

namespace Gui
{
    ChatPage::ChatPage(QWidget* parent)
        : QWidget(parent)
        , _ui(new Ui::ChatPage())
        , _model(new Model::AvailableChatsModel())
        , _delegate(new Delegate::AvailableChatsDelegate(_ui->availableChatsList))
    {
        _ui->setupUi(this);

        _ui->availableChatsList->setMouseTracking(true);
        _ui->availableChatsList->setUniformItemSizes(true);
        _ui->availableChatsList->setResizeMode(QListView::Adjust);

        connect(_ui->menuButton, &QPushButton::clicked, this, &ChatPage::OnMenuButtonPressed);

        connect(_ui->searchInput, &QLineEdit::textChanged, this, &ChatPage::OnSearchInputTextChanged);
    }

    ChatPage::~ChatPage()
    {
        delete _ui;
        delete _model;
        delete _delegate;
    }

    void ChatPage::PreparePage() noexcept
    {
        _model->SetAllAvailableChats();

        _ui->availableChatsList->setModel(_model);

        _ui->availableChatsList->setItemDelegate(_delegate);
    }

    void ChatPage::OnMenuButtonPressed() const noexcept
    {

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
} // !namespace Gui