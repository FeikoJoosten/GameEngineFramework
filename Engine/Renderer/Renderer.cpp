#include "Engine/Renderer/Renderer.hpp"

#if !defined STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif
#include <ThirdParty/stb/stb_image.h>

namespace Engine
{
	Renderer::Renderer() noexcept
	{
	}

	void Renderer::RendererBegin()
	{
	}

	void Renderer::RendererBegin(const glm::mat4x4& view, const glm::mat4x4& projection)
	{
	}

	void Renderer::Render()
	{
	}

	void Renderer::Render(const glm::mat4x4& modelMatrix, eastl::shared_ptr<Model> model, const glm::vec4& mainColor)
	{
	}

	void Renderer::RendererEnd()
	{
	}
} // namespace Engine