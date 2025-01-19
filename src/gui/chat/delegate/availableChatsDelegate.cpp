#include "availableChatsDelegate.h"

#include <QDateTime>
#include <QPainter>

#include "gui/chat/model/availableChatsModel.h"
#include "gui/colors/colors.h"

namespace Gui::Delegate
{
    void AvailableChatsDelegate::paint(QPainter* painter,
                                       const QStyleOptionViewItem& option,
                                       const QModelIndex& index) const
    {
        painter->save();

        const QRect rect = option.rect;
        QColor backgroundColor;
        if (option.state & QStyle::State_Selected)          backgroundColor = QColor(colorSelectedChat);
        else if (option.state & QStyle::State_MouseOver)    backgroundColor = QColor(colorOnHover);
        else                                                backgroundColor = QColor(colorGray);
        painter->fillRect(rect, backgroundColor);

        const QString name = index.data(Model::AvailableChatsModel::kChatNameRole).toString();
        const QString lastMessage = index.data(Model::AvailableChatsModel::kLastMessageRole).toString();
        const QString avatarPath = index.data(Qt::DecorationRole).toString();
        QString sendTime = index.data(Model::AvailableChatsModel::kLastMessageSendTimeRole).toString();

        const QDateTime dateTime = QDateTime::fromString(sendTime, "yyyy-MM-dd HH:mm:ss");
        sendTime = dateTime.time().toString("HH:mm");

        constexpr int avatarSize = 40;
        constexpr int margin = 10;
        const int centerY = rect.top() + (rect.height() - avatarSize) / 2;
        const QRect avatarRect(rect.left() + margin, centerY, avatarSize, avatarSize);

        if (const QPixmap avatarPixmap(avatarPath); 
            !avatarPixmap.isNull())
        {
            painter->drawPixmap(avatarRect, avatarPixmap.scaled(avatarSize,
                                                                avatarSize,
                                                                Qt::KeepAspectRatioByExpanding,
                                                                Qt::SmoothTransformation));
        }
        else
        {
            painter->setBrush(QBrush(QColor(colorLightGreen)));
            painter->setPen(Qt::NoPen);
            painter->drawEllipse(avatarRect);
        }

        QFont chatNameFont = painter->font();
        chatNameFont.setBold(true);
        painter->setFont(chatNameFont);
        painter->setPen(Qt::white);
        painter->drawText(QRect(avatarRect.right() + 10,
                                rect.top() + 10,
                                rect.width() - avatarRect.width() - 20,
                                20),
                          name);

        QFont messageFont = painter->font();
        messageFont.setBold(false);
        painter->setFont(messageFont);
        painter->setPen(Qt::white);
        painter->drawText(QRect(avatarRect.right() + 10,
                                rect.top() + 30,
                                rect.width() - avatarRect.width() - 70,
                                20),
                          lastMessage);

        painter->setPen(Qt::white);
        painter->drawText(QRect(rect.right() - 50,
                                rect.top() + 5,
                                40,
                                20),
                          Qt::AlignRight,
                          sendTime);

        painter->restore();
    }

    QSize AvailableChatsDelegate::sizeHint(const QStyleOptionViewItem& option,
                                           const QModelIndex& index) const
    {
        Q_UNUSED(option)
        Q_UNUSED(index)
        return { 0, 60 };
    }
} // !namespace Gui::Delegate