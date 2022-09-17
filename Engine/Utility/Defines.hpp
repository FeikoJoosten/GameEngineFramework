#pragma once

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

#ifndef CEREAL_SERIALIZE_FUNCTION_NAME
#define CEREAL_SERIALIZE_FUNCTION_NAME Serialize
#endif
#ifndef CEREAL_LOAD_FUNCTION_NAME
#define CEREAL_LOAD_FUNCTION_NAME Load
#endif
#ifndef CEREAL_SAVE_FUNCTION_NAME
#define CEREAL_SAVE_FUNCTION_NAME Save
#endif
#ifndef CEREAL_LOAD_MINIMAL_FUNCTION_NAME
#define CEREAL_LOAD_MINIMAL_FUNCTION_NAME LoadMinimal
#endif
#ifndef CEREAL_SAVE_MINIMAL_FUNCTION_NAME
#define CEREAL_SAVE_MINIMAL_FUNCTION_NAME SaveMinimal
#endif

#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#endif
