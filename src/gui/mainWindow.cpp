#include "mainWindow.h"

namespace Gui
{
    bool MainWindow::Init() noexcept
    {
        if (!glfwInit())
            return false;

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

        this->_mainWindow = glfwCreateWindow(this->_defaultDisplayWidth, this->_defaultDisplayHeight, "Online Chat", nullptr, nullptr);
        if (this->_mainWindow == nullptr)
            return false;

        glfwMakeContextCurrent(this->_mainWindow);
        glfwSwapInterval(false);                                                                                    // disable vsync

        // setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;		                                                // enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;			                                                // enable Docking

        // setup window style 
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

        ImGui_ImplGlfw_InitForOpenGL(this->_mainWindow, true);
        ImGui_ImplOpenGL3_Init(this->_glslVersion);

        this->_windowStatus = WindowStatusCode::kWIndowInited;

        return true;
    }

    void MainWindow::Draw() noexcept
    {
        if (this->_windowStatus != WindowStatusCode::kWIndowInited)
            return;

        while (!glfwWindowShouldClose(this->_mainWindow))
        {
            MainWindow::NewFrame();
            MainWindow::GenerateControls();
            MainWindow::Render();
        }
    }

    void MainWindow::NewFrame() const noexcept
    {
        if (this->_windowStatus != WindowStatusCode::kWIndowInited)
            return;

        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void MainWindow::GenerateControls() noexcept
    {
        if (this->_windowStatus != WindowStatusCode::kWIndowInited)
            return;

        ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_NoTabBar);				        // enable docking 
       
        ImGui::Begin("##main window");

        // available chats
        static uint32_t chatSelected = -1;
        static constexpr size_t availableChatsStartHeight = 65;
        static constexpr size_t availableChatsWidthScaleFactor = 3.0f;
        size_t availableChatsWidth = ImGui::GetWindowWidth() / availableChatsWidthScaleFactor;
        {
            ImVec2 availableChatsSize;
            if (ImGui::GetWindowWidth() < this->_defaultDisplayWidth)
            {
                availableChatsSize = ImVec2(ImGui::GetWindowWidth(), 0);
            }
            else
            {
                availableChatsSize = ImVec2(availableChatsWidth, 0);
            }

            ImGui::SetCursorPosY(availableChatsStartHeight);
            ImGui::BeginChild("##available chats", availableChatsSize);

            for (int i = 0; i < 100; i++)
            {
                char label[128];
                sprintf(label, "User %d", i);
                if (ImGui::Selectable(label, chatSelected == i))
                    chatSelected = i;
            }

            // unselect current chat
            if (ImGui::IsKeyPressed(ImGuiKey_Escape))
            {
                chatSelected = -1;
            }

            ImGui::EndChild();
        }

        // chat zone
        if (ImGui::GetWindowWidth() >= this->_defaultDisplayWidth && chatSelected != -1)
        {
            // input text
            static bool isEnterPressed = false;
            static bool reclaimFocus = false;
            {
                float oldFontScale = ImGui::GetFont()->Scale;
                ImGui::GetFont()->Scale *= 1.5f;																	// set new font scale for input text
                ImGui::PushFont(ImGui::GetFont());

                ImVec2 inputTextSize = ImVec2(ImGui::GetWindowWidth() - availableChatsWidth - 60, 45);
                ImGui::SetCursorPos(ImVec2(availableChatsWidth, ImGui::GetWindowHeight() - 45));
                if (ImGui::InputTextMultilineWithHint("##input", "Write a message", &(this->_inputBuffer), inputTextSize, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CtrlEnterForNewLine))
                {
                    isEnterPressed = true;
                    reclaimFocus = true;
                }

                ImGui::GetFont()->Scale = oldFontScale;																// set old font scale
                ImGui::PopFont();

                // auto-focus on window apparition
                ImGui::SetItemDefaultFocus();
                if (reclaimFocus)
                {
                    ImGui::SetKeyboardFocusHere(-1);																// auto focus input text
                    reclaimFocus = false;
                }
            }

            // button
            static bool isButtonPressed = false;
            {
                ImVec2 buttonSize = ImVec2(35, 40);
                ImGui::SameLine();
                ImGui::SetCursorPosY(ImGui::GetWindowHeight() - 45);
                if (ImGui::Button("send", buttonSize))
                {
                    isButtonPressed = true;
                    reclaimFocus = true;
                }
            }

            // send message 
            if ((isEnterPressed || isButtonPressed) && this->_inputBuffer != "")
            {
                Network::Client::GetInstance().Send(chatSelected, this->_inputBuffer.c_str(), this->_inputBuffer.size());
                Buffer::MessageBuffer::getInstance().pushFront(Buffer::MessageType::kSend, this->_inputBuffer.c_str());

                isEnterPressed = false;
                isButtonPressed = false;

                reclaimFocus = true;

                this->_inputBuffer = "";
            }

            // display sent and received messages
            {
                ImGuiStyle& windowStyle = ImGui::GetStyle();
                windowStyle.Colors[ImGuiCol_ChildBg] = ImVec4(0.0941f, 0.0980f, 0.1137f, 1.00f);					// setup new color for begin child

                ImGui::SetCursorPos(ImVec2(availableChatsWidth, availableChatsStartHeight));
                ImGui::BeginChild("##chat zone", ImVec2(ImGui::GetWindowWidth() - availableChatsWidth,
                                  ImGui::GetWindowHeight() - 110));

                windowStyle.Colors[ImGuiCol_ChildBg] = ImVec4(0.1608f, 0.1804f, 0.2039f, 1.00f);					// return it's default color for begin child 

                for (auto& item : Buffer::MessageBuffer::getInstance())
                {
                    ImDrawList* drawList = ImGui::GetWindowDrawList();

                    static constexpr uint8_t maxCharacterOnOneLine = 54;
                    static constexpr uint8_t paddingBetweenMessages = 15;
                    static constexpr uint8_t AdditionalSpaceForTimeAndStatus = 40;

                    ImVec2 textPosition;
                    size_t textWidth = ImGui::CalcTextSize(item.data).x;
                    size_t textHeight = ImGui::CalcTextSize(item.data).y;
                    static const size_t textMaxWidth = ImGui::CalcTextSize(" ").x * maxCharacterOnOneLine;

                    if (item.messageType == Buffer::MessageType::kReceived)
                    {
                        static constexpr uint8_t paddingFromAvailableChats = 15;

                        textPosition = ImVec2(paddingFromAvailableChats, ImGui::GetCursorPosY());
                        ImGui::PushTextWrapPos(availableChatsWidth + paddingFromAvailableChats);

                        ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x + textPosition.x - 10, ImGui::GetCursorScreenPos().y));

                        size_t rectangleHeight = ImGui::GetCursorScreenPos().y + textHeight + 10;
                        size_t rectangleLength = ImGui::GetCursorScreenPos().x + textWidth + 50;
                        drawList->AddRectFilled(ImGui::GetCursorScreenPos(), ImVec2(rectangleLength, rectangleHeight), IM_COL32(41, 46, 52, 255), 12.0f);
                    }
                    else if (item.messageType == Buffer::MessageType::kSend)
                    {
                        static constexpr uint8_t rightBorderPadding = 40;

                        if (strlen(item.data) < maxCharacterOnOneLine)
                        {
                            textPosition = ImVec2(ImGui::GetWindowWidth() - textWidth - rightBorderPadding, ImGui::GetCursorPosY() + 5);
                            ImGui::PushTextWrapPos(ImGui::GetWindowWidth() - rightBorderPadding);
                        }
                        else
                        {
                            textPosition = ImVec2(ImGui::GetWindowWidth() - textMaxWidth - rightBorderPadding, ImGui::GetCursorPosY() + 5);
                            ImGui::PushTextWrapPos(ImGui::GetWindowWidth() - rightBorderPadding);

                        }
                    }

                    ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x + textPosition.x - 10, ImGui::GetCursorScreenPos().y));

