#include "chatWindow.h"

#include "../chatSystem/chat.h"
#include "../client/client.h"
#include "../messageBuffer/messageBuffer.h"
#include "../userData/user.h"
#include "../userData/userRepository.h"

extern UserData::User currentUser;
extern std::list<MessageBuffer::MessageNode> MessageBuffer::messageBuffer;

namespace Gui
{
    void ChatWindow::DrawGui() noexcept
    {
        ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_NoTabBar);				                    // enable docking 

        ImGui::Begin("##main window");

        static std::vector<UserData::User> foundUsers;
        constexpr float availableChatsWidthScaleFactor = 3.0f;
        constexpr size_t availableChatsStartHeight = 65;
        float availableChatsWidth = ImGui::GetWindowWidth() / availableChatsWidthScaleFactor;

        static std::string searchBarSearchRequest;
        // search bar
        {
            ImDrawList* drawList = ImGui::GetWindowDrawList();
            drawList->AddRectFilled(ImVec2(0, 0), ImVec2(availableChatsWidth, availableChatsStartHeight), IM_COL32(41, 46, 52, 255));

            ImGuiStyle& windowStyle = ImGui::GetStyle();
            const float oldRounding = windowStyle.FrameRounding;
            const ImVec4 oldInputTextColor = windowStyle.Colors[ImGuiCol_FrameBg];
            windowStyle.FrameRounding = 12.0f;
            windowStyle.Colors[ImGuiCol_FrameBg] = ImVec4(0.0941f, 0.0980f, 0.1137f, 1.00f);

            ImGui::SetCursorPos(ImVec2(55, 20));
            ImGui::PushItemWidth(availableChatsWidth - 70.0f);
            if (ImGui::InputTextWithHint("##search", "Search", &searchBarSearchRequest))
            {
                foundUsers.clear();
                foundUsers = UserData::UserRepository::FindUsersByLogin(searchBarSearchRequest);
            }

            if (searchBarSearchRequest.empty())
            {
                foundUsers.clear();
            }

            ImGui::PopItemWidth();

            windowStyle.FrameRounding = oldRounding;
            windowStyle.Colors[ImGuiCol_FrameBg] = oldInputTextColor;
        }

        static int32_t chatSelected = -1;
        static bool newChatSelected = false;
        static bool isAvailableChatsUpdated = true;
        static std::vector<ChatSystem::Chat> availableChats;
        
        // available chats list
        {
            ImVec2 availableChatsSize;
            if (ImGui::GetWindowWidth() < defaultDisplayWidth)
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

            if (!foundUsers.empty() || !searchBarSearchRequest.empty())
            {
                for (size_t i = 0; i < foundUsers.size(); ++i)
                {
                    if (foundUsers[i].GetUserLogin() == currentUser.GetUserLogin())
                    {
                        continue;
                    }

                    if (ImGui::Selectable(foundUsers[i].GetUserLogin().c_str(), chatSelected == i, 0, ImVec2(0, 50)))
                    {
                        if (chatSelected != i)
                        {
                            newChatSelected = true;
                        }

                        chatSelected = i;

                    }
                }
            }
            else
            {
                if (isAvailableChatsUpdated)
                {
                    availableChats = UserData::UserRepository::GetAvailableChatsForUser(currentUser.GetUserId());
                    isAvailableChatsUpdated = false;
                }

                for (size_t i = 0; i < availableChats.size(); ++i)
                {

                    if (ImGui::Selectable(availableChats[i].GetChatName().c_str(), chatSelected == i, 0, ImVec2(0, 50)))
                    {
                        if (chatSelected != i)
                        {
                            newChatSelected = true;
                        }

                        chatSelected = i;
                    }
                }
            }

            ImGui::SetItemAllowOverlap();

            // unselect current chat
            if (ImGui::IsKeyPressed(ImGuiKey_Escape))
            {
                chatSelected = -1;
            }

            ImGui::EndChild();
        }

