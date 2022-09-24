#include "Engine/Material/Material.hpp"
#include "Engine/Engine/Engine.hpp"
#include "Engine/Resources/ResourceManager.hpp"

namespace Engine {

	Material::Material(const aiScene* scene, uint32_t materialIndex, std::string modelName)
	{
		if (scene->mNumMaterials <= materialIndex)
			return;

		materialData_ = {};

		this->modelName = modelName;

		materialData_.diffuseTextureLoaded = 0;
		materialData_.specularTextureLoaded = 0;

		aiMaterial* material = scene->mMaterials[materialIndex];

		diffuseTexture = LoadTexture(scene, material, aiTextureType_DIFFUSE);
		defaultDiffuseTexture = diffuseTexture;
		if (diffuseTexture.expired() == false)
			materialData_.diffuseTextureLoaded = 1;
		specularTexture = LoadTexture(scene, material, aiTextureType_SPECULAR);
		defaultSpecularTexture = specularTexture;
		if (specularTexture.expired() == false)
			materialData_.specularTextureLoaded = 1;
		
		bumpMapTexture = LoadTexture(scene, material, aiTextureType_NORMALS);
		defaultBumpMapTexture = bumpMapTexture;
		if (bumpMapTexture.expired() == false)
			materialData_.bumpMapLoaded = 1;

		missingTexture = ResourceManager::Get()->CreateTexture("default.png");
		
		aiMaterial* mat = scene->mMaterials[materialIndex];
		aiColor3D color(0.f, 0.f, 0.f);
		if (mat->Get(AI_MATKEY_COLOR_DIFFUSE, color) != AI_SUCCESS) 
		{
			// Couldn't find color, add failure code.
		}
		float transparency = 1.f;
		if (mat->Get(AI_MATKEY_OPACITY, transparency) != AI_SUCCESS) 
		{
			// Couldn't find transparancy, add failure code.
		}
		materialData_.diffuseColor = glm::vec4(color.r, color.g, color.b, transparency);

		color = aiColor3D(0.f, 0.f, 0.f);
		if (mat->Get(AI_MATKEY_COLOR_SPECULAR, color) != AI_SUCCESS) 
		{
			// Couldn't find color, add failure code.
		}
		materialData_.specularColor = glm::vec4(color.r, color.g, color.b, 0.f);

		float shiny = 0.f;
		if (mat->Get(AI_MATKEY_SHININESS, shiny) != AI_SUCCESS) 
		{

		}
		materialData_.specularExponent = shiny;

		float specScale = 1.f;
		if (mat->Get(AI_MATKEY_SHININESS_STRENGTH, specScale) != AI_SUCCESS) 
		{

		}
		materialData_.specularScale = specScale;

		defaultMaterialData_ = materialData_;

		UpdateMaterialData();
	}


	Material::~Material()
	{
	}

	void Material::SetDiffuseTexture(const std::string& diffuseTextureName)
	{
		diffuseTexture = ResourceManager::Get()->CreateTexture(diffuseTextureName);

		if (materialData_.diffuseTextureLoaded != 1 && diffuseTexture.expired() == false)
		{
			materialData_.diffuseTextureLoaded = 1;
			UpdateMaterialData();
		}
		else if (diffuseTexture.expired() == false) {
			materialData_.diffuseTextureLoaded = 0;
			UpdateMaterialData();
		}
	}

	void Material::SetDiffuseTexture(std::shared_ptr<Texture> newDiffuseTexture)
	{
		diffuseTexture = newDiffuseTexture;

		if (materialData_.diffuseTextureLoaded != 1 && newDiffuseTexture!=nullptr) 
		{
			materialData_.diffuseTextureLoaded = 1;
			UpdateMaterialData();
		}
		else if (newDiffuseTexture == nullptr) {
			materialData_.diffuseTextureLoaded = 0;
			UpdateMaterialData();
		}
	}

	std::weak_ptr<Texture> Material::GetDiffuseTexture()
	{
		if(diffuseTexture.expired() == false)
			return diffuseTexture;

		return missingTexture;
	}

	std::weak_ptr<Texture> Material::GetDefaultDiffuseTexture()
	{
		if(defaultDiffuseTexture.expired() == false)
			return defaultDiffuseTexture;

		return missingTexture;
	}

	bool Material::IsDiffuseLoaded() const
	{
		return materialData_.diffuseTextureLoaded >= 1;
	}

	void Material::SetBumpMapTexture(const std::string& bumpMapTextureName)
	{
		bumpMapTexture = ResourceManager::Get()->CreateTexture(bumpMapTextureName);

		if (materialData_.bumpMapLoaded != 1 && bumpMapTexture.expired() == false)
		{
			materialData_.bumpMapLoaded = 1;
			UpdateMaterialData();
		}
		else if (bumpMapTexture.expired()) {
			materialData_.bumpMapLoaded = 0;
			UpdateMaterialData();
		}
	}

	void Material::SetBumpMapTexture(std::shared_ptr<Texture> bumpMapTexture)
	{
		this->bumpMapTexture = bumpMapTexture;

		if (materialData_.bumpMapLoaded != 1 && bumpMapTexture != nullptr)
		{
			materialData_.bumpMapLoaded = 1;
			UpdateMaterialData();
		}
		else if (bumpMapTexture == nullptr) {
			materialData_.bumpMapLoaded = 0;
			UpdateMaterialData();
		}
	}

	std::weak_ptr<Texture> Material::GetBumpMapTexture() const
	{
		if (bumpMapTexture.expired() == false)
			return bumpMapTexture;

		return missingTexture;
	}

	std::weak_ptr<Texture> Material::GetDefaultBumpMapTexture() const
	{
		if (defaultBumpMapTexture.expired() == false)
			return defaultBumpMapTexture;

		return missingTexture;
	}