                    size_t rectangleHeight = ImGui::GetCursorScreenPos().y + textHeight + 10;
                    size_t rectangleLength = ImGui::GetCursorScreenPos().x + textWidth + AdditionalSpaceForTimeAndStatus;
                    drawList->AddRectFilled(ImGui::GetCursorScreenPos(), ImVec2(rectangleLength, rectangleHeight), IM_COL32(41, 46, 52, 255), 12.0f);

                    ImGui::SetCursorPos(textPosition);
                    ImGui::Text(item.data);
                    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + paddingBetweenMessages);
                    ImGui::PopTextWrapPos();
                    // TODO: fix rectangle
                }

                ImGui::EndChild();
            }
        } 
        else if (ImGui::GetWindowWidth() >= this->_defaultDisplayWidth && chatSelected == -1)
        {
            ImDrawList* drawList = ImGui::GetWindowDrawList();

            const char* textToDraw = "Select a chat to start messaging";
            size_t textX = (ImGui::GetWindowWidth() - availableChatsWidth) / 2 + (availableChatsWidth / 2);
            size_t textY = ImGui::GetWindowHeight() / 2;

            size_t rectangleLength = ImGui::CalcTextSize(textToDraw).x + textX;
            size_t rectangleHeight = ImGui::CalcTextSize(textToDraw).y + textY;
            drawList->AddRectFilled(ImVec2(textX - 10, textY - 4),
                                    ImVec2(rectangleLength + 10, rectangleHeight + 4),
                                    IM_COL32(41, 46, 52, 255),
                                    12.0f);																			// rounding					

            ImGui::SetCursorPos(ImVec2(textX, textY));
            ImGui::Text(textToDraw);
        }

        ImGui::End();
    }

    void MainWindow::Render() noexcept
    {
        if (this->_windowStatus != WindowStatusCode::kWIndowInited)
            return;

        ImGui::Render();
        glfwGetFramebufferSize(this->_mainWindow, &this->_currentDisplayWidth, &this->_currentDisplayHeight);
        glViewport(0, 0, this->_currentDisplayWidth, this->_currentDisplayHeight);

        glClearColor(this->_windowBackgroundColor.x * this->_windowBackgroundColor.w,								// red  
                     this->_windowBackgroundColor.y * this->_windowBackgroundColor.w,								// green							
                     this->_windowBackgroundColor.z * this->_windowBackgroundColor.w,							    // blue 
                     this->_windowBackgroundColor.w);																// alpha

        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(this->_mainWindow);
    }

    void MainWindow::Cleanup() noexcept
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        glfwDestroyWindow(this->_mainWindow);
        glfwTerminate();

        Network::Client::GetInstance().~Client();

        this->_windowStatus = WindowStatusCode::kWindowDeleted;
    }

    MainWindow::~MainWindow()
    {
        if (this->_windowStatus != WindowStatusCode::kWindowDeleted)
            MainWindow::Cleanup();
    }

} // !namespace Gui
