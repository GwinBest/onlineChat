#pragma once

#include "iImGuiWindow.h"

namespace Gui
{
	class SignUpWindow final : public IImGuiWindow
	{
	public:
		SignUpWindow() = default;

		static bool IsSignUpButtonPressed() noexcept;

	private:
		void GenerateControls() noexcept override;
	
	private:
		static inline bool _isSignUpButtonPressed = false;
	};
} // !namespace Gui