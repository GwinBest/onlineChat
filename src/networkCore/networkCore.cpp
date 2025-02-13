#include "networkCore.h"

#include <cstdlib>

namespace NetworkCore
{
    static std::string_view GetServerIpAddress()
    {
        return std::getenv("CHAT_SERVER_IP");
    }

    static uint32_t GetServerPort()
    {
        const char* const port = std::getenv("CHAT_SERVER_PORT");
        return port == nullptr ? 0 : static_cast<uint32_t>(std::atoi(port));
    }

    const std::string_view ipAddress = GetServerIpAddress();
    const uint32_t port = GetServerPort();
}   // namespace NetworkCore
