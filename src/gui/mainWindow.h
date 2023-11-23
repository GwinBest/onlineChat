#ifndef _ONLINECHAT_GUI_MAINWINDOW_H_
#define _ONLINECHAT_GUI_MAINWINDOW_H_

#include "imgui.h"
#include "imgui_stdlib.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <GLFW/glfw3.h> 

#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

namespace Gui
{
    //mainWindow is a parent window 
    class MainWindow
    {
    public:
        MainWindow() = default;
        virtual ~MainWindow();

        bool Init() noexcept;
        void Draw() noexcept;
        void Cleanup() noexcept;

    protected:
        // note: main window do not generate any controls
        virtual void GenerateControls() noexcept {};

    private:
        void SetupWindowStyle() const noexcept;
        void NewFrame() const noexcept;
        void Render() noexcept;
        
    protected:
        enum class WindowStatusCode : uint8_t
        {
            kWIndowUndefined = 0,
            kWIndowInited = 1,
            kWindowDeleted = 2
        };

        constexpr static int _defaultDisplayHeight = 700;
        constexpr static int _defaultDisplayWidth = 820;

        int _currentDisplayHeight = _defaultDisplayHeight;
        int _currentDisplayWidth = _defaultDisplayWidth;

        WindowStatusCode _windowStatus = WindowStatusCode::kWIndowUndefined;

    private:
        const char* _glslVersion = "#version 130";
        GLFWwindow* _mainWindow = nullptr;
        const ImVec4 _windowBackgroundColor = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    };

} // !namespace Gui

#endif // !_ONLINECHAT_GUI_MAINWINDOW_H_

