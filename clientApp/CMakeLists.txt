cmake_minimum_required(VERSION 3.20)

set(imgui_src_path ${CMAKE_SOURCE_DIR}/thirdParty/imgui)

set(glfw_src_path ${CMAKE_SOURCE_DIR}/thirdParty/glfw)

set(stb_src_path ${CMAKE_SOURCE_DIR}/thirdParty/stb)

# glfw
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Build the GLFW example programs")
set(GLFW_BUILD_TESTS OFF CACHE BOOL "Build the GLFW test programs")
set(GLFW_BUILD_DOCS OFF CACHE BOOL "Build the GLFW documentation")
set(GLFW_INSTALL OFF CACHE BOOL "Generate installation target") 
add_subdirectory(${glfw_src_path} glfw_binary)

# imgui with glfw + opengl3 backend
add_library(imgui STATIC
   ${imgui_src_path}/imgui.cpp
   ${imgui_src_path}/imgui_demo.cpp
   ${imgui_src_path}/imgui_draw.cpp
   ${imgui_src_path}/imgui_tables.cpp
   ${imgui_src_path}/imgui_widgets.cpp
   ${imgui_src_path}/imgui_internal.h
   ${imgui_src_path}/backends/imgui_impl_glfw.cpp
   ${imgui_src_path}/backends/imgui_impl_opengl3.cpp
   ${imgui_src_path}/misc/cpp/imgui_stdlib.cpp
)

target_include_directories(imgui PUBLIC 
   ${imgui_src_path}
   ${imgui_src_path}/backends
   ${imgui_src_path}/misc/cpp
)

target_link_libraries(imgui glfw)

include_directories(
    ../resources
    ${CMAKE_SOURCE_DIR}/thirdParty/mySql/include
    ${stb_src_path}
)

file(COPY ${CMAKE_SOURCE_DIR}/resources DESTINATION ${CMAKE_BINARY_DIR}/clientApp)

set(SOURCE_FILES
    main.cpp
    ../src/client/client.cpp
    ../src/gui/glfwWindow.cpp
    ../src/gui/chatWindow.h
    ../src/gui/chatWindow.cpp
    ../src/gui/iImGuiWindow.h
    ../src/gui/loginWindow.h
    ../src/gui/loginWindow.cpp
    ../src/gui/signUpWindow.h
    ../src/gui/signUpWindow.cpp
    ../src/userData/userCredentialsFile.h
    ../src/userData/userCredentialsFile.cpp
    ../src/userData/user.cpp
    ../src/userData/user.h
    ../src/chatSystem/chat.h
    ../src/chatSystem/chat.cpp
    ../src/networkCore/networkCore.h
    ../src/common/common.h
    ../src/userData/userRepository.h
    ../src/userData/userRepository.cpp
    ../src/utils/loadImage.cpp
)

add_executable(OnlineChat ${SOURCE_FILES})

find_package(OpenGL REQUIRED)

target_link_libraries(${CMAKE_PROJECT_NAME} imgui ${OPENGL_LIBRARIES})

