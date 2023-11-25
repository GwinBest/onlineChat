#pragma once

#include "iImGuiWindow.h"

namespace Gui
{
	class LoginWindow final : public IImGuiWindow
	{
	public:
		LoginWindow() = default;

		static bool IsSignUpLabelPressed() noexcept;
		static bool IsLoginButtonPressed() noexcept;

	private:
		void GenerateControls() noexcept override;

	private:
		static inline bool _isLoginButtonPressed = false;
		static inline bool _isSignUpLabelPressed = false;
	};

} // !namespace Gui