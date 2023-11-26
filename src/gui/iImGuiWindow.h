#pragma once

#include "imgui.h"
#include "imgui_stdlib.h"

#include "iWindow.h"
 
// from GL
typedef unsigned int GLuint;

extern bool loadTextureFromFile(const char* filename, GLuint* out_texture, int* out_width, int* out_height);

namespace Gui
{
	class IImGuiWindow : public IWindow
	{
	};

} // !namespace Gui