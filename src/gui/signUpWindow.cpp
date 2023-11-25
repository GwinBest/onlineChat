#include "signUpWindow.h"

namespace Gui
{
	bool SignUpWindow::IsSignUpButtonPressed() noexcept
	{
		return _isSignUpButtonPressed;
	}

	void SignUpWindow::GenerateControls() noexcept
	{
		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_NoTabBar);							// enable docking 

		ImGui::Begin("##signUp window");

		static const std::string welcomeText = "Welcome to the Online Chat app!";
		const float welcomeTextX = ImGui::GetWindowSize().x / 2 - ImGui::CalcTextSize(welcomeText.c_str()).x / 2;
		constexpr float welcomeTextY = 100.0f;
		ImGui::SetCursorPos(ImVec2(welcomeTextX, welcomeTextY));
		ImGui::TextDisabled(welcomeText.c_str());

		static const std::string loginText = "Create a new account";
		const float loginTextX = ImGui::GetWindowSize().x / 2 - ImGui::CalcTextSize(loginText.c_str()).x / 2;
		ImGui::SetCursorPosX(loginTextX);
		ImGui::Text(loginText.c_str());
		//FIXME: gap between text and input text when fullscreen

		static constexpr float inputTextWidth = 300.0f;

		static std::string inputBufferName;
		const float inputNameX = ImGui::GetWindowSize().x / 2 - inputTextWidth / 2;
		const float inputNameY = ImGui::GetWindowSize().y / 2 - welcomeTextY - 50.0f;
		ImGui::SetCursorPos(ImVec2(inputNameX, inputNameY));
		ImGui::PushItemWidth(inputTextWidth);
		ImGui::InputTextWithHint("##input text name", "Name", &inputBufferName);
		ImGui::PopItemWidth();

		static std::string inputBufferLogin;
		const float inputLoginX = inputNameX;
		const float inputLoginY = inputNameY + 50.0f;
		ImGui::SetCursorPos(ImVec2(inputLoginX, inputLoginY));
		ImGui::PushItemWidth(inputTextWidth);
		ImGui::InputTextWithHint("##input text login", "Login", &inputBufferLogin);
		ImGui::PopItemWidth();

		static std::string inputBufferPassword;
		const float inputPasswordX = inputNameX;
		const float inputPasswordY = inputLoginY + 50.0f;
		ImGui::SetCursorPos(ImVec2(inputPasswordX, inputPasswordY));
		ImGui::PushItemWidth(inputTextWidth);
		ImGui::InputTextWithHint("##input text password", "Password", &inputBufferPassword, ImGuiInputTextFlags_Password);
		ImGui::PopItemWidth();

		const float singUpButtonX = inputNameX;
		const float signUpButtonY = inputPasswordY + 50.0f;
		ImGui::SetCursorPos(ImVec2(singUpButtonX, signUpButtonY));
		if (ImGui::Button("Create an account", ImVec2(inputTextWidth, 0)))												// 0 means that Y will be default for ImGui
		{
			_isSignUpButtonPressed = true;
		}

		ImGui::End();
	}
} // !namespace Gui