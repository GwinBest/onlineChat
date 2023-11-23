#include "mainWindow.h"

namespace Gui
{
    bool MainWindow::Init() noexcept
    {
        if (!glfwInit())
            return false;

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

        _mainWindow = glfwCreateWindow(_defaultDisplayWidth, _defaultDisplayHeight, "Online Chat", nullptr, nullptr);
        if (_mainWindow == nullptr)
            return false;

        glfwMakeContextCurrent(_mainWindow);
        glfwSwapInterval(false);                                                                                    // disable vsync

        // setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;		                                                // enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;			                                                // enable Docking
        io.Fonts->AddFontFromFileTTF("OpenSans-Regular.ttf", 22, 0, io.Fonts->GetGlyphRangesCyrillic());

        SetupWindowStyle();
        
        ImGui_ImplGlfw_InitForOpenGL(_mainWindow, true);
        ImGui_ImplOpenGL3_Init(_glslVersion);

        _windowStatus = WindowStatusCode::kWIndowInited;

        return true;
    }

    void MainWindow::Draw() noexcept
    {
        if (_windowStatus != WindowStatusCode::kWIndowInited)
            return;

        while (!glfwWindowShouldClose(_mainWindow))
        {
            NewFrame();
            GenerateControls();
            Render();
        }
    }

    void MainWindow::Cleanup() noexcept
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        glfwDestroyWindow(_mainWindow);
        glfwTerminate();

        _windowStatus = WindowStatusCode::kWindowDeleted;
    }

    MainWindow::~MainWindow()
    {
        if (_windowStatus != WindowStatusCode::kWindowDeleted)
            MainWindow::Cleanup();
    }

    void MainWindow::SetupWindowStyle() const noexcept
    {
        ImGuiStyle& windowStyle = ImGui::GetStyle();
        windowStyle.ScrollbarSize = 10;
        windowStyle.WindowPadding.x = 0;
        windowStyle.WindowPadding.y = 0;

        windowStyle.Colors[ImGuiCol_WindowBg] = ImVec4(0.0941f, 0.0980f, 0.1137f, 1.00f);
        windowStyle.Colors[ImGuiCol_ChildBg] = ImVec4(0.1608f, 0.1804f, 0.2039f, 1.00f);
        windowStyle.Colors[ImGuiCol_FrameBg] = ImVec4(0.1608f, 0.1804f, 0.2039f, 1.00f);
        windowStyle.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.1608f, 0.1804f, 0.2039f, 1.00f);
        windowStyle.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.1608f, 0.1804f, 0.2039f, 1.00f);
        windowStyle.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
        windowStyle.Colors[ImGuiCol_Tab] = ImVec4(1.20f, 1.25f, 0.30f, 1.60f);
        windowStyle.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.55f, 0.53f, 0.55f, 0.51f);
        windowStyle.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.56f, 1.00f);
        windowStyle.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.91f);
        windowStyle.Colors[ImGuiCol_Header] = ImVec4(0.0f, 0.5882f, 0.5294f, 1.00f);
        windowStyle.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.2039f, 0.2235f, 0.2471f, 1.0f);
        windowStyle.Colors[ImGuiCol_HeaderActive] = ImVec4(0.0f, 0.5882f, 0.5294f, 1.00f);
    }

    void MainWindow::NewFrame() const noexcept
    {
        if (_windowStatus != WindowStatusCode::kWIndowInited)
            return;

        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void MainWindow::Render() noexcept
    {
        if (_windowStatus != WindowStatusCode::kWIndowInited)
            return;

        ImGui::Render();
        glfwGetFramebufferSize(_mainWindow, &_currentDisplayWidth, &_currentDisplayHeight);
        glViewport(0, 0, _currentDisplayWidth, _currentDisplayHeight);

        glClearColor(_windowBackgroundColor.x * _windowBackgroundColor.w,								            // red  
                     _windowBackgroundColor.y * _windowBackgroundColor.w,								            // green							
                     _windowBackgroundColor.z * _windowBackgroundColor.w,							                // blue 
                     _windowBackgroundColor.w);																        // alpha

        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(_mainWindow);
    }

} // !namespace Gui
