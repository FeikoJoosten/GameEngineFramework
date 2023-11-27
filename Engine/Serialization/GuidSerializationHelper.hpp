#pragma once

#include <cereal/cereal.hpp>
#include <crossguid/guid.hpp>
#include <nameof.hpp>

namespace xg {
    template<class Archive>
    void Load(Archive& archive, Guid& guid) {
        std::string string;
        archive(cereal::make_nvp(static_cast<std::string>(NAMEOF(guid)), string));
        guid = Guid(string);
    }

    template<class Archive>
    void Save(Archive& archive, const Guid& guid) {
        archive(cereal::make_nvp(static_cast<std::string>(NAMEOF(guid)), guid.str()));
    }
}