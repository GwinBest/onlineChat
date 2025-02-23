#include "messagesDelegate.h"

#include <QApplication>
#include <QCache>
#include <QDateTime>
#include <QPainter>
#include <QTextOption>

#include "gui/colors/colors.h"
#include "messageBuffer/messageBuffer.h"

namespace Gui::Delegate
{
    static constinit int padding = 10;
    static constinit int bubbleMargin = 12;
    static constinit int messageSpacing = 20;
    static constinit int minWidth = 50;
    static constinit int maxWidth = 350;
    static constinit int roundRadius = 15;
    static constinit int timePadding = 5;
    static constinit int dateHeight = 30;
    static constinit int dateMargin = 7;

    static QCache<QString, QSize> textSizeCache;

    static QSize GetTextSize(const QString& text, const QFont& font)
    {
        if (textSizeCache.contains(text)) return *textSizeCache.object(text);

        const QFontMetrics fm(font);
        const QRect rect = fm.boundingRect(QRect(0, 0, maxWidth - 2 * padding, 0),
                                           Qt::TextWrapAnywhere | Qt::AlignLeft,
                                           text);

        QSize size = rect.size();
        textSizeCache.insert(text, new QSize(size));

        return size;
    }

    void MessageDelegate::paint(QPainter* painter,
                                const QStyleOptionViewItem& option,
                                const QModelIndex& index) const
    {
        if (!index.isValid()) return;

        painter->save();

        const auto message = index.data(Qt::DisplayRole).value<MessageBuffer::MessageNode>();
        const bool isSent = (message.messageType == MessageBuffer::MessageStatus::kSend);
        const QString text = QString::fromStdString(message.data);

        const auto dateTime = QDateTime::fromString(message.sendTime.c_str(),
                                                    "yyyy-MM-dd HH:mm:ss");
        const QString sendTime = dateTime.time().toString("HH:mm");

        const QSize textSize = GetTextSize(text, painter->font());

        int availableWidth = option.rect.width() - 2 * bubbleMargin;
        const int bubbleWidth = qBound(minWidth,
                                       textSize.width() + 2 * padding,
                                       qMin(maxWidth, availableWidth));

        QString prevDate;
        if (index.row() > 0)
        {
            const auto prevMessage = index.model()
                                         ->data(index.model()->index(index.row() - 1, 0))
                                         .value<MessageBuffer::MessageNode>();
            prevDate = QDateTime::fromString(prevMessage.sendTime.c_str(), "yyyy-MM-dd HH:mm:ss")
                           .date()
                           .toString("MMMM dd");
        }

        const QString currentDate = dateTime.date().toString("MMMM dd");
        bool isNewDate = (currentDate != prevDate);
        if (isNewDate)
        {
            const QSize dateTextSize = GetTextSize(currentDate, painter->font());
            const int dateXPos = option.rect.left()
                               + (option.rect.width() - dateTextSize.width()) / 2;

            const QRect dateRect(dateXPos,
                                 option.rect.top() + dateMargin,
                                 dateTextSize.width() + 20,
                                 dateHeight);

            // Date drawing
            painter->setPen(Qt::NoPen);
            painter->setBrush(QColor(colorDateDivider));
            painter->drawRoundedRect(dateRect, roundRadius, roundRadius);

            painter->setPen(Qt::white);
            painter->drawText(dateRect, Qt::AlignCenter, currentDate);
        }

        int yOffset = (isNewDate) ? dateHeight + 2 * dateMargin : 0;

        const int timeWidth = QFontMetrics(painter->font()).horizontalAdvance(sendTime);
        const int timeHeight = QFontMetrics(painter->font()).height();
        const int xPos = isSent ? option.rect.right() - bubbleWidth - bubbleMargin - timeWidth
                                : option.rect.left() + bubbleMargin;

        const QRect bubbleRect(xPos,
                               option.rect.top() + yOffset,
                               bubbleWidth + timeWidth,
                               textSize.height() + 2 * padding);

        // Rectangle drawing for message bubble
        painter->setPen(Qt::NoPen);
        painter->setBrush(isSent ? QColor(colorMessageSend) : QColor(colorMessageReceived));
        painter->drawRoundedRect(bubbleRect, roundRadius, roundRadius);

        // Time drawing
        const int timeX = bubbleRect.right() - padding - timeWidth + timePadding;
        const int timeY = bubbleRect.bottom() - timeHeight - timePadding;
        painter->setPen(QColor(colorMessageTimeSend));
        painter->drawText(QRect(timeX, timeY, timeWidth, timeHeight), Qt::AlignRight, sendTime);

        // Message drawing
        painter->setPen(colorWhite);
        const QRect textRect = bubbleRect.adjusted(padding, padding, -padding, -padding);
        painter->drawText(textRect, Qt::TextWrapAnywhere | Qt::AlignLeft, text);

        painter->restore();
    }

    QSize MessageDelegate::sizeHint(const QStyleOptionViewItem& option,
                                    const QModelIndex& index) const
    {
        if (!index.isValid()) return {0, messageSpacing};

        const auto message = index.data(Qt::DisplayRole).value<MessageBuffer::MessageNode>();
        const QString text = QString::fromStdString(message.data);

        const QSize textSize = GetTextSize(text, QApplication::font());

        int totalHeight = textSize.height() + 2 * padding + messageSpacing;

        QString prevDate;
        if (index.row() > 0)
        {
            const auto prevMessage = index.model()
                                         ->data(index.model()->index(index.row() - 1, 0))
                                         .value<MessageBuffer::MessageNode>();
            prevDate = QDateTime::fromString(prevMessage.sendTime.c_str(), "yyyy-MM-dd HH:mm:ss")
                           .date()
                           .toString("MMMM dd");
        }

        const QString currentDate = QDateTime::fromString(message.sendTime.c_str(),
                                                          "yyyy-MM-dd HH:mm:ss")
                                        .date()
                                        .toString("MMMM dd");

        if (currentDate != prevDate) totalHeight += dateHeight + 2 * dateMargin;

        return {option.rect.width(), totalHeight};
    }

}   // namespace Gui::Delegate
