#pragma once

#include <QWidget>

#include "messageBuffer/messageBuffer.h"

namespace Gui::ScrollArea
{
    QWidget* CreateMessage(const MessageBuffer::MessageNode& message);
} // !namespace Gui::ScrollArea