#pragma once

#ifndef RENDERER
/// <summary>
/// This value determines the current graphics API.
/// </summary>
#define RENDERER OpenGL
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

#ifndef GLM_FORCE_LEFT_HANDED
#define GLM_FORCE_LEFT_HANDED
#endif

#ifndef GLM_FORCE_XYZW_ONLY
#define GLM_FORCE_XYZW_ONLY
#endif

#include "Engine/Serialization/SerializationHelpers.hpp"

#include <cereal/types/polymorphic.hpp>
#include <cereal/archives/json.hpp>