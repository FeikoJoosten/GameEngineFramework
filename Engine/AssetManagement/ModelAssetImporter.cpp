#include "Engine/AssetManagement/ModelAssetImporter.hpp"
#include "Engine/AssetManagement/MaterialAssetImporter.hpp"
#include "Engine/AssetManagement/MeshAssetImporter.hpp"
#include "Engine/AssetManagement/ModelAssetImportSettings.hpp"
#include "Engine/AssetManagement/AssetManager.hpp"
#include "Engine/AssetManagement/TextureAssetImporter.hpp"
#include "Engine/Components/MeshRendererComponent.hpp"
#include "Engine/Components/TransformComponent.hpp"
#include "Engine/Entity/Entity.hpp"
#include "Engine/Material/Material.hpp"
#include "Engine/Mesh/Mesh.hpp"
#include "Engine/Utility/Logging.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <ranges>
#include <stb/stb_image.h> // TODO: Make a wrapper about texture import settings flags so this include isn't needed

namespace Engine {

	ModelAssetImporter::ModelAssetImporter(const std::shared_ptr<AssetRegistry>& assetRegistry) : AssetImporter<Model>(assetRegistry) {}

	const char* ModelAssetImporter::GetDefaultAssetExtension() const {
		return ".model";
	}

	bool ModelAssetImporter::SupportsFile(const std::string& fullPath, const std::string& fileExtension) {
		return importer.IsExtensionSupported(fileExtension);
	}

	void RegisterImportAndAddAssetToParent(
		const std::shared_ptr<IAssetImporter>& assetImporter,
		const std::shared_ptr<AssetRegistry>& assetRegistry,
		const xg::Guid& parentGuid,
		const std::shared_ptr<Asset>& childAsset,
		const std::string& pathInProject,
		const std::string& assetNameWithExtension,
		const std::map<std::pair<std::string, std::string>, xg::Guid>& knownChildGuidsByPathAndName) {

		xg::Guid knownChildGuid {};
		for (const auto& [assetNameByPath, childGuid] : knownChildGuidsByPathAndName) {
			if (assetNameByPath.first != pathInProject) continue;
			if (assetNameByPath.second != assetNameWithExtension) continue;

			knownChildGuid = childGuid;
			break;
		}
		
		if (!knownChildGuid.isValid() || !AssetRegistry::TryAssignGuidToAsset(childAsset, knownChildGuid))
			if (!assetRegistry->TryRegisterAsset(childAsset, pathInProject, assetNameWithExtension.c_str())) return;

		assetImporter->ImportAsset(childAsset);

		AssetRegistry::TryAddChildAssetToAsset(parentGuid, childAsset->GetGuid());
	}

	std::shared_ptr<Asset> ModelAssetImporter::ImportAsset(const std::shared_ptr<Asset>& assetToImport, const std::shared_ptr<AssetImportSettings>& importSettings) {
		const std::shared_ptr<Model> modelToImport = std::dynamic_pointer_cast<Model>(assetToImport);
		if (!modelToImport) {
			DEBUG_ERROR("The asset trying to import is not of type Model!");
			return {};
		}

		// Should this also go over the materials, textures, child objects, etc?

		if (!AssetRegistry::TryWriteCustomDataToAsset(modelToImport->GetGuid(), modelToImport, GetDefaultAssetExtension())) {
			DEBUG_ERROR("Failed to save imported model to disk!");
			return {};
		}

		return modelToImport;
	}

	std::shared_ptr<Asset> ModelAssetImporter::ImportAsset(const std::string& pathInProject, const char* assetNameWithExtension, const std::shared_ptr<AssetImportSettings>& importSettings) {
		// TODO: Verify if asset was already imported before by manually loading the import settings, if so verify import flags
		const std::shared_ptr<ModelAssetImportSettings> modelAssetImportSettings = std::static_pointer_cast<ModelAssetImportSettings>(importSettings);

		const aiScene* scene = importer.ReadFile(AssetManager::PathInProjectToFullPath(pathInProject) + "/" + assetNameWithExtension, modelAssetImportSettings->importFlags);

		// check if there's a scene or flags and check if the flags show incomplete scene
		// or a missing root node (any successful import returns root node)
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			DEBUG_WARNING(importer.GetErrorString());
			importer.FreeScene();
			return {};
		}

