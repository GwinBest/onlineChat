#pragma once

#include "iImGuiWindow.h"

namespace Gui
{
	class SignUpWindow final : public IImGuiWindow
	{
	public:
		SignUpWindow() = default;

		static bool IsSignUpButtonPressed() noexcept;
		static bool IsBackToLoginButtonPressed() noexcept;

		static char* GetName() noexcept;
		static char* GetLogin() noexcept;
		static std::string GetPassword() noexcept;

		static void SetShowUserAlreadyExistMessage(const bool state) noexcept;

	private:
		void DrawGui() noexcept override;
	
	private:
		static inline char _inputBufferName[50] = "";
		static inline char _inputBufferLogin[50] = "";
		static inline std::string _inputBufferPassword;

		static inline bool _isInputNameEmpty = false;
		static inline bool _isInputLoginEmpty = false;
		static inline bool _isInputPasswordEmpty = false;

		static inline bool _showUserAlreadyExistMessage = false;

		static inline bool _isSignUpButtonPressed = false;
		static inline bool _isBackToLoginButtonPressed = false;
	};

} // !namespace Gui