        // chat zone
        if (ImGui::GetWindowWidth() >= defaultDisplayWidth && chatSelected != -1)
        {
            // selected user info
            {
                ImDrawList* drawList = ImGui::GetWindowDrawList();
                drawList->AddRectFilled(ImVec2(availableChatsWidth, 0), ImVec2(ImGui::GetWindowSize().x, availableChatsStartHeight), IM_COL32(41, 46, 52, 255));
            }
            
            bool isEnterPressed = false;
            static bool reclaimFocus = false;
            static bool scrollToBottom = false;
            static bool autoScroll = true;
            static constexpr float distanceFromRightToInputText = 45.0f;

            // input text
            {
                float oldFontScale = ImGui::GetFont()->Scale;
                ImGui::GetFont()->Scale *= 1.4f;																	// set new font scale for input text
                ImGui::PushFont(ImGui::GetFont());

                ImVec2 inputTextSize = ImVec2(ImGui::GetWindowWidth() - availableChatsWidth - distanceFromRightToInputText, 45);
                ImGui::SetCursorPos(ImVec2(availableChatsWidth, ImGui::GetWindowHeight() - 45));
                if(ImGui::InputTextMultilineWithHint("##input", "Write a message", _inputBuffer, sizeof(_inputBuffer), 
                    inputTextSize, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CtrlEnterForNewLine))
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

            bool isButtonPressed = false;

            // send button
            {
                ImGuiStyle& windowStyle = ImGui::GetStyle();
                ImVec4 oldButtonColor = windowStyle.Colors[ImGuiCol_Button];
                windowStyle.Colors[ImGuiCol_Button] = windowStyle.Colors[ImGuiCol_FrameBg];								        // set the button color to the same 
                                                                                                                                // as the input text so that only picture is visible
                ImGui::SameLine();
                ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() - distanceFromRightToInputText - 1, ImGui::GetCursorPos().y));
                int32_t sendMessageButtonWidth = 0;																				// loads from loadTextureFromFile
                int32_t sendMessageButtonHeight = 0;																			// loads from loadTextureFromFile
                ImTextureID sendMessageButtonTexture = nullptr;
                loadTextureFromFile(".\\images\\sendMessage.png", reinterpret_cast<GLuint*>(&sendMessageButtonTexture), &sendMessageButtonWidth, &sendMessageButtonHeight);
                if (ImGui::ImageButton("##send message", sendMessageButtonTexture, ImVec2(sendMessageButtonWidth, sendMessageButtonHeight)))
                {
                    isButtonPressed = true;
                    reclaimFocus = true;

                    scrollToBottom = true;
                }
               
                windowStyle.Colors[ImGuiCol_Button] = oldButtonColor;															// reset the button color to default
            }

            // send message 
            if ((isEnterPressed || isButtonPressed) && _inputBuffer[0] != '\0')
            {
                UserData::User receiver;

                if (!foundUsers.empty())
                {
                    receiver.SetUserLogin(availableChats[chatSelected].GetChatName());
                }
                else
                {
                    receiver.SetUserLogin(availableChats[chatSelected].GetChatName());
                }

                ClientNetworking::Client::GetInstance().SendUserMessage(currentUser, receiver.GetUserLogin(), _inputBuffer);

                MessageBuffer::messageBuffer.emplace_back(MessageBuffer::MessageNode(MessageBuffer::MessageStatus::kSend, _inputBuffer));
                
                reclaimFocus = true;

                _inputBuffer[0] = '\0';
            }

            // display sent and received messages
            {
                ImGuiStyle& windowStyle = ImGui::GetStyle();
                ImVec4 oldColor = windowStyle.Colors[ImGuiCol_ChildBg];
                windowStyle.Colors[ImGuiCol_ChildBg] = windowStyle.Colors[ImGuiCol_WindowBg];					// setup new color for begin child

                if (newChatSelected)
                {
                    newChatSelected = false;

                    const NetworkCore::ChatPacket chatPacket =
                    {
                        .actionType = NetworkCore::ActionType::kReceiveAllMessagesForSelectedChat,
                        .chatUserLogin = currentUser.GetUserLogin(),
                        .chatId = availableChats[chatSelected].GetChatId(),
                    };

                    ClientNetworking::Client::GetInstance().SendChatInfoPacket(chatPacket);

                    ClientNetworking::Client::GetInstance().GetServerResponse<bool>();
                }

                ImGui::SetCursorPos(ImVec2(availableChatsWidth, availableChatsStartHeight));
                ImGui::BeginChild("##chat zone", ImVec2(ImGui::GetWindowWidth() - availableChatsWidth,
                    ImGui::GetWindowHeight() - 110));

                windowStyle.Colors[ImGuiCol_ChildBg] = oldColor;					                            // return it's default color for begin child 

                for (const auto& item : MessageBuffer::messageBuffer)
                {
                    ImDrawList* drawList = ImGui::GetWindowDrawList();

                    static constexpr uint8_t maxCharacterOnOneLine = 54;
                    static constexpr uint8_t paddingBetweenMessages = 5;

                    ImVec2 textPosition;
                    float textWidth = ImGui::CalcTextSize(item.data.c_str()).x;
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

                        if (item.data.size() < maxCharacterOnOneLine)
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
        else if (ImGui::GetWindowWidth() >= defaultDisplayWidth && chatSelected == -1)
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
                IM_COL32(41, 46, 52, 255), 12.0f);	                            // 12.0f - rounding					

            ImGui::SetCursorPos(ImVec2(textX, textY));
            ImGui::Text(textToDraw);
        }

        ImGui::End();
    }

} // !namespace Gui