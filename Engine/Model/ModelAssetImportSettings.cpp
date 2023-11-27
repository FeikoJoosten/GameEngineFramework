#include "Engine/Model/ModelAssetImportSettings.hpp"
#include "Engine/AssetManagement/AssetManager.hpp"
#include "Engine/Mesh/MeshAssetImporter.hpp"
#include "Engine/Texture/TextureAssetImporter.hpp"

CEREAL_REGISTER_TYPE(Engine::ModelAssetImportSettings);
CEREAL_SPECIALIZE_FOR_ALL_ARCHIVES(Engine::ModelAssetImportSettings, specialization::member_load_save)

Engine::ModelAssetImportSettings::ModelAssetImportSettings() {
	if(!textureAssetImportSettings)	textureAssetImportSettings = AssetManager::Get()->GetAssetImporter<TextureAssetImporter>()->GetDefaultImportSettings();
	if (!meshAssetImportSettings) meshAssetImportSettings = AssetManager::Get()->GetAssetImporter<MeshAssetImporter>()->GetDefaultImportSettings();
}
