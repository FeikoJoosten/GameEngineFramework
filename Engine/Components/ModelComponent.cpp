#include "Engine/Components/ModelComponent.hpp"
#include "Engine/Resources/ModelLoader.hpp"

namespace Engine
{
	ModelComponent::ModelComponent(Model * newModel) : model(newModel) {}

	ModelComponent::ModelComponent(eastl::string path) : model(ModelLoader::CreateNewModel(path)) {}

	ModelComponent::~ModelComponent() {  }
} //namespace Engine