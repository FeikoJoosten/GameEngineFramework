#pragma once

#include "Engine/Api.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Model/Model.hpp"
#include "Engine/Utility/Event.hpp"
#include "Engine/Utility/Defines.hpp"
#include <glm/glm.hpp>

namespace Engine
{
	class RenderComponent;
	class Component;
	class Entity;
	class EntitySystem;

	/// <summary>
	/// This is the main renderer parent class which contains the base methods required for a renderer. NOTE: Only the Engine can create this object.
	/// </summary>
	class ENGINE_API Renderer
	{
#ifdef USING_OPENGL
		friend class OpenGLRenderer;
#endif
#ifdef USING_VULKAN
		friend class VulkanRenderer;
#endif

		Renderer() noexcept;
	public:

		Renderer(const Renderer&other) = delete;
		Renderer(Renderer &&other) noexcept = delete;
		Renderer& operator=(const Renderer& other) = delete;
		Renderer& operator=(Renderer&& other) noexcept = delete;
		virtual ~Renderer();

		static std::shared_ptr<Renderer> Get();

		template<typename RendererType>
		static std::shared_ptr<RendererType> Get();

		//Used to render the selected Shader & Entity combination defined in RendererBegin()
		/// <summary>
		/// This method is used to send draw data to the GPU.
		/// </summary>
		/// <param name="view">The view matrix used for this frame.</param>
		/// <param name="projection">The projection matrix used for this frame.</param>
		void RenderFrame(const glm::mat4x4& view, const glm::mat4x4& projection);

		// TODO: Get rid of this method and instead retrieve the data from the renderer component. This way the rendering queue can be optimized
		/// <summary>
		/// This method is used to send draw data to the GPU.
		/// </summary>
		/// <param name="modelMatrix">The model matrix of the object you want to draw.</param>
		/// <param name="model">The model to render.</param>
		/// <param name="mainColor">The color you want to render your model in. By default this is set to white.</param>
		virtual void Render(const glm::mat4x4& modelMatrix, std::shared_ptr<Model> model, const glm::vec4& mainColor = glm::vec4(1, 1, 1, 1));

		/// <summary>
		/// This event is called whenever the Renderer is ready to start rendering.
		/// </summary>
		Sharp::Event<> PostRenderComponentsRenderEvent;

	protected:
		/// <summary>
		/// This method is used to initialize the renderer for the current frame.
		/// </summary>
		/// <param name="view">The view matrix used for this frame.</param>
		/// <param name="projection">The projection matrix used for this frame.</param>
		virtual void RendererBegin(const glm::mat4x4& view, const glm::mat4x4& projection);

		//Used to unbind the current selected Shader & Entity combination defined in RendererBegin()
		/// <summary>
		/// This method is used to reset the current frame, so it's ready for the next frame.
		/// </summary>
		virtual void RendererEnd();

		std::vector<std::weak_ptr<RenderComponent>> renderComponents;

	private:

		void HandleOnComponentAddedToEntityEvent(std::shared_ptr<Entity> entity, std::shared_ptr<Component> addedComponent);
		void HandleOnComponentRemovedFromEntityEvent(std::shared_ptr<Entity> entity, std::shared_ptr<Component> removedComponent);

		std::shared_ptr<EntitySystem> entitySystem;
	};

	template <typename RendererType> std::shared_ptr<RendererType> Renderer::Get() {
		return Engine::GetRenderer<RendererType>();
	}
} //namespace Engine