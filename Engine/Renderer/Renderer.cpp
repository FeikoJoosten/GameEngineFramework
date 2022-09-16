#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Entity/EntitySystem.hpp"
#include "Engine/Components/RenderComponent.hpp"
#include "Engine/Components/TransformComponent.hpp"

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif
#include <stb/stb_image.h>

namespace Engine
{
	Renderer::Renderer() noexcept
	{
		entitySystem = EntitySystem::Get();
		if (entitySystem)
			entitySystem->OnComponentAddedToEntityEvent += Sharp::EventHandler::Bind(this, &Renderer::HandleOnComponentAddedToEntityEvent);
	}

	Renderer::~Renderer() {
		if (entitySystem)
			entitySystem->OnComponentAddedToEntityEvent -= Sharp::EventHandler::Bind(this, &Renderer::HandleOnComponentAddedToEntityEvent);
	}

	std::shared_ptr<Renderer> Renderer::Get() {
		return Engine::GetRenderer();
	}

	void Renderer::RendererBegin(const glm::mat4x4& view, const glm::mat4x4& projection)
	{
	}

	void Renderer::RenderFrame(const glm::mat4x4& view, const glm::mat4x4& projection)
	{
		RendererBegin(view, projection);

		for(const std::weak_ptr<RenderComponent>& renderComponent : renderComponents) {
			if (renderComponent.expired()) continue;

			const std::shared_ptr<RenderComponent> lockedRenderComponent = renderComponent.lock();
			if (!lockedRenderComponent->GetIsEnabled() || !lockedRenderComponent->GetOwner().lock()->GetIsActive()) continue;
			if (lockedRenderComponent->transformComponent.expired()) continue;
			const std::shared_ptr<TransformComponent> transformComponent = lockedRenderComponent->transformComponent.lock();
			if (!transformComponent->GetIsEnabled()) continue;

			// TODO: Add support for rendering with materials
			Render(transformComponent->GetModelMatrix(), lockedRenderComponent->GetModel());
		}
		PostRenderComponentsRenderEvent();

		RendererEnd();
	}

	void Renderer::Render(const glm::mat4x4& modelMatrix, std::shared_ptr<Model> model, const glm::vec4& mainColor)
	{
	}

	void Renderer::RendererEnd()
	{
	}

	void Renderer::HandleOnComponentAddedToEntityEvent(std::shared_ptr<Entity> entity, std::shared_ptr<Component> addedComponent) {
		if (const std::shared_ptr<RenderComponent> renderComponent = std::dynamic_pointer_cast<RenderComponent>(addedComponent)) 
			renderComponents.push_back(renderComponent);
	}

	void Renderer::HandleOnComponentRemovedFromEntityEvent(std::shared_ptr<Entity> entity, std::shared_ptr<Component> removedComponent) {
		const std::shared_ptr<RenderComponent> renderComponent = std::dynamic_pointer_cast<RenderComponent>(removedComponent);
		if (!renderComponent) return;

		renderComponents.erase(
			std::remove_if(renderComponents.begin(), renderComponents.end(),
				[renderComponent](const std::weak_ptr<RenderComponent>& component) {
					return !component.expired() && component.lock().get() == renderComponent.get();
				}), renderComponents.end());
	}
} // namespace Engine