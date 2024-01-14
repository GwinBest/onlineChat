#pragma once

namespace Gui
{
    class IWindow
    {
    public:
        virtual ~IWindow() = default;

        virtual void DrawGui() noexcept = 0;

        static constexpr int32_t _defaultDisplayWidth = 820;
        static constexpr int32_t _defaultDisplayHeight = 700;
    };

} // !namespace Gui