	bool Material::IsBumpMapLoaded() const
	{
		return materialData_.bumpMapLoaded >= 1;
	}

	void Material::SetSpecularTexture(std::shared_ptr<Texture> specularTexture)
	{
		this->specularTexture = specularTexture;

		if (materialData_.specularTextureLoaded != 1 && specularTexture != nullptr) 
		{
			materialData_.specularTextureLoaded = 1;
			UpdateMaterialData();
		}
		else if (specularTexture == nullptr) {
			materialData_.specularTextureLoaded = 0;
			UpdateMaterialData();
		}
	}

	std::weak_ptr<Texture> Material::GetSpecularTexture() const
	{
		return specularTexture;
	}

	std::weak_ptr<Texture> Material::GetDefaultSpecularTexture() const
	{
		return defaultSpecularTexture;
	}

	bool Material::IsSpecularLoaded() const
	{
		return static_cast<bool>(materialData_.specularTextureLoaded);
	}

	void Material::SetDiffuseColor(glm::vec3 diffuseColor)
	{
		materialData_.diffuseColor = glm::vec4(diffuseColor.x, diffuseColor.y, diffuseColor.z, 1.0f);
		UpdateMaterialData();
	}

	glm::vec3 Material::GetDiffuseColor() const
	{
		return glm::vec3(materialData_.diffuseColor.x, materialData_.diffuseColor.y, materialData_.diffuseColor.z);
	}

	glm::vec3 Material::GetDefaultDiffuseColor() const
	{
		return glm::vec3(defaultMaterialData_.diffuseColor.x, defaultMaterialData_.diffuseColor.y, defaultMaterialData_.diffuseColor.z);
	}

	void Material::SetSpecularColor(glm::vec3 specularColor)
	{
		materialData_.specularColor = glm::vec4(specularColor.x, specularColor.y, specularColor.z, 1.0f);
		UpdateMaterialData();
	}

	glm::vec3 Material::GetSpecularColor() const
	{
		return glm::vec3(materialData_.specularColor.x, materialData_.specularColor.y, materialData_.specularColor.z);
	}

	glm::vec3 Material::GetDefaultSpecularColor() const
	{
		return glm::vec3(defaultMaterialData_.specularColor.x, defaultMaterialData_.specularColor.y, defaultMaterialData_.specularColor.z);
	}

	void Material::SetSpecularScale(float scale)
	{
		materialData_.specularScale = scale;
		UpdateMaterialData();
	}

	float Material::GetSpecularScale() const
	{
		return materialData_.specularScale;
	}

	float Material::GetDefaultSpecularScale() const
	{
		return defaultMaterialData_.specularScale;
	}

	void Material::SetSpecularExponent(float exponent)
	{
		materialData_.specularExponent = exponent;
		UpdateMaterialData();
	}

	float Material::GetSpecularExponent() const
	{
		return materialData_.specularExponent;
	}

	float Material::GetDefaultSpecularExponent() const
	{
		return defaultMaterialData_.specularExponent;
	}

	bool Material::operator==(const Material & other) const
	{
		if (materialData_.specularExponent != other.materialData_.specularExponent) return false;
		if (materialData_.specularScale != other.materialData_.specularScale) return false;

		if (materialData_.diffuseColor != other.materialData_.diffuseColor) return false;
		if (materialData_.specularColor != other.materialData_.specularColor) return false;

		if (this->diffuseTexture.lock().get() != other.diffuseTexture.lock().get()) return false;
		if (this->specularTexture.lock().get() != other.specularTexture.lock().get()) return false;

		return true;
	}

	void Material::UpdateMaterialData()
	{
	}

	std::weak_ptr<Texture> Material::LoadTexture(const aiScene* scene, aiMaterial * material, aiTextureType textureType)
	{
		if (material->GetTextureCount(textureType) > 0) { // Textures located
			aiString path;
			material->GetTexture(textureType, 0, &path);

			if (path.data[0] == '*') { // Texture is an embedded texture

				std::string name = modelName + std::string(path.C_Str());

				std::weak_ptr<Texture> ret = ResourceManager::Get()->GetTexture(name);

				if (ret.expired() == false)
					return ret;

				uint32_t index = atoi(path.C_Str() + 1);
				if (index < scene->mNumTextures) 
				{
					aiTexture* texture = scene->mTextures[index];

					stbi_uc* textureData;

					int width, height, channels;

					if (texture->mHeight == 0) 
					{ // Compressed texture
						char* data = (char*)malloc(sizeof(char)*texture->mWidth);
						memcpy(data, texture->pcData, texture->mWidth);
						textureData = stbi_load_from_memory((stbi_uc*)data, texture->mWidth, &width, &height, &channels, 4);
					}
					else 
					{
						textureData = (stbi_uc*)malloc(sizeof(stbi_uc)*texture->mWidth*texture->mHeight * 4);
						for (size_t i = 0; i < texture->mWidth*texture->mHeight*4; i += 4) 
						{
							textureData[i] = texture->pcData->r;
							textureData[i + 1] = texture->pcData->g;
							textureData[i + 2] = texture->pcData->b;
							textureData[i + 3] = texture->pcData->a;
						}

						width = texture->mWidth;
						height = texture->mHeight;
						channels = 4;					
					}

					return ResourceManager::Get()->CreateTexture(name, textureData, width, height);

				}
				else 
				{
					return std::shared_ptr<Texture>();
				}
			}
			else 
			{
				return ResourceManager::Get()->CreateTexture(std::string(path.C_Str()));
			}
		}
		else 
		{
			return std::shared_ptr<Texture>();
		}
	}

} // namespace Engine