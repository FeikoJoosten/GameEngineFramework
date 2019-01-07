#pragma once

#include "Engine/api.hpp"
#include "Engine/Model/Model.hpp"
#include "Engine/Utility/Event.hpp"
#include "Engine/Utility/Defines.hpp"
#include <ThirdParty/glm/glm/glm.hpp>
#include <ThirdParty/EASTL-master/include/EASTL/shared_ptr.h>

namespace Engine
{
	/// <summary>
	/// This is the main renderer parent class which contains the base methods required for a renderer. NOTE: Only the Engine can create this object.
	/// </summary>
	class ENGINE_API Renderer
	{
	private:
		friend class Engine;
#ifdef USING_OPENGL
		friend class OpenGLRenderer;
#endif
#ifdef USING_VULKAN
		friend class VulkanRenderer;
#endif

		Renderer() noexcept;
		Renderer(Renderer const &other) = default;
		Renderer(Renderer &&other) noexcept = default;
		virtual ~Renderer() noexcept = default;
	public:

		//Should probably add the Shader reference for vertex and fragment to this, so they can be sent to the GPU
		//Possibly also want to add an Entity reference, so you could send it's data (mesh and texture) to the GPU as well.
		//At least that's how it worked in OpenGL
		/// <summary>
		/// This method is used to initialize the renderer for the current frame.
		/// </summary>
		virtual void RendererBegin();
		/// <summary>
		/// This method is used to initialize the renderer for the current frame.
		/// </summary>
		/// <param name="view">The view matrix used for this frame.</param>
		/// <param name="projection">The projection matrix used for this frame.</param>
		virtual void RendererBegin(const glm::mat4x4& view, const glm::mat4x4& projection);

		//Used to render the selected Shader & Entity combination defined in RendererBegin()
		/// <summary>
		/// This method is used to send draw data to the GPU.
		/// </summary>
		virtual void Render();
		/// <summary>
		/// This method is used to send draw data to the GPU.
		/// </summary>
		/// <param name="modelMatrix">The model matrix of the object you want to draw.</param>
		/// <param name="model">The model to render.</param>
		/// <param name="mainColor">The color you want to render your model in. By default this is set to white.</param>
		virtual void Render(const glm::mat4x4& modelMatrix, eastl::shared_ptr<Model> model, const glm::vec4& mainColor = glm::vec4(1, 1, 1, 1));

		//Used to unbind the current selected Shader & Entity combination defined in RendererBegin()
		/// <summary>
		/// This method is used to reset the current frame, so it's ready for the next frame.
		/// </summary>
		virtual void RendererEnd();

		/// <summary>
		/// This event is called whenever the Renderer is ready to start rendering.
		/// </summary>
		Sharp::Event<void> OnRender;
	};
} //namespace Engine