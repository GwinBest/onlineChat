#pragma once

#include <string>

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

        ChatInfo(const size_t id,
                 const std::string& name,
                 const std::string& lastMessage = "",
                 const std::string& lastMessageSendTime = "") noexcept
            : id(id)
            , name(name.data())
            , lastMessage(lastMessage.data())
            , lastMessageSendTime(lastMessageSendTime.data())
        {}
    };
} // !namespace ChatSystem
