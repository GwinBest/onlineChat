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

		static std::string& GetName() noexcept;
		static std::string& GetLogin() noexcept;
		static std::string& GetPassword() noexcept;

	private:
		void GenerateControls() noexcept override;
	
	private:
		static inline std::string _inputBufferName;
		static inline std::string _inputBufferLogin;
		static inline std::string _inputBufferPassword;

		static inline bool _isInputNameEmpty = false;
		static inline bool _isInputLoginEmpty = false;
		static inline bool _isInputPasswordEmpty = false;

		static inline bool _isSignUpButtonPressed = false;
		static inline bool _isBackToLoginButtonPressed = false;
	};
} // !namespace Gui