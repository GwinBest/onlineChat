#pragma once

#include <cstdint>
#include <string>

namespace MessageBuffer
{
    enum class MessageStatus : uint8_t
    {
        kUndefined = 0,
        kSend = 1,
        kReceived = 2
    };

    struct MessageNode final
    {
        MessageNode(const MessageStatus messageType, std::string data, std::string sendTime)
            : data(std::move(data))
            , sendTime(std::move(sendTime))
            , messageType(messageType)
        {}

        std::string data;
        std::string sendTime;
        MessageStatus messageType = MessageStatus::kUndefined;
    };
} // !namespace MessageBuffer