		xg::Guid relatedAssetGuid = importSettings->relatedAssetGuid;
		if (relatedAssetGuid.isValid() && !AssetRegistry::IsAssetRegistered(relatedAssetGuid))
			relatedAssetGuid = {}; // Reset the related asset guid since the related asset has been removed from the asset registry

		std::map<std::pair<std::string, std::string>, xg::Guid> childGuidsByPathAndName {};
		if (std::vector<xg::Guid> childGuids {}; relatedAssetGuid.isValid() && AssetRegistry::TryGetChildrenForAsset(relatedAssetGuid, childGuids)) {
			for (const xg::Guid& childGuid : childGuids) {
				std::string childPathInProject;
				std::string childAssetNameWithExtension;
				if (!AssetRegistry::TryGetPathForGuid(childGuid, childPathInProject, childAssetNameWithExtension)) continue;

				childGuidsByPathAndName[{ childPathInProject, childAssetNameWithExtension }] = childGuid;
			}
		}

		std::vector<std::shared_ptr<Texture>> textures {};

		for (unsigned int i = 0; i < scene->mNumTextures; i++) {
			const aiTexture* texture = scene->mTextures[i];

			const unsigned int bufferSize = texture->mHeight == 0 ? texture->mWidth : texture->mWidth * texture->mHeight;
			int width;
			int height;
			int channels;
			std::shared_ptr<Texture> modelTexture = TextureAssetImporter::ProcessRawData(reinterpret_cast<unsigned char*>(texture->pcData), static_cast<int>(bufferSize), width, height, channels, STBI_rgb_alpha);
			if (!modelTexture) continue; // No need to log if this fails, the TextureAssetImporter already does this

			textures.push_back(modelTexture);
		}

		std::vector<std::shared_ptr<Material>> materials {};

		for (unsigned int i = 0; i < scene->mNumMaterials; i++) {
			//aiMaterial* material = scene->mMaterials[i];
			// TODO: Process materials
		}

		// TODO: Load Animations

		std::map<unsigned int, std::shared_ptr<Mesh>> meshes {};
		for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
			const std::shared_ptr<Mesh> mesh = MeshAssetImporter::ProcessRawData(scene->mMeshes[i]);
			if (!mesh) continue; // No need to log if this fails, the MeshAssetImporter already does this

