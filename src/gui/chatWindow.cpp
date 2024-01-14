#include "chatWindow.h"

extern std::list<MessageBuffer::MessageNode> MessageBuffer::messageBuffer;

namespace Gui
{
    void ChatWindow::DrawGui() noexcept
    {
        static std::vector<UserData::User> foundUsers;
        static std::vector<Chat::Chat> availableChats;
        static bool isAvailableChatsUpdated = true;
        static bool newChat = false;
        static size_t sameChatSelected;
        bool isSearch = false;

        ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_NoTabBar);				        // enable docking 

        ImGui::Begin("##main window");

        // available chats
        
        static int32_t chatSelected = -1;
        static constexpr const size_t availableChatsStartHeight = 65;
        static constexpr const float availableChatsWidthScaleFactor = 3.0f;
        float availableChatsWidth = ImGui::GetWindowWidth() / availableChatsWidthScaleFactor;
        {

            // search
            {
                ImDrawList* drawList = ImGui::GetWindowDrawList();
                drawList->AddRectFilled(ImVec2(0, 0), ImVec2(availableChatsWidth, 65), IM_COL32(41, 46, 52, 255));

                ImGuiStyle& windowStyle = ImGui::GetStyle();
                const float oldRounding = windowStyle.FrameRounding;
                const ImVec4 oldInputTextColor = windowStyle.Colors[ImGuiCol_FrameBg];
                windowStyle.FrameRounding = 12.0f;
                windowStyle.Colors[ImGuiCol_FrameBg] = ImVec4(0.0941f, 0.0980f, 0.1137f, 1.00f);

                static std::string search;
                ImGui::SetCursorPos(ImVec2(55, 20));
                ImGui::PushItemWidth(availableChatsWidth - 70);
                if (ImGui::InputTextWithHint("##search", "Search", &search))
                {
                    foundUsers = UserData::User::FindUsersByLogin(search);

                }
                if (search != "")
                {
                    isSearch = true;
                }
                ImGui::PopItemWidth();

                windowStyle.FrameRounding = oldRounding;
                windowStyle.Colors[ImGuiCol_FrameBg] = oldInputTextColor;
            }

            ImVec2 availableChatsSize;
            if (ImGui::GetWindowWidth() < _defaultDisplayWidth)
            {
                availableChatsSize = ImVec2(ImGui::GetWindowWidth(), 0);
            }
            else
            {
                availableChatsSize = ImVec2(availableChatsWidth, 0);
            }

            ImGui::SetCursorPosY(availableChatsStartHeight);
            ImGui::BeginChild("##available chats", availableChatsSize);

            ImVec2 selectablePosition = { 0, 0 };

            ImGui::SetCursorPos(selectablePosition);
            if (isSearch)
            {
                for (size_t i = 0; i < foundUsers.size(); ++i)
                {
                    if (ImGui::Selectable(foundUsers[i].GetUserLogin().c_str(), chatSelected == i, 0, ImVec2(0, 50)))
                    {
                        if (chatSelected != sameChatSelected)
                        {
                            newChat = true;
                            sameChatSelected = i;
                        }
                        chatSelected = i;

                    }
                }
            }
            else
            {
                if (isAvailableChatsUpdated)
                {
                    availableChats = Chat::Chat::GetAvailableChats(currentUser.GetUserLogin());
                    isAvailableChatsUpdated = false;
                }

                for (size_t i = 0; i < availableChats.size(); ++i)
                {

                    if (ImGui::Selectable(availableChats[i].GetChatName().c_str(), chatSelected == i, 0, ImVec2(0, 50)))
                    {
                        if (chatSelected != sameChatSelected)
                        {
                            newChat = true;
                            sameChatSelected = i;
                        }
                        chatSelected = i;
                    }
                }
            }

            ImGui::SetItemAllowOverlap();

            //ImGui::SetCursorPos(ImVec2(selectablePosition.x + 10, selectablePosition.y + 15));
            //ImGui::Text("text");

            selectablePosition.y += 50;


            // unselect current chat
            if (ImGui::IsKeyPressed(ImGuiKey_Escape))
            {
                chatSelected = -1;
            }

            ImGui::EndChild();
        }

