#include "loginWindow.h"


namespace Gui
{
	bool LoginWindow::IsLoginButtonPressed() noexcept
	{
		if (_isLoginButtonPressed)
		{
			_isLoginButtonPressed = false;
			return true;
		}

		return false;
	}

	bool LoginWindow::IsSignUpLabelPressed() noexcept
	{
		if (_isSignUpLabelPressed)
		{
			_isSignUpLabelPressed = false;
			return true;
		}

		return false;
	}

	void LoginWindow::GenerateControls() noexcept
	{
		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_NoTabBar);							// enable docking 
		
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
		//FIXME: gap between text and input text when fullscreen

		static constexpr float inputTextWidth = 300.0f;

		static std::string inputBufferLogin;
		const float inputLoginX = ImGui::GetWindowSize().x / 2 - inputTextWidth / 2;
		const float inputLoginY = ImGui::GetWindowSize().y / 2 - welcomeTextY;
		ImGui::SetCursorPos(ImVec2(inputLoginX, inputLoginY));
		ImGui::PushItemWidth(inputTextWidth);
		ImGui::InputTextWithHint("##input text login", "Login", &inputBufferLogin, ImGuiInputTextFlags_EnterReturnsTrue);
		ImGui::PopItemWidth();

		const std::string forgotPasswordText = "Forgot password?";
		const float forgotPasswordX = inputLoginX + inputTextWidth - ImGui::CalcTextSize(forgotPasswordText.c_str()).x;
		const float forgotPasswordY = inputLoginY + 45.0f;
		ImGui::SetCursorPos(ImVec2(forgotPasswordX, forgotPasswordY));
		ImGui::TextDisabled(forgotPasswordText.c_str());
		//TODO: add forgot pass realization

		static std::string inputBufferPassword;
		const float inputPasswordX = inputLoginX;
		const float inputPasswordY = inputLoginY + 70.0f;
		ImGui::SetCursorPos(ImVec2(inputPasswordX, inputPasswordY));
		ImGui::PushItemWidth(inputTextWidth);
		ImGui::InputTextWithHint("##input text password", "Password", &inputBufferPassword, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_Password);
		ImGui::PopItemWidth();

		const float loginButtonX = inputLoginX;
		const float loginButtonY = inputPasswordY + 55.0f;
		ImGui::SetCursorPos(ImVec2(loginButtonX, loginButtonY));
		if (ImGui::Button("Login", ImVec2(inputTextWidth, 0)))															//0 means that Y will be default for ImGui
		{
			_isLoginButtonPressed = true;
		}

		const std::string signUpText = "Don't have an account? Create it!";
		const float signUpTextX = inputLoginX;
		const float signUpTextY = loginButtonY + 40.0f;
		const float signUpTextLength = ImGui::CalcTextSize(signUpText.c_str()).x;
		ImGui::SetCursorPos(ImVec2(signUpTextX, signUpTextY));
		if (ImGui::Selectable(signUpText.c_str(), false, 0, ImVec2(signUpTextLength, 0)))								//0 means that Y will be default for ImGui
		{
			_isSignUpLabelPressed = true;
		}

		ImGui::End();
	}

} // !namespace Gui