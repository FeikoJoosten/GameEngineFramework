#pragma once
#include "Engine/Api/Api.hpp"
#include "Engine/AssetManagement/Asset.hpp"
#include "Engine/Components/Component.hpp"
#include "Engine/Entity/Entity.hpp"
#include "Engine/Utility/Defines.hpp"

#include <cereal/types/base_class.hpp>

namespace Engine
{
	/// <summary>
	/// This object is a storage container for meshes.
	/// </summary>
	class ENGINE_API Model final : public Asset
	{
		friend cereal::access;
		friend class ModelAssetImporter;

		explicit Model() = default;

		std::vector<std::shared_ptr<Entity>> entities {};

	public:
		virtual ~Model() override = default;
		Model(const Model& other) = delete;
		Model(Model&& other) = delete;
		Model& operator=(const Model& other) = delete;
		Model& operator=(Model&& other) = delete;

		std::vector<std::shared_ptr<Entity>> GetEntities();

	private:
		template <class Archive>
		void Serialize(Archive & archive);

#pragma endregion
	};

	template <class Archive>
	void Model::Serialize(Archive& archive) {
		archive(
			CEREAL_NVP(cereal::base_class<Asset>(this)),
			CEREAL_NVP(entities)
		);
	}
}
