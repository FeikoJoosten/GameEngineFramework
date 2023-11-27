#include "Engine/Model/ModelAssetImporter.hpp"
#include "Engine/AssetManagement/AssetManager.hpp"
#include "Engine/Components/MeshRendererComponent.hpp"
#include "Engine/Components/TransformComponent.hpp"
#include "Engine/Entity/Entity.hpp"
//#include "Engine/Material/Material.hpp"
#include "Engine/Material/MaterialAssetImporter.hpp"
#include "Engine/Mesh/Mesh.hpp"
#include "Engine/Mesh/MeshAssetImporter.hpp"
#include "Engine/Model/ModelAssetImportSettings.hpp"
#include "Engine/Texture/TextureAssetImporter.hpp"
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

	std::shared_ptr<Asset> ModelAssetImporter::CreateAsset() const {
		return std::shared_ptr<Model>(new Model());
	}

	std::shared_ptr<AssetImportSettings> ModelAssetImporter::GetDefaultImportSettings() const {
		return std::make_shared<ModelAssetImportSettings>();
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

	std::shared_ptr<Asset> ModelAssetImporter::ImportAsset(const std::string& fullPath, const char* assetNameWithExtension, const std::shared_ptr<AssetImportSettings>& importSettings) {
		const std::shared_ptr<ModelAssetImportSettings> modelAssetImportSettings = std::static_pointer_cast<ModelAssetImportSettings>(importSettings);

		const aiScene* scene = importer.ReadFile(fullPath + "/" + assetNameWithExtension, modelAssetImportSettings->importFlags);

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

		// TODO: The registering of these child assets should really ensure the names are unique
		const std::shared_ptr<AssetManager> assetManager = AssetManager::Get();
		const std::string& processedModelPath = fullPath + std::string(assetNameWithExtension) + "/";

		std::vector<std::shared_ptr<Texture>> textures {};
		const std::shared_ptr<TextureAssetImporter> textureAssetImporter = assetManager->GetAssetImporter<TextureAssetImporter>();
		const std::shared_ptr<AssetImportSettings>& textureAssetImportSettings = modelAssetImportSettings->textureAssetImportSettings;
		for (unsigned int i = 0; i < scene->mNumTextures; i++) {
			const aiTexture* texture = scene->mTextures[i];

			std::shared_ptr<Texture> createdTexture = std::static_pointer_cast<Texture>(textureAssetImporter->CreateAsset());
			createdTexture->SetName(texture->mFilename.C_Str());

			const unsigned int bufferSize = texture->mHeight == 0 ? texture->mWidth : texture->mWidth * texture->mHeight;
			int width;
			int height;
			int channels;
			TextureAssetImporter::ProcessRawData(createdTexture, reinterpret_cast<unsigned char*>(texture->pcData), static_cast<int>(bufferSize), width, height, channels, STBI_rgb_alpha);
			if (!createdTexture) continue; // No need to log if this fails, the TextureAssetImporter already does this

			assetRegistry->TryRegisterAsset(createdTexture, textureAssetImportSettings->GetGuid(), processedModelPath, (createdTexture->GetName() + textureAssetImporter->GetDefaultAssetExtension()).c_str());
			createdTexture = std::static_pointer_cast<Texture>(textureAssetImporter->ImportAsset(createdTexture, textureAssetImportSettings));

			textures.push_back(createdTexture);
		}

		// TODO: Process materials
		//std::vector<std::shared_ptr<Material>> materials {};
		//for (unsigned int i = 0; i < scene->mNumMaterials; i++) {
		//	aiMaterial* material = scene->mMaterials[i];
		//}

		// TODO: Load Animations

		std::map<unsigned int, std::shared_ptr<Mesh>> meshes {};
		const std::shared_ptr<MeshAssetImporter> meshAssetImporter = assetManager->GetAssetImporter<MeshAssetImporter>();
		const std::shared_ptr<AssetImportSettings>& meshAssetImportSettings = modelAssetImportSettings->meshAssetImportSettings;
		for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
			const aiMesh* mesh = scene->mMeshes[i];
			std::shared_ptr<Mesh> createdMesh = std::static_pointer_cast<Mesh>(meshAssetImporter->CreateAsset());
			createdMesh->SetName(mesh->mName.C_Str());

			MeshAssetImporter::ProcessRawData(createdMesh, scene->mMeshes[i]);
			if (!createdMesh) continue; // No need to log if this fails, the MeshAssetImporter already does this

			assetRegistry->TryRegisterAsset(createdMesh, meshAssetImportSettings->GetGuid(), processedModelPath, (createdMesh->GetName() + meshAssetImporter->GetDefaultAssetExtension()).c_str());
			createdMesh = std::static_pointer_cast<Mesh>(meshAssetImporter->ImportAsset(createdMesh, meshAssetImportSettings));

			meshes[i] = createdMesh;
		}

		std::shared_ptr<Model> processedModel;
		if (scene->mNumMeshes /*|| !materials.empty()*/ || !textures.empty()) {
			processedModel = std::shared_ptr<Model>(new Model());
			processedModel->SetName(assetNameWithExtension);
			ProcessNode(scene->mRootNode, meshes, processedModel->entities, nullptr, modelAssetImportSettings);

			if (!relatedAssetGuid.isValid() || !AssetRegistry::TryAssignGuidToAsset(processedModel, relatedAssetGuid))
				assetRegistry->TryRegisterAsset(processedModel, modelAssetImportSettings->GetGuid(), fullPath, assetNameWithExtension);

			const xg::Guid processedModelGuid = processedModel->GetGuid();

			for (const std::shared_ptr<Texture>& texture : textures)
				AssetRegistry::TryAddChildAssetToAsset(processedModelGuid, texture->GetGuid());
			
			//for (const std::shared_ptr<Material>& material : materials)
			//	AssetRegistry::TryAddChildAssetToAsset(processedModelGuid, material->GetGuid());
			
			for (const std::shared_ptr<Mesh>& mesh : meshes | std::views::values)
				AssetRegistry::TryAddChildAssetToAsset(processedModelGuid, mesh->GetGuid());

			if (!AssetRegistry::TryWriteCustomDataToAsset(processedModel->GetGuid(), processedModel, GetDefaultAssetExtension())) {
				DEBUG_ERROR("Failed to save imported model to disk!");
				return {};
			}
		}

		importer.FreeScene();
		return processedModel;
	}

	std::shared_ptr<Asset> ModelAssetImporter::LoadAsset(const xg::Guid& assetGuid, const std::shared_ptr<AssetImportSettings>& importSettings) const {
		std::shared_ptr<Asset> model;
		if (!AssetRegistry::TryLoadCustomDataFromAsset(assetGuid, GetDefaultAssetExtension(), model)) return {};

		return model;
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
