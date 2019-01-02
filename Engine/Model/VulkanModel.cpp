#include "Engine/Model/VulkanModel.hpp"
#include "Engine/Mesh/VulkanMesh.hpp"
#include "Engine/Texture/VulkanTexture.hpp"

namespace Engine
{
	void VulkanModel::ProcessNode(aiNode* node, const aiScene* scene)
	{
		//process all node's meshes if any
		// if there aren't any try going through the children nodes
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(ProcessMesh(mesh, scene));
		}

		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			ProcessNode(node->mChildren[i], scene);
		}

		for (unsigned int i = 0; i < meshes.size(); i++)
		{
			VkBuffer test = meshes[i]->GetUBO<VkBuffer>();
			if (meshes[i]->GetTextures().size() > 0) {
				dynamic_cast<VulkanTexture*>(meshes[i]->GetTextures()[0])->CreateDescriptorSet(&test);

			}
			else {
				meshes[i]->AddTexture("default.png");
			}
		}
	}

	eastl::vector<Texture*> VulkanModel::ProcessTextures(aiMaterial* material)
	{
		eastl::vector<Texture*> textures;

		eastl::vector<Texture*> diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.resize(diffuseMaps.size());
		for (size_t i = 0, size = diffuseMaps.size(); i < size; i++)
		{
			textures[i] = diffuseMaps[i];
		}

		eastl::vector<Texture*> specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		textures.resize(diffuseMaps.size() + specularMaps.size());
		for (size_t i = 0, size = specularMaps.size(), diffuseSize = diffuseMaps.size(); i < size; i++)
		{
			textures[diffuseSize + i] = specularMaps[i];
		}

		return textures;
	}
} //namespace Engine