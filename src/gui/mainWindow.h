#ifndef _ONLINECHAT_GUI_GUI_H_
#define _ONLINECHAT_GUI_GUI_H_

#include "imgui.h"
#include "imgui_stdlib.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <GLFW/glfw3.h> 

#include "../client/client.h"

#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

namespace Gui
{
	class MainWindow
	{
	public:
		MainWindow() = default;
		~MainWindow();

		bool Init() noexcept;
		void Draw() noexcept;
		void Cleanup() noexcept;

	private:
		void NewFrame() const noexcept;
		void GenerateControls() noexcept;
		void Render() noexcept;

	private:
		enum class WindowStatusCode : uint8_t
		{
			kWIndowUndefined = 0,
			kWIndowInited = 1,
			kWindowDeleted = 2
		};

		const char* _glslVersion = "#version 130";
		GLFWwindow* _mainWindow = nullptr;
		const ImVec4 _windowBackgroundColor = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		
		const size_t _defaultDisplayHeight = 700;
		const size_t _defaultDisplayWidth = 820;

		int _currentDisplayHeight = _defaultDisplayHeight;
		int _currentDisplayWidth = _defaultDisplayWidth;

		WindowStatusCode _windowStatus = WindowStatusCode::kWIndowUndefined;

		std::string _inputBuffer;

	};

}//!namespace Gui

#endif // !_ONLINECHAT_GUI_GUI_H_

