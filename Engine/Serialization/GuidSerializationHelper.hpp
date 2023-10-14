#pragma once

#include <cereal/cereal.hpp>
#include <crossguid/guid.hpp>

namespace xg {
    template<class Archive>
    void Load(Archive& archive, Guid& guid) {
    	std::array<unsigned char, 16> bytes {};

        archive(
            cereal::make_nvp("bytes", bytes)
        );

        guid = Guid(bytes);
    }

    template<class Archive>
    void Save(Archive& archive, const Guid& guid) {
        archive(
            cereal::make_nvp("bytes", guid.bytes())
        );
    }
}