#pragma once

#include "Engine/Api/Api.hpp"
#include "Engine/Application/Application.hpp"
#include "Engine/Engine/Engine.hpp"
#include "Engine/Utility/Event.hpp"
#include "Engine/Renderer/RenderCommand.hpp"
#include <glm/glm.hpp>

namespace Engine {
	class CameraComponent;
	class Component;
	class Entity;
	class EntitySystem;
	class Material;
	class Mesh;
	class Model;
	class MeshRendererComponent;
	class RenderComponent;
	class Shader;
	class Texture;

	class ENGINE_API Renderer {
		friend void Application::Update();

		std::shared_ptr<EntitySystem> entitySystem;
		std::vector<std::shared_ptr<RenderComponent>> renderComponents;
		inline static std::map<std::shared_ptr<CameraComponent>, std::vector<RenderCommand>> renderCommandsByCamera {};

	protected:
		std::vector<std::shared_ptr<RenderComponent>> activeRenderComponents;

		Renderer() noexcept;

	public:
		Sharp::Event<> PostRenderComponentsRenderEvent;

		Renderer(const Renderer& other) = delete;
		Renderer(Renderer&& other) noexcept = delete;
		Renderer& operator=(const Renderer& other) = delete;
		Renderer& operator=(Renderer&& other) noexcept = delete;
		virtual ~Renderer();

		static std::shared_ptr<Renderer> Get();

		virtual void InitializeTextureForRendering(std::shared_ptr<Texture> textureToInitialize) = 0;

		virtual void TryExtractShaderParameters(const std::shared_ptr<Shader>& shaderToLoadParametersFrom) = 0;

		static void RenderMesh(const std::shared_ptr<Mesh>& meshToRender, const std::shared_ptr<CameraComponent>& camera, const std::shared_ptr<Material>& material, glm::mat4x4 modelMatrix);

	protected:

		virtual void RendererBegin() = 0;

		virtual void RenderCommands(const std::shared_ptr<CameraComponent>& camera, const std::vector<RenderCommand>& commands) = 0;

		virtual void RendererEnd() = 0;

		virtual void OnActiveRenderComponentAdded(const std::shared_ptr<RenderComponent>& addedComponent);

		virtual void OnActiveRenderComponentRemoved(const std::shared_ptr<RenderComponent>& removedComponent);

		virtual void HandleOnMeshRendererComponentMeshChangedEvent(std::shared_ptr<MeshRendererComponent> meshRendererComponent, std::shared_ptr<Mesh> oldMesh, std::shared_ptr<Mesh> newMesh);

		virtual void HandleOnRenderComponentMaterialChangedEvent(std::shared_ptr<RenderComponent> renderComponent, std::shared_ptr<Material> oldMaterial, std::shared_ptr<Material> newMaterial);

	private:

		void RenderFrame(const std::shared_ptr<CameraComponent>& camera);

		void HandleOnComponentAddedToEntityEvent(std::shared_ptr<Entity> entity, std::shared_ptr<Component> addedComponent);

		void HandleOnComponentRemovedFromEntityEvent(std::shared_ptr<Entity> entity, std::shared_ptr<Component> removedComponent);

		void HandleOnRenderComponentEnabledStateChangedEvent(std::shared_ptr<Component> component, bool isEnabled);

	};
} //namespace Engine