#pragma once

#if _WIN32 || _WIN64
#if _WIN64
#define BITS 64
#else
#define BITS 32
#endif
#endif


enum RenderType
{
	OpenGL = 1,
	Vulkan = 2
};

#define RENDERER 1