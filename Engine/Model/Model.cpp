#include "Engine/Model/Model.h"
#include "Engine/Mesh/Mesh.h" 
#include "Engine/Utility/Defines.hpp"
#include "Engine/Resources/ModelLoader.hpp"
#include <iostream>

#if RENDERER == 1
#include "Engine/Mesh/OpenGLMesh.hpp"
#include "Engine/Texture/OpenGLTexture.hpp"
#elif RENDERER == 2
#include "Engine/Mesh/VulkanMesh.hpp"
#include "Engine/Texture/VulkanTexture.hpp"
#else
#include "Engine/Mesh/Mesh.hpp"
#include "Engine/Texture/Texture.hpp"
#endif

namespace Engine
{
	void Model::LoadModel(eastl::string path)
	{
		////check if the mesh is not already loaded inside the meshes map
		//auto const result = existingMeshes.find(path);
		//
		//if (result == existingMeshes.end())
		//{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);

		//check if there's a scene or flags and check if the flags show incomplete scene
		//or a missing root node (any successful import returns rood node)
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			std::cout << "ERROR ASSIMP: " << importer.GetErrorString() << std::endl;
			return;
		}

		directory = path.substr(0, path.find_last_of('/'));

		//process the nodes and extract their data
		ProcessNode(scene->mRootNode, scene);

		//	existingMeshes.insert(eastl::pair<eastl::string, eastl::vector<Mesh>>(path, meshes));
		//
		//
		//	return existingMeshes.find(path)->second;
		//}else
		//{
		//	return (result->second);
		//}
	}

	void Model::ProcessNode(aiNode * node, const aiScene * scene)
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

	eastl::vector<Texture*> Model::ProcessTextures(aiMaterial* material) { return eastl::vector<Texture*>(); }

	Mesh* Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
	{
		eastl::vector<Vertex> vertices;
		eastl::vector<unsigned> indices;
		eastl::vector<Texture*> textures;

		//unpack vertices
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;
			//position vertex coords
			vertex.position.x = mesh->mVertices[i].x;
			vertex.position.y = mesh->mVertices[i].y;
			vertex.position.z = mesh->mVertices[i].z;

			//check if we have normals
			if (mesh->mNormals)
			{
				//normal vertex coordinates
				vertex.normal.x = mesh->mNormals[i].x;
				vertex.normal.y = mesh->mNormals[i].y;
				vertex.normal.z = mesh->mNormals[i].z;
			}

			//Assimp allows a mesh to have 8 sets of texture coordinates
			//I'll load only the first set for now, if needed we can extend
			if (mesh->mTextureCoords[0] > 0)
			{
				vertex.texCoords.s = mesh->mTextureCoords[0][i].x;
				vertex.texCoords.t = mesh->mTextureCoords[0][i].y;
			}
			else
			{
				//no texture coordinates
				vertex.texCoords = glm::vec2(0.0f, 0.0f);
			}

			vertices.push_back(vertex);
		}


		for (size_t i = 0; i < mesh->mNumFaces; ++i)
		{
			aiFace *face = &mesh->mFaces[i];

			// retrieve all indices of the face and store them in the indices vector
			for (size_t j = 0; j < face->mNumIndices; j++)
			{
				indices.push_back(static_cast<unsigned>(face->mIndices[j]));
			}
		}

		//check for materials
		if (mesh->mMaterialIndex >= 0)
		{
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
			textures = ProcessTextures(material);
		}

#if RENDERER == 1
		return new OpenGLMesh(vertices, indices, textures);
#elif RENDERER == 2
		return new VulkanMesh(vertices, indices, textures);
#else 
		return new Mesh(vertices, indices, textures);
#endif
	}

	eastl::vector<Texture*> Model::LoadMaterialTextures(aiMaterial* material, aiTextureType type, eastl::string type_name)
	{
		eastl::vector<Texture*> textures;
		for (unsigned int i = 0; i < material->GetTextureCount(type); i++)
		{
			aiString str;
			material->GetTexture(type, i, &str);
			eastl::string filename = eastl::string(str.C_Str());

			// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
			bool skip = false;
			for (unsigned int j = 0; j < loadedTextures.size(); j++)
			{
				if (loadedTextures[j]->GetFileName() == filename)
				{
					textures.push_back(loadedTextures[j]);
					skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
					break;
				}
			}
			if (!skip)
			{   // if texture hasn't been loaded already, load it
				//Texture texture;
				//texture.id = loadTextureFromFile(str.C_Str(), this->directory);
				//texture.type = typeName;
				//texture.path = str;

				Texture* texture = ModelLoader::CreateTexture(filename);
				textures.push_back(texture);

				loadedTextures.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
			}
		}
		return textures;
	}
} // namespace Engine