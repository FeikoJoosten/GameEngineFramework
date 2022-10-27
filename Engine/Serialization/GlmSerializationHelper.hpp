#pragma once

#include "Engine/Utility/Defines.hpp"

#include <cereal/cereal.hpp>
#include <glm/glm.hpp>

namespace glm {
    template<class Archive, class T>
    void Serialize(Archive& archive, glm::vec<2, T, glm::defaultp>& v) {
        archive(
            CEREAL_NVP(v.x),
            CEREAL_NVP(v.y)
        );
    }

    template<class Archive, class T>
    void Serialize(Archive& archive, glm::vec<3, T, glm::defaultp>& v) {
        archive(
            CEREAL_NVP(v.x),
            CEREAL_NVP(v.y),
            CEREAL_NVP(v.z)
        );
    }

    template<class Archive, class T>
    void Serialize(Archive& archive, glm::vec<4, T, glm::defaultp>& v) {
        archive(
            CEREAL_NVP(v.x),
            CEREAL_NVP(v.y),
            CEREAL_NVP(v.z),
            CEREAL_NVP(v.w)
        );
    }

    // glm matrices serialization
    template<class Archive, class T>
    void Serialize(Archive& archive, glm::mat<2, 2, T, glm::defaultp>& m) {
        archive(
            CEREAL_NVP(m[0]),
            CEREAL_NVP(m[1])
        );
    }

    template<class Archive, class T>
    void Serialize(Archive& archive, glm::mat<3, 3, T, glm::defaultp>& m) {
        archive(
            CEREAL_NVP(m[0]),
            CEREAL_NVP(m[1]),
            CEREAL_NVP(m[2])
        );
    }

    template<class Archive, class T>
    void Serialize(Archive& archive, glm::mat<4, 4, T, glm::defaultp>& m) {
	    archive(
            CEREAL_NVP(m[0]), 
            CEREAL_NVP(m[1]), 
            CEREAL_NVP(m[2]), 
            CEREAL_NVP(m[3])
        );
    }

    template<class Archive, class T>
    void Serialize(Archive& archive, glm::qua<T, glm::defaultp>& q) {
        archive(
            CEREAL_NVP(q.x),
            CEREAL_NVP(q.y),
            CEREAL_NVP(q.z),
            CEREAL_NVP(q.w)
        );
    }
}