#pragma once

#include "Engine/Entity/Entity.hpp"

#include <cereal/cereal.hpp>

namespace Engine {
        template<class Archive>
        void Serialize(Archive& archive, Entity& entity) {
            archive(
                CEREAL_NVP(cereal::base_class<Asset>(&entity)),
                CEREAL_NVP(entity.components),
                CEREAL_NVP(entity.isActive)
            );
        }
} // namespace Engine

// Needed so Cereal knows which serialization function to use
namespace cereal {
    template <class Archive>
    struct specialize<Archive, Engine::Entity, cereal::specialization::non_member_serialize> {};
}