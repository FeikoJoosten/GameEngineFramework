#pragma once

#include "Engine/api.hpp"
#include "Engine/Model/Model.h"
#include "Engine/Utility/Event.hpp"
#include "Engine/Renderer/IMGUI/imgui.h"
#include <ThirdParty/glm/glm/glm.hpp>

namespace Engine
{
	class ENGINE_API Renderer
	{
	public:

		Renderer() noexcept;
		Renderer(Renderer const &other) = default;
		Renderer(Renderer &&other) noexcept = default;
		virtual ~Renderer() noexcept = default;

		//Should probably add the Shader reference for vertex and fragment to this, so they can be sent to the GPU
		//Possibly also want to add an Entity reference, so you could send it's data (mesh and texture) to the GPU as well.
		//At least that's how it worked in OpenGL
		virtual void RendererBegin();
		virtual void RendererBegin(const glm::mat4x4& view, const glm::mat4x4& projection);

		//Used to render the selected Shader & Entity combination defined in RendererBegin()
		virtual void Render();
		virtual void Render(const glm::mat4x4& modelMatrix, Model* model, const glm::vec4& mainColor = glm::vec4(1, 1, 1, 1));

		//Used to unbind the current selected Shader & Entity combination defined in RendererBegin()
		virtual void RendererEnd();

		Sharp::Event<void> OnRender;
	};
} //namespace Engine