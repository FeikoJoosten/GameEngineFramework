#include "Engine/AssetManagement/ModelImporter.hpp"
#include "AssetManagement/AssetManager.hpp"
#include "Engine/Utility/Logging.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>

namespace Engine {

	ModelImporter::ModelImporter(const std::shared_ptr<AssetRegistry> assetRegistry, unsigned importFlags) : AssetImporter<Model>(assetRegistry), importFlags(importFlags) {}

	bool ModelImporter::SupportsFileExtension(const std::string& fileExtension) {
		return importer.IsExtensionSupported(fileExtension);
	}

	std::shared_ptr<Asset> ModelImporter::ProcessAsset(const std::string& pathInProject, const std::string& assetName) {
		const aiScene* scene = importer.ReadFile(AssetManager::GetProjectRoot() + pathInProject + "/" + assetName, importFlags);

		// check if there's a scene or flags and check if the flags show incomplete scene
		// or a missing root node (any successful import returns rood node)
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			DEBUG_WARNING(importer.GetErrorString())
			importer.FreeScene();
			return {};
		}

		return {};
	}

	std::shared_ptr<Model> ModelImporter::LoadAsset(xg::Guid assetId) {
		std::string pathInProject;
		std::string assetName;
		if (!assetRegistry->TryGetPathForGuid(assetId, pathInProject, assetName)) {
			DEBUG_ERROR("Failed to load scene, because no scene is known with GUID: " + assetId.str());
			return {};
		}

		return LoadAsset(pathInProject, assetName);
	}

	std::shared_ptr<Model> ModelImporter::LoadAsset(const std::string& pathInProject, const std::string& assetName) {
		return {};
		//return AssetManager::Get()->ReadDataFromPath<std::shared_ptr<Model>>(pathInProject, assetName);
	}

	void ModelImporter::SetImportFlags(unsigned newImportFlags) {
		importFlags = newImportFlags;
	}
}
