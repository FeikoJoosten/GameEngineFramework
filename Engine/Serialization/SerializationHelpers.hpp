#pragma once

#include "Engine/Serialization/GlmSerializationHelper.hpp"
#include "Engine/Serialization/GuidSerializationHelper.hpp"

namespace cereal {
    template<class Archive, class F, class S>
    void Serialize(Archive& archive, std::pair<F, S>& pair) {
        archive(
            pair.first, 
            pair.second
        );
    }

    template <class Archive, class F, class S>
    struct specialize<Archive, std::pair<F, S>, cereal::specialization::non_member_serialize> {};
}