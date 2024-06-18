#include "loginWindow.h"

#include <imgui_internal.h>

namespace Gui
{
	bool LoginWindow::IsSignUpLabelPressed() noexcept
	{
		if (_isSignUpLabelPressed)
		{
			_isSignUpLabelPressed = false;
			return true;
		}

		return false;
	}

	bool LoginWindow::IsLoginButtonPressed() noexcept
	{
		if (_isLoginButtonPressed)
		{
			_isLoginButtonPressed = false;
			return true;
		}

		return false;
	}

	const char* LoginWindow::GetLogin() noexcept
	{
		return _inputBufferLogin;
	}

	const std::string& LoginWindow::GetPassword() noexcept
	{
		return _inputBufferPassword;
	}

	void LoginWindow::SetShowUserNotFoundMessage(const bool value) noexcept
	{
		_showUserNotFoundMessage = value;
	}

	void LoginWindow::DrawGui() noexcept
	{
		ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_NoTabBar);							// enable docking 
		
		ImGui::Begin("##login window");

		static const std::string welcomeText = "Welcome back!";
		const float welcomeTextX = ImGui::GetWindowSize().x / 2 - ImGui::CalcTextSize(welcomeText.c_str()).x / 2;
		constexpr float welcomeTextY = 100.0f;
		ImGui::SetCursorPos(ImVec2(welcomeTextX, welcomeTextY));
		ImGui::TextDisabled(welcomeText.c_str());

		static const std::string loginText = "Log into your account";
		const float loginTextX = ImGui::GetWindowSize().x / 2 - ImGui::CalcTextSize(loginText.c_str()).x / 2;
		ImGui::SetCursorPosX(loginTextX);
		ImGui::Text(loginText.c_str());
		//TODO: gap between text and input text when fullscreen

		static constexpr float inputTextWidth = 300.0f;
		ImGuiStyle& windowStyle = ImGui::GetStyle();
		ImVec4 oldInputTextColor = windowStyle.Colors[ImGuiCol_FrameBg];
		ImVec4 inputTextEmptyColor = ImVec4(0.8f, 0.0f, 0.0f, 0.5f);
		if (_isInputLoginEmpty)
		{
			windowStyle.Colors[ImGuiCol_FrameBg] = inputTextEmptyColor;
		}
		const float inputLoginX = ImGui::GetWindowSize().x / 2 - inputTextWidth / 2;
		const float inputLoginY = ImGui::GetWindowSize().y / 2 - welcomeTextY;
		ImGui::SetCursorPos(ImVec2(inputLoginX, inputLoginY));
		ImGui::PushItemWidth(inputTextWidth);
		ImGui::InputTextWithHint("##input text login", "Login", _inputBufferLogin, IM_ARRAYSIZE(_inputBufferLogin), ImGuiInputTextFlags_EnterReturnsTrue);
		ImGui::PopItemWidth();
		windowStyle.Colors[ImGuiCol_FrameBg] = oldInputTextColor;

		if (_showUserNotFoundMessage)
		{
			ImGui::SetCursorPos(ImVec2(inputLoginX, inputLoginY - 25));
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "This user is not exist");
		}

		const std::string forgotPasswordText = "Forgot password?";
		const float forgotPasswordX = inputLoginX + inputTextWidth - ImGui::CalcTextSize(forgotPasswordText.c_str()).x;
		const float forgotPasswordY = inputLoginY + 45.0f;
		ImGui::SetCursorPos(ImVec2(forgotPasswordX, forgotPasswordY));
		ImGui::TextDisabled(forgotPasswordText.c_str());
		//TODO: add forgot pass realization

		if (_isInputPasswordEmpty)
		{
			windowStyle.Colors[ImGuiCol_FrameBg] = inputTextEmptyColor;
		}
		const float inputPasswordX = inputLoginX;
		const float inputPasswordY = inputLoginY + 70.0f;
		ImGui::SetCursorPos(ImVec2(inputPasswordX, inputPasswordY));
		ImGui::PushItemWidth(inputTextWidth);
		ImGui::InputTextWithHint("##input text password", "Password", &_inputBufferPassword, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_Password);
		ImGui::PopItemWidth();
		windowStyle.Colors[ImGuiCol_FrameBg] = oldInputTextColor;

		const float loginButtonX = inputLoginX;
		const float loginButtonY = inputPasswordY + 55.0f;
		ImGui::SetCursorPos(ImVec2(loginButtonX, loginButtonY));
		if (ImGui::Button("Login", ImVec2(inputTextWidth, 0)))															//0 means that Y will be default for ImGui
		{
			if (_inputBufferLogin[0] == '\0')
			{
				_isInputLoginEmpty = true;
			}
			else
			{
				_isInputLoginEmpty = false;
			}

			if (_inputBufferPassword == "")
			{
				_isInputPasswordEmpty = true;
			}
			else
			{
				_isInputPasswordEmpty = false;
			}

			if (!_isInputLoginEmpty && !_isInputPasswordEmpty)
			{
				_isLoginButtonPressed = true;

				_showUserNotFoundMessage = false;
			}
		}

		const std::string signUpText = "Don't have an account? Create it!";
		const float signUpTextX = inputLoginX;
		const float signUpTextY = loginButtonY + 40.0f;
		const float signUpTextLength = ImGui::CalcTextSize(signUpText.c_str()).x;
		ImGui::SetCursorPos(ImVec2(signUpTextX, signUpTextY));
		if (ImGui::Selectable(signUpText.c_str(), false, 0, ImVec2(signUpTextLength, 0)))								//0 means that Y will be default for ImGui
		{
			_isSignUpLabelPressed = true;

			_showUserNotFoundMessage = false;

			_inputBufferLogin[0] = '\0';
			_inputBufferPassword = "";

			_isInputLoginEmpty = false;
			_isInputPasswordEmpty = false;

		}

		ImGui::End();
	}

} // !namespace Gui