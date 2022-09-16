#include "Engine/Components/ModelComponent.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Utility/Time.hpp"

namespace Engine
{
	ModelComponent::ModelComponent(std::shared_ptr<Model> model) noexcept : RenderComponent(std::move(model)), time(Time::Get()) {
	}

	ModelComponent::ModelComponent(const std::string& path) noexcept : RenderComponent(path), time(Time::Get()) {
	}

	void ModelComponent::Update()
	{
		if (time.expired()) return; // Should never happen, but just to be safe
		const std::shared_ptr<Model>& model = GetModel();
		if (!model)
			return;

		model->Update(time.lock()->GetDeltaTime());
	}
} //namespace Engine