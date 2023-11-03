#ifndef _ONLINECHAT_GUI_GUI_H_
#define _ONLINECHAT_GUI_GUI_H_

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <GLFW/glfw3.h> 

#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif


namespace Gui
{
	class MainWindow
	{
	private:
		enum WindowStatusCode : uint8_t
		{
			kWIndowUndefined = 0,
			kWIndowInited	 = 1,
			kWindowDeleted	 = 2
		};
		const char* _glslVersion = "#version 130";
		const ImVec4 _windowColorBackground = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
		ImGuiIO _io;

		int _currentDisplayHeight;
		int _currentDisplayWidth;

		uint8_t _windowStatus = kWIndowUndefined;

	public:
		GLFWwindow* _window = nullptr;
		MainWindow() = default;
		~MainWindow();

		bool Init();
		void NewFrame();
		void Update();
		void Render();
		void Cleanup();
	};
}


























#endif // !_ONLINECHAT_GUI_GUI_H_

