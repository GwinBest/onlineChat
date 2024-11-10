#include "chatPage.h"

#include "ui_chatPage.h"

namespace Gui
{
    ChatPage::ChatPage(QWidget* parent)
        : QWidget(parent)
        , ui(new Ui::ChatPage())
    {
        ui->setupUi(this);
    }

    ChatPage::~ChatPage()
    {
        delete ui;
    }
} // !namespace Gui