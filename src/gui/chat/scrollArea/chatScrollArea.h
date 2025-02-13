#pragma once

#include <QWidget>

#include "messageBuffer/messageBuffer.h"

namespace Gui::ScrollArea
{
    QWidget* CreateMessage(const MessageBuffer::MessageNode& message);

    QWidget* CreateDateDivider(const QString& dateText);
}   // namespace Gui::ScrollArea