        // chat zone
        if (ImGui::GetWindowWidth() >= _defaultDisplayWidth && chatSelected != -1)
        {
            // selected user info
            {
                ImDrawList* drawList = ImGui::GetWindowDrawList();
                drawList->AddRectFilled(ImVec2(availableChatsWidth, 0), ImVec2(ImGui::GetWindowSize().x, 65), IM_COL32(41, 46, 52, 255));
            }

            // input text
            static bool isEnterPressed = false;
            static bool reclaimFocus = false;
            static bool scrollToBottom = false;
            static bool autoScroll = true;
            {
                float oldFontScale = ImGui::GetFont()->Scale;
                ImGui::GetFont()->Scale *= 1.4f;																	// set new font scale for input text
                ImGui::PushFont(ImGui::GetFont());

                ImVec2 inputTextSize = ImVec2(ImGui::GetWindowWidth() - availableChatsWidth - 60, 45);
                ImGui::SetCursorPos(ImVec2(availableChatsWidth, ImGui::GetWindowHeight() - 45));
                if (ImGui::InputTextMultilineWithHint("##input", "Write a message", &(_inputBuffer), inputTextSize, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CtrlEnterForNewLine))
                {
                    isEnterPressed = true;
                    reclaimFocus = true;

                    scrollToBottom = true;
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
                //TODO: add arrow button
                ImVec2 buttonSize = ImVec2(45, 40);
                ImGui::SameLine();
                ImGui::SetCursorPosY(ImGui::GetWindowHeight() - 45);
                if (ImGui::Button("send", buttonSize))
                {
                    isButtonPressed = true;
                    reclaimFocus = true;

                    scrollToBottom = true;
                }
            }

            // send message 
            if ((isEnterPressed || isButtonPressed) && _inputBuffer != "")
            {
                //TODO: 4096 max message size
                if (isSearch)
                {
                    isSearch = false;
                    Network::Client::GetInstance().SendUserMessage(currentUser.GetUserLogin(), foundUsers[chatSelected].GetUserLogin(), _inputBuffer);
                }
                else
                {
                    Network::Client::GetInstance().SendUserMessage(currentUser.GetUserLogin(), availableChats[chatSelected].GetChatName(), _inputBuffer);
                }

                MessageBuffer::messageBuffer.push_back(MessageBuffer::MessageNode(MessageBuffer::MessageStatus::kSend, _inputBuffer));

                isEnterPressed = false;
                isButtonPressed = false;

                reclaimFocus = true;

                _inputBuffer = "";
            }

            // display sent and received messages
            {
                ImGuiStyle& windowStyle = ImGui::GetStyle();
                windowStyle.Colors[ImGuiCol_ChildBg] = ImVec4(0.0941f, 0.0980f, 0.1137f, 1.00f);					// setup new color for begin child

                if (newChat)
                {
                    newChat = false;

                    Network::ChatPacket chatPacket =
                    {
                        .actionType = NetworkCore::ActionType::kReceiveAllMessagesForSelectedChat,
                        .chatUserLogin = currentUser.GetUserLogin(),
                        .chatId = availableChats[chatSelected].GetChatId(),
                    };

                    Network::Client::GetInstance().SendChatInfoPacket(chatPacket);

                    Network::Client::GetInstance().GetServerResponse<bool>();
                }

                ImGui::SetCursorPos(ImVec2(availableChatsWidth, availableChatsStartHeight));
                ImGui::BeginChild("##chat zone", ImVec2(ImGui::GetWindowWidth() - availableChatsWidth,
                    ImGui::GetWindowHeight() - 110));

                windowStyle.Colors[ImGuiCol_ChildBg] = ImVec4(0.1608f, 0.1804f, 0.2039f, 1.00f);					// return it's default color for begin child 

                size_t a = MessageBuffer::messageBuffer.size();
                for (const auto& item : MessageBuffer::messageBuffer)
                {
                    ImDrawList* drawList = ImGui::GetWindowDrawList();

                    static constexpr uint8_t maxCharacterOnOneLine = 54;
                    static constexpr uint8_t paddingBetweenMessages = 5;

                    ImVec2 textPosition;
                    float textWidth = ImGui::CalcTextSize(item.data.c_str()).x;
                    float textHeight = ImGui::CalcTextSize(item.data.c_str()).y;
                    static const float textMaxWidth = ImGui::CalcTextSize(" ").x * maxCharacterOnOneLine;

                    if (item.messageType == MessageBuffer::MessageStatus::kReceived)
                    {
                        static constexpr uint8_t paddingFromAvailableChats = 15;

                        textPosition = ImVec2(paddingFromAvailableChats, ImGui::GetCursorPosY());
                        ImGui::PushTextWrapPos(availableChatsWidth + paddingFromAvailableChats);
                    }
                    else if (item.messageType == MessageBuffer::MessageStatus::kSend)
                    {
                        static constexpr uint8_t rightBorderPadding = 40;

                        if (strlen(item.data.c_str()) < maxCharacterOnOneLine)
                        {
                            textPosition = ImVec2(ImGui::GetWindowWidth() - textWidth - rightBorderPadding, ImGui::GetCursorPosY() + 5);
                        }
                        else
                        {
                            textPosition = ImVec2(ImGui::GetWindowWidth() - textMaxWidth - rightBorderPadding, ImGui::GetCursorPosY() + 5);
                        }

                        ImGui::PushTextWrapPos(ImGui::GetWindowWidth() - rightBorderPadding);
                    }

                    ImDrawListSplitter splitter;
                    splitter.Split(drawList, 2);

                    splitter.SetCurrentChannel(drawList, 1);
                    ImGui::SetCursorPos(textPosition);
                    ImGui::Text(item.data.c_str());
                    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + paddingBetweenMessages);
                    ImGui::PopTextWrapPos();

                    splitter.SetCurrentChannel(drawList, 0);
                    ImVec2 rectanglePosition = ImVec2(ImGui::GetItemRectMin().x - 10, ImGui::GetItemRectMin().y - 4);
                    float rectangleHeight = ImGui::GetItemRectMax().x + 10;
                    float rectangleLength = ImGui::GetItemRectMax().y + 4;
                    drawList->AddRectFilled(rectanglePosition, ImVec2(rectangleHeight, rectangleLength), IM_COL32(41, 46, 52, 255), 12.0f);

                    splitter.Merge(drawList);
                }

                        // Keep up at the bottom of the scroll region if we were already at the bottom at the beginning of the frame.
                        // Using a scrollbar or mouse-wheel will take away from the bottom edge.
                if (scrollToBottom || (autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
                {
                    ImGui::SetScrollHereY(1.0f);
                }
                scrollToBottom = false;

                ImGui::EndChild();
            }
        }
        else if (ImGui::GetWindowWidth() >= _defaultDisplayWidth && chatSelected == -1)
        {
            ImDrawList* drawList = ImGui::GetWindowDrawList();

            //TODO: fix textX and textY
            const char* textToDraw = "Select a chat to start messaging";
            float textX = (ImGui::GetWindowWidth() - availableChatsWidth) / 2 + (availableChatsWidth / 2);
            float textY = ImGui::GetWindowHeight() / 2;

            float rectangleLength = ImGui::CalcTextSize(textToDraw).x + textX;
            float rectangleHeight = ImGui::CalcTextSize(textToDraw).y + textY;
            drawList->AddRectFilled(ImVec2(textX - 10, textY - 4),
                ImVec2(rectangleLength + 10, rectangleHeight + 4),
                IM_COL32(41, 46, 52, 255),
                12.0f);																			                    // rounding					

            ImGui::SetCursorPos(ImVec2(textX, textY));
            ImGui::Text(textToDraw);
        }

        ImGui::End();
    }




} // !namespace Gui