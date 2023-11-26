#pragma once

namespace Gui
{
    class IWindow
    {
    public:
        virtual ~IWindow() = default;

        virtual void GenerateControls() noexcept = 0;

        enum class WindowState : uint8_t
        {
            kWIndowUndefined    = 0,
            kWIndowInited       = 1,
            kWindowDeleted      = 2
        };

        static constexpr int32_t _defaultDisplayWidth = 820;
        static constexpr int32_t _defaultDisplayHeight = 700;
    };

} // !namespace Gui