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
        MessageNode(const MessageStatus messageType, std::string data)
            : data(std::move(data))
            , messageType(messageType)
        {}

        std::string data;
        MessageStatus messageType = MessageStatus::kUndefined;
    };
} // !namespace MessageBuffer

