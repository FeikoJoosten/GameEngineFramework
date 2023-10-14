#include "Engine/Model/Model.hpp"

CEREAL_REGISTER_TYPE(Engine::Model);

namespace Engine {
	std::vector<std::shared_ptr<Entity>> Model::GetEntities() {
		return entities;
	}
}
