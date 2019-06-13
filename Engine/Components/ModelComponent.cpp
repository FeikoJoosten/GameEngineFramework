#include "Engine/Components/ModelComponent.hpp"
#include "Engine/Components/TransformComponent.hpp"
#include "Engine/engine.hpp"

namespace Engine
{
	ModelComponent::ModelComponent(eastl::shared_ptr<Model> newModel) noexcept : Component()
	{
		model = newModel;
		isEnabled = true;
	}

	ModelComponent::ModelComponent(eastl::string path) noexcept : Component()
	{
		this->path = path;
		isEnabled = true;
	}

	void ModelComponent::InitializeComponent()
	{
		SetModel(path);

		Engine::GetEngine().lock()->GetRenderer().lock()->OnRender += Sharp::EventHandler::Bind(&ModelComponent::Render, this);
	}

	ModelComponent::~ModelComponent()
	{
		Engine::GetEngine().lock()->GetRenderer().lock()->OnRender -= Sharp::EventHandler::Bind(&ModelComponent::Render, this);
	}

	void ModelComponent::SetModel(eastl::shared_ptr<Model> newModel)
	{
		model = newModel;
	}

	void ModelComponent::SetModel(const eastl::string& path)
	{
		// Shortcut
		eastl::weak_ptr<ResourceManager> resourceManager = Engine::GetEngine().lock()->GetResourceManager();

		// Get Model
		eastl::weak_ptr<Model> model = resourceManager.lock()->CreateModel(path, path);

		if (model.expired() == false && model.lock().get() != nullptr)
		{
			this->path = path;
			this->model = model;
		}
	}

	eastl::weak_ptr<Model> ModelComponent::GetModel() const
	{
		return model;
	}

	void ModelComponent::Update()
	{
		if (model.expired() || isEnabled == false || GetOwner().lock()->GetIsActive() == false)
			return;

		if (transformComponent.expired())
			return;

		GetModel().lock()->Update(Engine::GetEngine().lock()->GetTime().lock()->GetDeltaTime());
	}

	void ModelComponent::Render()
	{
		if (model.expired() || isEnabled == false || GetOwner().lock()->GetIsActive() == false)
			return;

		if (transformComponent.expired())
			return;

		Engine::GetEngine().lock()->GetRenderer().lock()->Render(transformComponent.lock()->GetModelMatrix(), Engine::GetEngine().lock()->GetResourceManager().lock()->GetModel(model.lock()->GetName()).lock());
	}

	void ModelComponent::OnComponentAdded(eastl::weak_ptr<Component> addedComponent)
	{
		if (eastl::dynamic_pointer_cast<TransformComponent>(addedComponent.lock()) && transformComponent.expired())
		{
			transformComponent = eastl::static_pointer_cast<TransformComponent>(addedComponent.lock());
		}
	}

	void ModelComponent::OnComponentRemoved(eastl::weak_ptr<Component> removedComponent)
	{
		if (transformComponent.lock().get() == removedComponent.lock().get())
			transformComponent.reset();

		//In case we have multiple transformcomponents on this entity for some reason.
		transformComponent = GetComponent<TransformComponent>();
	}

	template <typename archive>
	void ModelComponent::SaveComponent(archive ar)
	{
		ar(CEREAL_NVP(model), CEREAL_NVP(transformComponent), CEREAL_NVP(isEnabled));
	}

	template <typename archive>
	void ModelComponent::LoadComponent(archive ar)
	{
		ar(model, transformComponent, isEnabled);
	}
} //namespace Engine