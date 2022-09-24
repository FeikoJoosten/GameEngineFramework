#pragma once

#include "Engine/Api/Api.hpp"

#include <guiddef.h>
#include <string>
#include <cereal/access.hpp>
#include <cereal/cereal.hpp>

namespace Engine {
	class ENGINE_API Asset {
		friend class cereal::access;
		Asset();

	protected:
		Asset(std::string name, std::string path = {});

	public:

		const std::string& GetName() const;

		void SetName(const std::string& newName);

		const std::string& GetPath() const;

		void SetPath(const std::string& newPath);

		const GUID& GetGuid() const;

	private:
		std::string name;
		std::string path;
		GUID guid{};

		template <class Archive>
		void Save(Archive& archive);

		template <class Archive>
		void Load(Archive& archive);
	};

	template <class Archive>
	void Asset::Save(Archive& archive) {
		
	}

	template <class Archive>
	void Asset::Load(Archive& archive) {
		
	}
}
