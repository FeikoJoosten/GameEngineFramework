#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Engine/Engine.hpp"
#include "Engine/Entity/Entity.hpp"
#include "Engine/Entity/EntitySystem.hpp"
#include "Engine/Components/MeshRendererComponent.hpp"
#include "Engine/Components/RenderComponent.hpp"
#include "Engine/Components/TransformComponent.hpp"
#include "Engine/Renderer/RenderCommand.hpp"

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

		for (const std::shared_ptr<RenderComponent>& renderComponent : renderComponents)
			renderComponent->OnEnabledStateChangedEvent -= Sharp::EventHandler::Bind(this, &Renderer::HandleOnRenderComponentEnabledStateChangedEvent);
		renderComponents.clear();

		for (const std::shared_ptr<RenderComponent>& renderComponent : activeRenderComponents)
			renderComponent->OnMaterialChangedEvent -= Sharp::EventHandler::Bind(this, &Renderer::HandleOnRenderComponentMaterialChangedEvent);
		activeRenderComponents.clear();
	}

	std::shared_ptr<Renderer> Renderer::Get() {
		return Engine::GetRenderer();
	}

	void Renderer::RenderMesh(const std::shared_ptr<Mesh>& meshToRender, const std::shared_ptr<CameraComponent>& camera, const std::shared_ptr<Material>& material, const glm::mat4x4 modelMatrix) {
		if (const std::map<std::shared_ptr<CameraComponent>, std::vector<RenderCommand>>::iterator iterator = renderCommandsByCamera.find(camera); iterator == renderCommandsByCamera.end())
			renderCommandsByCamera[camera] = std::vector<RenderCommand>();

		std::vector<RenderCommand>& queuedCommands = renderCommandsByCamera[camera];
		queuedCommands.emplace_back(meshToRender, material, modelMatrix);
	}

	void Renderer::OnActiveRenderComponentAdded(const std::shared_ptr<RenderComponent>& addedComponent) {
		addedComponent->OnMaterialChangedEvent += Sharp::EventHandler::Bind(this, &Renderer::HandleOnRenderComponentMaterialChangedEvent);

		if(const std::shared_ptr<MeshRendererComponent> meshRendererComponent = std::dynamic_pointer_cast<MeshRendererComponent>(addedComponent)) {
			meshRendererComponent->OnMeshChangedEvent += Sharp::EventHandler::Bind(this, &Renderer::HandleOnMeshRendererComponentMeshChangedEvent);

			if (const std::shared_ptr<Mesh> meshRendererComponentMesh = meshRendererComponent->GetMesh(); meshRendererComponentMesh) 
				HandleOnMeshRendererComponentMeshChangedEvent(meshRendererComponent, nullptr, meshRendererComponentMesh);
		}

		const std::shared_ptr<Material>& renderComponentMaterial = addedComponent->GetMaterial();
		if (!renderComponentMaterial) return;

		HandleOnRenderComponentMaterialChangedEvent(addedComponent, nullptr, renderComponentMaterial);
	}

	void Renderer::OnActiveRenderComponentRemoved(const std::shared_ptr<RenderComponent>& removedComponent) {
		removedComponent->OnMaterialChangedEvent -= Sharp::EventHandler::Bind(this, &Renderer::HandleOnRenderComponentMaterialChangedEvent);

		HandleOnRenderComponentMaterialChangedEvent(removedComponent, removedComponent->GetMaterial(), nullptr);
	}

	void Renderer::HandleOnMeshRendererComponentMeshChangedEvent(std::shared_ptr<MeshRendererComponent> meshRendererComponent, std::shared_ptr<Mesh> oldMesh, std::shared_ptr<Mesh> newMesh) {}

	void Renderer::HandleOnRenderComponentMaterialChangedEvent(std::shared_ptr<RenderComponent> renderComponent, std::shared_ptr<Material> oldMaterial, std::shared_ptr<Material> newMaterial) {}

	void Renderer::RenderFrame(const std::shared_ptr<CameraComponent>& camera) {
		RendererBegin();

		renderCommandsByCamera.clear();
		for (const std::shared_ptr<RenderComponent>& renderComponent : activeRenderComponents)
			renderComponent->Render(camera);

		for (const std::pair<const std::shared_ptr<CameraComponent>, std::vector<RenderCommand>>& commandsByCamera : renderCommandsByCamera)
			RenderCommands(commandsByCamera.first, commandsByCamera.second);

		PostRenderComponentsRenderEvent();

		RendererEnd();
	}

	void Renderer::HandleOnComponentAddedToEntityEvent([[maybe_unused]] std::shared_ptr<Entity> entity, const std::shared_ptr<Component> addedComponent) {
		const std::shared_ptr<RenderComponent> renderComponent = std::dynamic_pointer_cast<RenderComponent>(addedComponent);
		if (!renderComponent) return;

		renderComponents.push_back(renderComponent);
		renderComponent->OnEnabledStateChangedEvent += Sharp::EventHandler::Bind(this, &Renderer::HandleOnRenderComponentEnabledStateChangedEvent);

		HandleOnRenderComponentEnabledStateChangedEvent(renderComponent, renderComponent->GetIsEnabled());
	}

	void Renderer::HandleOnComponentRemovedFromEntityEvent([[maybe_unused]] const std::shared_ptr<Entity> entity, const std::shared_ptr<Component> removedComponent) {
		const std::shared_ptr<RenderComponent> renderComponent = std::dynamic_pointer_cast<RenderComponent>(removedComponent);
		if (!renderComponent) return;

		std::ranges::borrowed_iterator_t<std::vector<std::shared_ptr<RenderComponent>>&> iterator = std::ranges::find(renderComponents, renderComponent);
		if (iterator == renderComponents.end()) return;

		renderComponents.erase(iterator);
		renderComponent->OnEnabledStateChangedEvent -= Sharp::EventHandler::Bind(this, &Renderer::HandleOnRenderComponentEnabledStateChangedEvent);

		iterator = std::ranges::find(activeRenderComponents, renderComponent);
		if (iterator == activeRenderComponents.end()) return;

		activeRenderComponents.erase(iterator);
		OnActiveRenderComponentRemoved(*iterator);
	}

	void Renderer::HandleOnRenderComponentEnabledStateChangedEvent(std::shared_ptr<Component> component, bool isEnabled) {
		const std::shared_ptr<RenderComponent> renderComponent = std::static_pointer_cast<RenderComponent>(component);
		if (!renderComponent) return; // Should never happen, but just to be safe

		const bool isActiveAndEnabled = component->GetIsActiveAndEnabled();
		const std::ranges::borrowed_iterator_t<std::vector<std::shared_ptr<RenderComponent>>&> iterator = std::ranges::find(activeRenderComponents, renderComponent);
		if (iterator == activeRenderComponents.end()) {
			if (!isActiveAndEnabled) return; // No need to do anything, the component wasn't added to the active render components vector and isn't active

			activeRenderComponents.push_back(renderComponent);
			OnActiveRenderComponentAdded(renderComponent);
			return;
		}

		if (isActiveAndEnabled) return; // No need to do anything, the component is active and is already added to the active render components vector

		activeRenderComponents.erase(iterator);
		OnActiveRenderComponentRemoved(renderComponent);
	}

}
