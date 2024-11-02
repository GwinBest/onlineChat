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
    ${CMAKE_SOURCE_DIR}/src
    ../resources
    ${CMAKE_SOURCE_DIR}/thirdParty/mySql/include
    ${stb_src_path}
)

file(COPY ${CMAKE_SOURCE_DIR}/resources DESTINATION ${CMAKE_BINARY_DIR}/clientApp)

set(SOURCE_FILES
    main.cpp
     ../src/gui/signIn/signInPage.cpp
     ../src/gui/signIn/signInPage.h
     ../src/gui/signIn/signInPage.ui
     ../src/gui/mainWindow.h
     ../src/gui/mainWindow.cpp
     ../src/gui/mainWindow.ui
    ../src/gui/colors/colors.h
    ../src/gui/login/loginPage.cpp
    ../src/gui/login/loginPage.h
    ../src/gui/login/loginPage.ui
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

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)

target_link_libraries(${CMAKE_PROJECT_NAME}
        Qt::Core
        Qt::Gui
        Qt::Widgets
)

if (WIN32 AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(DEBUG_SUFFIX)
    if (MSVC AND CMAKE_BUILD_TYPE MATCHES "Debug")
        set(DEBUG_SUFFIX "d")
    endif ()
    set(QT_INSTALL_PATH "${CMAKE_PREFIX_PATH}")
    if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
        set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
            set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        endif ()
    endif ()
    if (EXISTS "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
    endif ()
    foreach (QT_LIB Core Gui Widgets)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/bin/Qt6${QT_LIB}${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
    endforeach (QT_LIB)
endif ()
