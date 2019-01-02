#include "Engine/Model/OpenGLModel.hpp"

namespace Engine
{
	void OpenGLModel::ProcessNode(aiNode* node, const aiScene* scene)
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
	}

	eastl::vector<Texture*> OpenGLModel::ProcessTextures(aiMaterial* material)
	{
		eastl::vector<Texture*> textures;

		eastl::vector<Texture*> diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.begin());

		eastl::vector<Texture*> specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

		return textures;
	}
} // namespace Engine