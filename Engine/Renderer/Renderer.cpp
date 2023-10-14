#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Engine/Engine.hpp"
#include "Engine/Entity/Entity.hpp"
#include "Engine/Entity/EntitySystem.hpp"
#include "Engine/Components/RenderComponent.hpp"
#include "Engine/Components/TransformComponent.hpp"

// Required for configuring stb image library
#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif
#include <stb/stb_image.h>

namespace Engine {
	Renderer::Renderer() noexcept {
		entitySystem = EntitySystem::Get();
		entitySystem->OnComponentAddedToEntityEvent += Sharp::EventHandler::Bind(this, &Renderer::HandleOnComponentAddedToEntityEvent);
		entitySystem->OnComponentRemovedFromEntityEvent += Sharp::EventHandler::Bind(this, &Renderer::HandleOnComponentRemovedFromEntityEvent);

		for(std::shared_ptr<RenderComponent>& renderComponent : entitySystem->GetAllComponents<RenderComponent>())
			renderComponents.push_back(renderComponent);
	}

	Renderer::~Renderer() {
		if (entitySystem)
			entitySystem->OnComponentAddedToEntityEvent -= Sharp::EventHandler::Bind(this, &Renderer::HandleOnComponentAddedToEntityEvent);
		entitySystem->OnComponentRemovedFromEntityEvent -= Sharp::EventHandler::Bind(this, &Renderer::HandleOnComponentRemovedFromEntityEvent);

		renderComponents.clear();
	}

	std::shared_ptr<Renderer> Renderer::Get() {
		return Engine::GetRenderer();
	}

	void Renderer::RendererBegin(const glm::mat4x4& view, const glm::mat4x4& projection) {}

	void Renderer::RendererEnd() {}

	void Renderer::RenderFrame(const glm::mat4x4& view, const glm::mat4x4& projection) {
		RendererBegin(view, projection);

		for (const std::weak_ptr<RenderComponent>& renderComponent : renderComponents) {
			if (renderComponent.expired()) continue;

			const std::shared_ptr<RenderComponent> lockedRenderComponent = renderComponent.lock();
			if (!lockedRenderComponent->GetIsActiveAndEnabled()) continue;

			// TODO: Add sorting based on same material
			// Then 'activate' the material so properties like color, texture, etc can be sent to the GPU

			lockedRenderComponent->Render();
		}
		PostRenderComponentsRenderEvent();

		RendererEnd();
	}

	void Renderer::HandleOnComponentAddedToEntityEvent([[maybe_unused]] std::shared_ptr<Entity> entity, const std::shared_ptr<Component> addedComponent) {
		const std::shared_ptr<RenderComponent> renderComponent = std::dynamic_pointer_cast<RenderComponent>(addedComponent);
		if (!renderComponent) return;

		renderComponents.push_back(renderComponent);
	}

	void Renderer::HandleOnComponentRemovedFromEntityEvent([[maybe_unused]] const std::shared_ptr<Entity> entity, const std::shared_ptr<Component> removedComponent) {
		const std::shared_ptr<RenderComponent> renderComponent = std::dynamic_pointer_cast<RenderComponent>(removedComponent);
		if (!renderComponent) return;

		std::erase_if(renderComponents,
		              [renderComponent](const std::weak_ptr<RenderComponent>& component) {
			              return !component.expired() && component.lock().get() == renderComponent.get();
		              });
	}
}