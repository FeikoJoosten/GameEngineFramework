#include "Engine/Resources/ResourceManager.hpp"
#include "Engine/Utility/Defines.hpp"
#include "Engine/Utility/Logging.hpp"

#ifdef USING_OPENGL
#include "Engine/Texture/OpenGLTexture.hpp"
#include "Engine/Mesh/OpenGLMesh.hpp"
#endif
#ifdef USING_VULKAN
#include "Engine/Texture/VulkanTexture.hpp"
#include "Engine/Mesh/VulkanMesh.hpp"
#include "Engine/Material/VulkanMaterial.hpp"
#endif

#include <ThirdParty/EASTL-master/include/EASTL/algorithm.h>
#include <iostream>
#include <ThirdParty/assimp/include/assimp/postprocess.h>

namespace Engine
{
	eastl::weak_ptr<Model> ResourceManager::GetModel(eastl::string modelName)
	{
		// if already loaded
		for (size_t i = 0, size = loadedModels_.size(); i < size; ++i)
		{
			if (loadedModels_[i]->GetName() == modelName)
				return loadedModels_[i];
		}

		// If not (yet) loaded
		return eastl::shared_ptr<Model>();
	}

	eastl::weak_ptr<Model> ResourceManager::CreateModel(eastl::string modelName, eastl::string meshToLoad, eastl::string skeletonToLoad)
	{
		Assimp::Importer importer;
		eastl::string path = "Resources/Models/" + meshToLoad;
		eastl::string skeletonPath;

		const aiScene* scene = importer.ReadFile(path.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices | aiProcess_ImproveCacheLocality |
			aiProcess_OptimizeMeshes);

		// check if there's a scene or flags and check if the flags show incomplete scene
		// or a missing root node (any successful import returns rood node)
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			debug_warning("ResourceManager", "CreateModel", importer.GetErrorString());
			return eastl::shared_ptr<Model>();
		}

		eastl::weak_ptr<Skeleton> skeleton;
		if (skeletonToLoad != "")
			skeleton = CreateSkeleton(skeletonToLoad);
		else
			skeleton = CreateSkeleton(meshToLoad);

		loadedModels_.push_back(eastl::shared_ptr<Model>(new Model(scene, modelName)));
		eastl::shared_ptr<Model>modelToAddTo = loadedModels_.back();

