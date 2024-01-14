# CMakeList.txt : CMake project for OnlineChat, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)
set(CMAKE_CXX_STANDARD 20)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("OnlineChat")

include_directories(thirdParty/imgui/include thirdParty/imgui/glfw/include resources/)



# Add source to this project's executable.
add_executable (OnlineChat
				"clientApp/main.cpp" 
				"src/client/client.cpp" 
				"src/gui/glfwWindow.cpp"
				"thirdParty/imgui/include/imgui.cpp" 
				"thirdParty/imgui/include/imgui_stdlib.cpp"
				"thirdParty/imgui/include/imgui_demo.cpp"
				"thirdParty/imgui/include/imgui_draw.cpp"
				"thirdParty/imgui/include/imgui_impl_glfw.cpp"
				"thirdParty/imgui/include/imgui_impl_opengl3.cpp"
				"thirdParty/imgui/include/imgui_tables.cpp"
				"thirdParty/imgui/include/imgui_widgets.cpp"
				"src/gui/chatWindow.h" 
				"src/gui/chatWindow.cpp"  "src/gui/iImGuiWindow.h"  
				"src/gui/loginWindow.h" "src/gui/loginWindow.cpp" 
				"src/gui/signUpWindow.h" "src/gui/signUpWindow.cpp"
				"thirdParty/imgui/glfw/include/GLFW/glfwLoadImage.h"
				"thirdParty/imgui/glfw/include/GLFW/glfwLoadImage.cpp" "src/userData/userCredentialsFile.h"
				"src/userData/userCredentialsFile.cpp" "src/userData/userData.cpp" "src/userData/userData.h" "src/chat/chat.h" "src/chat/chat.cpp"  "src/messageBuffer/messageBuffer.cpp" "src/networkCore/networkCore.h")

target_link_libraries(OnlineChat
	opengl32.lib
    ${CMAKE_SOURCE_DIR}/thirdParty/imgui/glfw/lib-vc2010-64/glfw3.lib
)


if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET OnlineChat PROPERTY CXX_STANDARD 20)
endif()
