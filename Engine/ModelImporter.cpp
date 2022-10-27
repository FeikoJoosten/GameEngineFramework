#include "Engine/AssetManagement/ModelImporter.hpp"
#include "Engine/Utility/Logging.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>

namespace Engine {

	ModelImporter::ModelImporter(unsigned importFlags) : importFlags(importFlags) {}

	bool ModelImporter::SupportsFileExtension(const char* fileExtension) {
		return importer.IsExtensionSupported(fileExtension);
	}

	void ModelImporter::ProcessAsset(const char* fullSystemPath) {
		const aiScene* scene = importer.ReadFile(fullSystemPath, importFlags);

		// check if there's a scene or flags and check if the flags show incomplete scene
		// or a missing root node (any successful import returns rood node)
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			DEBUG_WARNING(importer.GetErrorString())
			importer.FreeScene();
			return;
		}


	}

	std::shared_ptr<Model> ModelImporter::LoadAsset(xg::Guid assetId) {
		return {};
	}

	void ModelImporter::SetImportFlags(unsigned newImportFlags) {
		importFlags = newImportFlags;
	}
}
