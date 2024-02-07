include_directories(
    ../thirdParty/imgui/include
    ../thirdParty/imgui/glfw/include
    ../resources
    ../thirdParty/mySql/include
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
    ../thirdParty/imgui/include/imgui.cpp
    ../thirdParty/imgui/include/imgui_stdlib.cpp
    ../thirdParty/imgui/include/imgui_demo.cpp
    ../thirdParty/imgui/include/imgui_draw.cpp
    ../thirdParty/imgui/include/imgui_impl_glfw.cpp
    ../thirdParty/imgui/include/imgui_impl_opengl3.cpp
    ../thirdParty/imgui/include/imgui_tables.cpp
    ../thirdParty/imgui/include/imgui_widgets.cpp
    ../thirdParty/imgui/glfw/include/GLFW/glfwLoadImage.h
    ../thirdParty/imgui/glfw/include/GLFW/glfwLoadImage.cpp
)

add_executable(OnlineChat ${SOURCE_FILES})

target_link_libraries(OnlineChat PUBLIC opengl32)
target_link_libraries(OnlineChat PUBLIC ${CMAKE_SOURCE_DIR}/thirdParty/imgui/glfw/lib-vc2010-64/glfw3.lib)
