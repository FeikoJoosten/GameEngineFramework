#pragma once

#if _WIN32 || _WIN64
#if _WIN64
/// <summary>
/// This value is used to automatically use a 64 bit unsigned int.
/// </summary>
#define BITS uint64_t
#else
/// <summary>
/// This value is used to automatically use a 32 bit unsigned int.
/// </summary>
#define BITS uint32_t
#endif
#endif

#ifndef RENDERER
/// <summary>
/// This value determines the current graphics API.
/// </summary>
#define RENDERER Vulkan
#endif

#ifdef NDEBUG
#define RENDERER_DEMO 1
#endif

#define OpenGL 1
#define Vulkan 2

#if RENDERER == OpenGL
#define USING_OPENGL
#undef USING_VULKAN
#elif RENDERER == Vulkan
#undef USING_OPENGL
#define USING_VULKAN
#endif
