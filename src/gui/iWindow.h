#pragma once

namespace Gui
{
    class IWindow
    {
    public:
        virtual ~IWindow() = default;

        virtual void DrawGui() noexcept = 0;

        static constexpr int32_t defaultDisplayWidth = 820;
        static constexpr int32_t defaultDisplayHeight = 700;
    };

} // !namespace Gui