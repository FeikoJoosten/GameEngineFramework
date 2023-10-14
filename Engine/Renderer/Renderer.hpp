#pragma once

#include "Engine/Api/Api.hpp"
#include "Engine/Application/Application.hpp"
#include "Engine/Engine/Engine.hpp"
#include "Engine/Utility/Event.hpp"
#include <glm/glm.hpp>

namespace Engine
{
	class Model;
	class RenderComponent;
	class Component;
	class Entity;
	class EntitySystem;

	/// <summary>
	/// This is the main renderer parent class which contains the base methods required for a renderer. NOTE: Only the Engine can create this object.
	/// </summary>
	class ENGINE_API Renderer
	{
		friend void Application::Update();

	protected:
		Renderer() noexcept;

	public:
		/// <summary>
		/// This event is called whenever the Renderer is ready to start rendering.
		/// </summary>
		Sharp::Event<> PostRenderComponentsRenderEvent;

		Renderer(const Renderer&other) = delete;
		Renderer(Renderer &&other) noexcept = delete;
		Renderer& operator=(const Renderer& other) = delete;
		Renderer& operator=(Renderer&& other) noexcept = delete;
		virtual ~Renderer();

		static std::shared_ptr<Renderer> Get();

		template<typename RendererType>
		static std::shared_ptr<RendererType> Get();

	protected:
		std::vector<std::weak_ptr<RenderComponent>> renderComponents;

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

	private:
		std::shared_ptr<EntitySystem> entitySystem;

		//Used to render the selected Shader & Entity combination defined in RendererBegin()
		/// <summary>
		/// This method is used to send draw data to the GPU.
		/// </summary>
		/// <param name="view">The view matrix used for this frame.</param>
		/// <param name="projection">The projection matrix used for this frame.</param>
		void RenderFrame(const glm::mat4x4& view, const glm::mat4x4& projection);

		void HandleOnComponentAddedToEntityEvent(std::shared_ptr<Entity> entity, std::shared_ptr<Component> addedComponent);
		void HandleOnComponentRemovedFromEntityEvent(std::shared_ptr<Entity> entity, std::shared_ptr<Component> removedComponent);
	};

	template <typename RendererType> std::shared_ptr<RendererType> Renderer::Get() {
		return Engine::GetRenderer<RendererType>();
	}
} //namespace Engine