			meshes[i] = mesh;
		}

		std::shared_ptr<Model> processedModel;
		if (scene->mNumMeshes || !materials.empty() || !textures.empty()) {
			processedModel = std::shared_ptr<Model>(new Model());
			processedModel->SetName(assetNameWithExtension);
			ProcessNode(scene->mRootNode, meshes, processedModel->entities, nullptr, modelAssetImportSettings);

			if (!relatedAssetGuid.isValid() || !AssetRegistry::TryAssignGuidToAsset(processedModel, relatedAssetGuid))
				assetRegistry->TryRegisterAsset(processedModel, pathInProject, assetNameWithExtension);

			const xg::Guid processedModelGuid = processedModel->GetGuid();
			const std::string& processedModelPath = processedModel->GetName() + "/";

			// TODO: The registering of these child assets should really ensure the names are unique
			const std::shared_ptr<AssetManager> assetManager = AssetManager::Get();

			std::shared_ptr<TextureAssetImporter> textureAssetImporter = assetManager->GetAssetImporter<TextureAssetImporter>();
			const char* textureExtension = textureAssetImporter->GetDefaultAssetExtension();
			for (const std::shared_ptr<Texture>& texture : textures)
				RegisterImportAndAddAssetToParent(textureAssetImporter, assetRegistry, processedModelGuid, texture, pathInProject, processedModelPath + texture->GetName() + textureExtension, childGuidsByPathAndName);

			std::shared_ptr<MaterialAssetImporter> materialAssetImporter = assetManager->GetAssetImporter<MaterialAssetImporter>();
			const char* materialExtension = materialAssetImporter->GetDefaultAssetExtension();
			for (const std::shared_ptr<Material>& material : materials)
				RegisterImportAndAddAssetToParent(materialAssetImporter, assetRegistry, processedModelGuid, material, pathInProject, processedModelPath + material->GetName() + materialExtension, childGuidsByPathAndName);

			std::shared_ptr<MeshAssetImporter> meshAssetImporter = assetManager->GetAssetImporter<MeshAssetImporter>();
			const char* meshExtension = meshAssetImporter->GetDefaultAssetExtension();
			for (const std::shared_ptr<Mesh>& mesh : meshes | std::views::values)
				RegisterImportAndAddAssetToParent(meshAssetImporter, assetRegistry, processedModelGuid, mesh, pathInProject, processedModelPath + mesh->GetName() + meshExtension, childGuidsByPathAndName);

			if (!AssetRegistry::TryWriteCustomDataToAsset(processedModel->GetGuid(), processedModel, GetDefaultAssetExtension())) {
				DEBUG_ERROR("Failed to save imported model to disk!");
				return {};
			}
		}

		importer.FreeScene();
		return processedModel;
	}

	std::shared_ptr<Asset> ModelAssetImporter::LoadAsset(const xg::Guid& assetGuid) const {
		std::shared_ptr<Asset> model;
		if (!AssetRegistry::TryLoadCustomDataFromAsset(assetGuid, GetDefaultAssetExtension(), model)) return {};

		return model;
	}

	std::shared_ptr<AssetImportSettings> ModelAssetImporter::GetDefaultImportSettings() {
		return std::make_shared<ModelAssetImportSettings>();
	}

	void ModelAssetImporter::ProcessNode(const aiNode* node, const std::map<unsigned int, std::shared_ptr<Mesh>>& meshes, std::vector<std::shared_ptr<Entity>>& entities, const std::shared_ptr<TransformComponent>& parent, const std::shared_ptr<ModelAssetImportSettings>& importSettings) {
		const std::shared_ptr<Entity> entity = std::make_shared<Entity>(node->mName.C_Str());
		entities.emplace_back(entity);
		const std::shared_ptr<TransformComponent> transformComponent = entity->AddComponent<TransformComponent>();
		const aiMatrix4x4& modelMatrix = node->mTransformation;

		if (parent) transformComponent->SetParent(parent);

		// TODO: This assumes the incoming matrix is column major, however some formats use row major instead... Need to figure out which model types that are and convert accordingly
		transformComponent->SetLocalMatrix({
			modelMatrix.a1, modelMatrix.a2, modelMatrix.a3, modelMatrix.a4 * importSettings->scaleFactor,
			modelMatrix.b1, modelMatrix.b2, modelMatrix.b3, modelMatrix.b4 * importSettings->scaleFactor,
			modelMatrix.c1, modelMatrix.c2, modelMatrix.c3, modelMatrix.c4 * importSettings->scaleFactor,
			modelMatrix.d1, modelMatrix.d2, modelMatrix.d3, modelMatrix.d4
			});

		for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
			std::map<unsigned, std::shared_ptr<Mesh>>::const_iterator mesh = meshes.find(node->mMeshes[i]);
			if (mesh == meshes.end()) continue;

			const std::shared_ptr<MeshRendererComponent> meshRenderer = entity->AddComponent<MeshRendererComponent>();
			meshRenderer->SetMesh(mesh->second);
		}

		for (unsigned int i = 0; i < node->mNumChildren; ++i)
			ProcessNode(node->mChildren[i], meshes, entities, transformComponent, importSettings);
	}
}
