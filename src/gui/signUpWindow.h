#pragma once

#include "../common/common.h"
#include "iImGuiWindow.h"

namespace Gui
{
	class SignUpWindow final : public IImGuiWindow
	{
	public:
		SignUpWindow() = default;

		static bool IsSignUpButtonPressed() noexcept;
		static bool IsBackToLoginButtonPressed() noexcept;

		static const char* GetName() noexcept;
		static const char* GetLogin() noexcept;
		static const std::string& GetPassword() noexcept;

		static void SetShowUserAlreadyExistMessage(const bool state) noexcept;

	private:
		void DrawGui() noexcept override;
	
	private:
		static inline char _inputBufferName[Common::userNameSize] = "";
		static inline char _inputBufferLogin[Common::userLoginSize] = "";
		static inline std::string _inputBufferPassword;

		static inline bool _isInputNameEmpty = false;
		static inline bool _isInputLoginEmpty = false;
		static inline bool _isInputPasswordEmpty = false;

		static inline bool _showUserAlreadyExistMessage = false;

		static inline bool _isSignUpButtonPressed = false;
		static inline bool _isBackToLoginButtonPressed = false;
	};

} // !namespace Gui