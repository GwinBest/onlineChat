#include "chatScrollArea.h"

#include <QDateTime>
#include <QHBoxLayout>
#include <QLabel>
#include <QSpacerItem>
#include <QVBoxLayout>

namespace Gui::ScrollArea
{
    QWidget* CreateMessage(const MessageBuffer::MessageNode& message)
    {
        auto* messageWidget = new QWidget();
        auto* layout = new QHBoxLayout(messageWidget);

        const QDateTime dateTime = QDateTime::fromString(message.sendTime.c_str(), "yyyy-MM-dd HH:mm:ss");
        const QString sendTime = dateTime.time().toString("HH:mm");

        const auto content = QString(
            "<div style='font-size: 14px;'>%1</div>"
            "<div style='color: gray; font-size: 10px; text-align: right;'>%2</div>")
            .arg(message.data.c_str())
            .arg(sendTime);

        auto* messageLabel = new QLabel(content);
        messageLabel->setWordWrap(true);
        messageLabel->setStyleSheet(message.messageType == MessageBuffer::MessageStatus::kSend
                                    ? "background-color: #2a2f33; border-radius: 10px; padding: 8px;"
                                    : "background-color: #33393f; border-radius: 10px; padding: 8px;");

        if (message.messageType == MessageBuffer::MessageStatus::kSend)
        {
            layout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));
            layout->addWidget(messageLabel);
        }
        else
        {
            layout->addWidget(messageLabel);
            layout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));
        }

        layout->setContentsMargins(0, 0, 0, 0);
        return messageWidget;
    }

    QWidget* CreateDateDivider(const QString& dateText)
    {
        QWidget* dividerWidget = new QWidget();
        QHBoxLayout* layout = new QHBoxLayout(dividerWidget);

        QFrame* leftLine = new QFrame();
        leftLine->setFrameShape(QFrame::HLine);
        leftLine->setFrameShadow(QFrame::Sunken);
        leftLine->setStyleSheet("color: gray;");

        QLabel* dateLabel = new QLabel(dateText);
        dateLabel->setAlignment(Qt::AlignCenter);
        dateLabel->setStyleSheet(
            "color: white;"
            "background-color: #2f343b;"
            "border-radius: 12px;"
            "padding: 4px 12px;"
            "font-size: 12px;"
            "font-weight: bold;"
        );

        QFrame* rightLine = new QFrame();
        rightLine->setFrameShape(QFrame::HLine);
        rightLine->setFrameShadow(QFrame::Sunken);
        rightLine->setStyleSheet("color: gray;");

        layout->addWidget(leftLine);
        layout->addWidget(dateLabel);
        layout->addWidget(rightLine);
        layout->setContentsMargins(0, 0, 0, 0);

        return dividerWidget;
    }
} // !namespace ScrollArea
