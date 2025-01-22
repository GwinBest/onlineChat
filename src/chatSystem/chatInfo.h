#pragma once

#include <QString>

namespace ChatSystem
{
    struct ChatInfo final
    {
        static constexpr size_t chatUndefined = 0;

        size_t id = chatUndefined;
        QString name;
        QString lastMessage;
        QString lastMessageSendTime;

        ChatInfo() noexcept = default;

        ChatInfo(const size_t id,
                 QString name,
                 QString lastMessage = "",
                 QString lastMessageSendTime = "") noexcept
            : id(id)
            , name(std::move(name))
            , lastMessage(std::move(lastMessage))
            , lastMessageSendTime(std::move(lastMessageSendTime))
        {}
    };
} // !namespace ChatSystem
