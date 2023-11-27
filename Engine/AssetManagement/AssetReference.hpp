#pragma once

#include "Engine/Api/Api.hpp"
#include "Engine/AssetManagement/Asset.hpp"
#include "Engine/AssetManagement/AssetManager.hpp"
#include "Engine/Utility/Defines.hpp"

#include <cereal/access.hpp>

namespace Engine {
	template <typename T>
	class ENGINE_API AssetReference final {
		static_assert(std::is_base_of_v<Asset, T>, "The T type must be of type Asset");

		friend cereal::access;

		std::shared_ptr<T> assetReference = nullptr;


	public:
		explicit AssetReference() = default;
		explicit AssetReference(const xg::Guid& assetGuid);
		explicit AssetReference(const std::shared_ptr<T>& assetReference);
		~AssetReference() = default;
		AssetReference(const AssetReference& other) = delete;
		AssetReference(AssetReference&& other) = delete;
		AssetReference& operator=(const AssetReference& other) = delete;
		AssetReference& operator=(AssetReference&& other) = delete;
		AssetReference& operator=(const std::shared_ptr<T>& newAssetReference);
		AssetReference& operator=(const xg::Guid& assetGuid);
		bool operator==(const std::shared_ptr<T>& otherAsset) const;
		bool operator==(const xg::Guid& otherAssetGuid) const;

		operator std::shared_ptr<T>() const { return assetReference; }
		operator const std::shared_ptr<T>& () { return assetReference; }
		operator xg::Guid() const { return assetReference ? assetReference->GetGuid() : xg::Guid(); }
		operator const xg::Guid& () { return assetReference ? assetReference->GetGuid() : xg::Guid(); }
		explicit operator bool() const { return assetReference.get(); }

	private:

		template<class Archive>
		void Save(Archive& archive) const;

		template<class Archive>
		void Load(Archive& archive);
	};

	template <typename T>
	AssetReference<T>::AssetReference(const xg::Guid& assetGuid) {
		const std::shared_ptr<AssetManager>& assetManager = AssetManager::Get();
		assetReference = assetManager->LoadAsset<T>(assetGuid);
	}

	template <typename T>
	AssetReference<T>::AssetReference(const std::shared_ptr<T>& assetReference) {
		this->assetReference = assetReference;
	}

	template <typename T>
	AssetReference<T>& AssetReference<T>::operator=(const std::shared_ptr<T>& newAssetReference) {
		this->assetReference = newAssetReference;
		return *this;
	}

	template <typename T>
	AssetReference<T>& AssetReference<T>::operator=(const xg::Guid& assetGuid) {
		const std::shared_ptr<AssetManager>& assetManager = AssetManager::Get();
		assetReference = assetManager->LoadAsset<T>(assetGuid);
		return *this;
	}

	template <typename T>
	bool AssetReference<T>::operator==(const std::shared_ptr<T>& otherAsset) const {
		if (otherAsset == assetReference) return true;
		if (otherAsset && assetReference) return otherAsset.get() == assetReference.get();
		return false;
	}

	template <typename T>
	bool AssetReference<T>::operator==(const xg::Guid& otherAssetGuid) const {
		return (assetReference ? assetReference->GetGuid() : xg::Guid()) == otherAssetGuid;
	}

	template <typename T>
	template <class Archive>
	void AssetReference<T>::Save(Archive& archive) const {
		xg::Guid assetReferenceGuid;

		if (assetReference) assetReferenceGuid = assetReference->GetGuid();

		archive(
			CEREAL_NVP(assetReferenceGuid)
		);
	}

	template <typename T>
	template <class Archive>
	void AssetReference<T>::Load(Archive& archive) {
		xg::Guid assetReferenceGuid {};

		archive(
			CEREAL_NVP(assetReferenceGuid)
		);

		if (!assetReferenceGuid.isValid()) return;

		const std::shared_ptr<AssetManager>& assetManager = AssetManager::Get();
		assetReference = assetManager->LoadAsset<T>(assetReferenceGuid);
	}
}