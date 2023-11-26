#pragma once

#include <memory>
#include <vector>

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <GLFW/glfw3.h> 

#include "iWindow.h"
#include "iImGuiWindow.h"

#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

namespace Gui
{
    class GlfwWindow final : public IWindow
    {
    public:
        GlfwWindow();
        ~GlfwWindow();

        bool Init() noexcept;
        void Draw() noexcept;
        void Cleanup() noexcept;

        void PushWindow(std::unique_ptr<IImGuiWindow>&& window) noexcept;
        void PopWindow() noexcept;

        GLFWwindow* GetGlfwWindow() const noexcept;

    private:
        void SetupWindowStyle() const noexcept;

        void NewFrame() const noexcept;
        void GenerateControls() noexcept override;
        void Render() noexcept;

    private:
        const char* _glslVersion = "#version 130";
        GLFWwindow* _mainWindow = nullptr;
        const ImVec4 _windowBackgroundColor = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        std::vector<std::unique_ptr<IImGuiWindow>> _windowArray;

        int32_t _currentDisplayHeight = _defaultDisplayHeight;
        int32_t _currentDisplayWidth = _defaultDisplayWidth;

        WindowState _windowState = WindowState::kWIndowUndefined;
    };

} // !namespace Gui



