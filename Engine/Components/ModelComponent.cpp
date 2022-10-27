#include "Engine/Components/ModelComponent.hpp"
#include "Engine/Utility/Time.hpp"

namespace Engine
{
	void ModelComponent::Update()
	{
		if (time.expired()) return; // Should never happen, but just to be safe
		const std::shared_ptr<Model>& model = GetModel();
		if (!model)
			return;

		model->Update(time.lock()->GetDeltaTime());
	}
} //namespace Engine