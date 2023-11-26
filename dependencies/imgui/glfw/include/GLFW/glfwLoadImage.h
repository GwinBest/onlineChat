#pragma once

#define STB_IMAGE_IMPLEMENTATION
#include "../../stb/stb_image.h"

#include <GLFW/glfw3.h>

#define GL_CLAMP_TO_EDGE 0x812F

bool loadTextureFromFile(const char* filename, GLuint* out_texture, int* out_width, int* out_height);


