#ifndef _ONLINECHAT_GUI_GUI_H_
#define _ONLINECHAT_GUI_GUI_H_

#include <thread>

#include "imgui.h"
#include "imgui_stdlib.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <GLFW/glfw3.h> 

#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

#include "../client/client.h"

namespace Gui
{
	class MainWindow
	{
	public:
		MainWindow() = default;
		~MainWindow();

		bool Init();
		void Draw();
		void Cleanup();

	private:
		void NewFrame();
		void GenerateControls();
		void Render();

	private:
		enum class WindowStatusCode : uint8_t
		{
			kWIndowUndefined = 0,
			kWIndowInited = 1,
			kWindowDeleted = 2
		};
		const char* _glslVersion = "#version 130";
		GLFWwindow* _window = nullptr;
		const ImVec4 _windowColorBackground = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

		int _currentDisplayHeight;
		int _currentDisplayWidth;

		WindowStatusCode _windowStatus = WindowStatusCode::kWIndowUndefined;

		std::string _buffer;

	};

}//!namespace Gui

#endif // !_ONLINECHAT_GUI_GUI_H_