		// process the nodes and extract their data
		ProcessModel(modelName, modelToAddTo, scene->mRootNode, scene, skeleton.lock());
		return loadedModels_.back();
	}

	eastl::weak_ptr<Skeleton> ResourceManager::CreateSkeleton(eastl::string skeletonToLoad)
	{
		Assimp::Importer importer;
		eastl::string path = "Resources/Models/" + skeletonToLoad;

		for (size_t i = 0, size = loadedSkeletons_.size(); i < size; ++i) {
			if (loadedSkeletons_[i]->GetName() == skeletonToLoad)
				return loadedSkeletons_[i];
		}

		const aiScene* scene = importer.ReadFile(path.c_str(), 0);

		// check if there's a scene or flags and check if the flags show incomplete scene
		// or a missing root node (any successful import returns rood node)
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			debug_warning("ResourceManager", "CreateSkeleton", importer.GetErrorString());
			return eastl::shared_ptr<Skeleton>();
		}

		eastl::shared_ptr<Skeleton> skeleton = eastl::shared_ptr<Skeleton>(new Skeleton(scene));
		skeleton->SetName(skeletonToLoad);

		loadedSkeletons_.push_back(skeleton);
		return skeleton;
	}

	void ResourceManager::AddAnimationsToSkeleton(eastl::string skeletonName, eastl::string animationsToLoad, eastl::vector<eastl::string> names)
	{
		Assimp::Importer importer;
		eastl::string path = "Resources/Animations/" + animationsToLoad;
		const aiScene* scene = importer.ReadFile(path.c_str(), 0);

		for (size_t i = 0, size = loadedSkeletons_.size(); i < size; ++i)
		{
			if (loadedSkeletons_[i]->GetName() == skeletonName) {
				loadedSkeletons_[i]->LoadAnimationSet(scene, names);
			}
		}
	}

	eastl::weak_ptr<Mesh> ResourceManager::CreateMesh(aiMesh* mesh, eastl::shared_ptr<Skeleton> skeleton, eastl::vector<Vertex> vertices, eastl::vector<unsigned> indices)
	{
		// If already loaded
		eastl::weak_ptr<Mesh> meshToReturn = GetMesh(vertices, indices);
		if (meshToReturn.expired() == false && meshToReturn.lock().get() != nullptr)
			return meshToReturn;

#ifdef USING_OPENGL
		eastl::shared_ptr<Mesh> createdMesh = eastl::shared_ptr<OpenGLMesh>(new OpenGLMesh(vertices, indices));
#endif
#ifdef USING_VULKAN
		eastl::shared_ptr<Mesh> createdMesh = eastl::shared_ptr<VulkanMesh>(new VulkanMesh(mesh, skeleton, vertices, indices));
#endif
		loadedMeshes_.push_back(eastl::move(createdMesh));

		return meshToReturn;
	}

	eastl::weak_ptr<Texture> ResourceManager::GetTexture(eastl::string meshName)
	{
		// if not (yet) loaded
		if (loadedTextures_.find(meshName) == loadedTextures_.end())
			return eastl::shared_ptr<Texture>();

		// if already loaded
		return loadedTextures_[meshName];
	}

	eastl::weak_ptr<Texture> ResourceManager::CreateTexture(eastl::string textureName)
	{
		// If already loaded
		eastl::weak_ptr<Texture> textureToReturn = GetTexture(textureName);
		if (textureToReturn.expired() == false && textureToReturn.lock().get() != nullptr)
			return textureToReturn;

#ifdef USING_OPENGL
		eastl::shared_ptr<Texture> createdTexture = eastl::shared_ptr<OpenGLTexture>(new OpenGLTexture(textureName));
#endif
#ifdef USING_VULKAN
		eastl::shared_ptr<Texture> createdTexture = eastl::shared_ptr<VulkanTexture>(new VulkanTexture(textureName));
#endif
		loadedTextures_.insert(eastl::make_pair(textureName, eastl::move(createdTexture)));

		return loadedTextures_[textureName];
	}

	void ResourceManager::ProcessModel(eastl::string modelName, eastl::shared_ptr<Model> modelToAddTo, aiNode* node, const aiScene* scene, eastl::shared_ptr<Skeleton> skeleton)
	{

		if (node->mParent == nullptr)
			modelToAddTo->SetSkeleton(skeleton);

		// process all node's meshes if any
		// if there aren't any try going through the children nodes
		for (unsigned short i = 0; i < scene->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[i];


			modelToAddTo->AddMesh(ProcessMesh(mesh, scene, skeleton).lock());

			eastl::shared_ptr<Material> material;

#ifdef USING_VULKAN
			material = eastl::shared_ptr<VulkanMaterial>(new VulkanMaterial(scene,
				static_cast<uint32_t>(mesh->mMaterialIndex), modelName));
#endif
#ifdef USING_OPENGL
			material = eastl::shared_ptr<Material>(new Material(scene,
				static_cast<uint32_t>(mesh->mMaterialIndex), modelName));
#endif

			modelToAddTo->SetMeshMaterial(modelToAddTo->GetModelMeshes()[modelToAddTo->GetModelMeshes().size() - 1], eastl::move(material));
		}

		for (unsigned short i = 0; i < node->mNumChildren; i++)
		{
			ProcessModel(modelName, modelToAddTo, node->mChildren[i], scene, skeleton);
		}
	}

	eastl::weak_ptr<Mesh> ResourceManager::ProcessMesh(aiMesh* mesh, const aiScene* scene, eastl::shared_ptr<Skeleton> skeleton)
	{
		eastl::vector<Vertex> vertices;
		eastl::vector<unsigned> indices;
		eastl::vector<eastl::shared_ptr<Texture>> diffuseTextures;
		eastl::vector<eastl::shared_ptr<Texture>> specularTextures;

		//unpack vertices
		for (unsigned i = 0; i < mesh->mNumVertices; i++)
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
			if (mesh->mTextureCoords[0] > nullptr)
			{
				vertex.texCoords.s = mesh->mTextureCoords[0][i].x;
				vertex.texCoords.t = mesh->mTextureCoords[0][i].y;
			}
			else
			{
				//no texture coordinates
				vertex.texCoords = glm::vec2(0.0f, 0.0f);
			}

			vertex.boneWeights[0] = 0.f;
			vertex.boneWeights[1] = 0.f;
			vertex.boneWeights[2] = 0.f;
			vertex.boneWeights[3] = 0.f;
			vertex.boneIds[0] = 0;
			vertex.boneIds[1] = 0;
			vertex.boneIds[2] = 0;
			vertex.boneIds[3] = 0;

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

		//TODO - Parse materials
		if (mesh->mMaterialIndex >= 0)
		{
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		}

		return CreateMesh(mesh, skeleton, vertices, indices);
	}

	eastl::weak_ptr<Texture> ResourceManager::CreateTexture(eastl::string textureName, stbi_uc * data, int width, int height)
	{
		// If already loaded
		eastl::weak_ptr<Texture> textureToReturn = GetTexture(textureName);
		if (textureToReturn.expired() == false && textureToReturn.lock().get() != nullptr)
			return textureToReturn;

#ifdef USING_OPENGL
		eastl::shared_ptr<Texture> createdTexture = eastl::shared_ptr<OpenGLTexture>(new OpenGLTexture(width, height));
#endif
#ifdef USING_VULKAN
		eastl::shared_ptr<Texture> createdTexture = eastl::shared_ptr<VulkanTexture>(new VulkanTexture(width, height));
#endif
		createdTexture->CreateTextureWithData(data, false);

		loadedTextures_.insert(eastl::make_pair(textureName, eastl::move(createdTexture)));

		return loadedTextures_[textureName];
	}

	void ResourceManager::AddTexture(eastl::string textureName, eastl::shared_ptr<Texture> textureToAdd)
	{
		// If a texture with this name has already been loaded
		eastl::weak_ptr<Texture> textureToReturn = GetTexture(textureName);
		if (textureToReturn.expired() == false && textureToReturn.lock().get() != nullptr)
			return;

		loadedTextures_[textureName] = eastl::move(textureToAdd);
	}

	eastl::weak_ptr<Mesh> ResourceManager::GetMesh(eastl::vector<Vertex> vertices, eastl::vector<unsigned> indices)
	{
		if (loadedMeshes_.size() == 0) return eastl::shared_ptr<Mesh>();

		for (size_t i = 0, size = loadedMeshes_.size(); i < size; ++i)
		{
			bool isEqual = true;

			if (loadedMeshes_[i]->vertices.size() != vertices.size()) continue;
			if (loadedMeshes_[i]->indices.size() != indices.size()) continue;

			for (size_t j = 0, verticesSize = loadedMeshes_[i]->vertices.size(); j < verticesSize; ++j)
			{
				Vertex vertex = loadedMeshes_[i]->vertices[j];

				if (vertex.normal != vertices[j].normal) { isEqual = false; break; }
				if (vertex.position != vertices[j].position) { isEqual = false; break; }
				if (vertex.texCoords != vertices[j].texCoords) { isEqual = false; break; }
			}

			if (isEqual == false) continue;

			for (size_t j = 0, indicesSize = loadedMeshes_[i]->indices.size(); j < indicesSize; ++j)
			{
				if (loadedMeshes_[i]->indices[j] != indices[j]) { isEqual = false; break; }
			}

			if (isEqual)
				return loadedMeshes_[i];
		}

		return eastl::shared_ptr<Mesh>();
	}

	eastl::vector<eastl::shared_ptr<Texture>> ResourceManager::ProcessDiffuseTextures(aiMaterial* material)
	{
		eastl::vector<eastl::shared_ptr<Texture>> textures;

		eastl::vector<eastl::shared_ptr<Texture>> diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

		return textures;
	}

	eastl::vector<eastl::shared_ptr<Texture>> ResourceManager::ProcessSpecularTextures(aiMaterial* material)
	{
		eastl::vector<eastl::shared_ptr<Texture>> textures;

		eastl::vector<eastl::shared_ptr<Texture>> specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

		return textures;
	}

	eastl::vector<eastl::shared_ptr<Texture>> ResourceManager::LoadMaterialTextures(aiMaterial* material,
		aiTextureType textureType, eastl::string typeName)
	{
		eastl::vector<eastl::shared_ptr<Texture>> textures;
		for (unsigned short i = 0; i < material->GetTextureCount(textureType); i++)
		{
			aiString str;
			material->GetTexture(textureType, i, &str);
			eastl::string fileName = eastl::string(str.C_Str());

			// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
			bool skip = false;
			for (unsigned short j = 0; j < loadedTextures_.size(); j++)
			{
				if (loadedTextures_.find(fileName) != loadedTextures_.end())
				{
					textures.push_back(loadedTextures_[fileName]);
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

				eastl::weak_ptr<Texture> texture = CreateTexture(fileName);
				textures.push_back(texture.lock());

				// store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
				loadedTextures_.insert(eastl::make_pair(fileName, texture.lock()));
			}
		}
		return textures;
	}
} // namespace Engine