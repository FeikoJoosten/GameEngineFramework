#pragma once

#include "Engine/Api/Api.hpp"
#include "Engine/Engine/Engine.hpp"
#include "Engine/Model/Model.hpp"

#include <assimp/scene.h>

namespace Engine
{
	/// <summary>
	/// This class is used to create models, load in meshes and textures. NOTE: Only the Engine is allowed to create this object.
	/// </summary>
	class ENGINE_API ResourceManager
	{
		friend std::shared_ptr<ResourceManager> Engine::GetResourceManager() noexcept;
		ResourceManager() = default;
	public:
		~ResourceManager() = default;
		ResourceManager(const ResourceManager& other) = delete;
		ResourceManager(ResourceManager&& other) noexcept = delete;

		ResourceManager& operator=(const ResourceManager& other) = delete;
		ResourceManager& operator=(ResourceManager&& other) noexcept = delete;

		static std::shared_ptr<ResourceManager> Get();

		/// <summary>
		/// This method allows you to get a model with a specific name.
		/// </summary>
		/// <param name="modelName">The model name you are looking for.</param>
		/// <returns>Returns a shared pointer to the model is it was found. Otherwise it will return a empty shared pointer.</returns>
		std::weak_ptr<Model> GetModel(std::string modelName);
		/// <summary>
		/// This method will allow you to create a new model with the model name.
		/// </summary>
		/// <param name="modelName">The model you want to create.</param>
		/// <param name="meshToLoad">Optional parameter where you can add a mesh you want to load in. NOTE: The mesh needs to be located under 'Resources/Models/'
		/// You then give the mesh name + its extension. The mesh can be in a subfolder of 'Resources/Meshes/'</param>
		/// <param name="skeleton">Optional parameter to load a specific skeleton with the model. If not specified the function will load the skeleton from
		/// the same file as the model. NOTE: The skeleton needs to be located under 'Resources/Models/'
		/// You then give the mesh name + its extension. The skeleton can be in a subfolder of 'Resources/Models/'</param>
		/// <returns>Will return a new model, optionally with an already loaded mesh and skeleton.</returns>
		std::weak_ptr<Model> CreateModel(std::string modelName, std::string meshToLoad = "", std::string skeleton = "");
		/// <summary>
		/// This will load the skeleton data from the file specified. NOTe: This method will not load in the skeleton again, if it's already loaded in.
		/// </summary>
		/// <param name="skeletonToLoad">The file to load the skeleton from. NOTE: The skeleton needs to be located under 'Resources/Models/'
		/// You then give the mesh name + its extension. The skeleton can be in a subfolder of 'Resources/Models/'</param>
		/// <returns>Will return a new skeleton.</returns>
		std::weak_ptr<Skeleton> CreateSkeleton(std::string skeletonToLoad);
		/// <summary>
		/// This method loads animations from a file, and adds them to the skeleton with the specified name.
		/// </summary>
		/// <param name="skeletonName">The model to add animations to.</param>
		/// <param name="animationsToLoad">The file containing the animations. NOTE: The animation file needs to be located under 'Resources/Animations/'
		/// You then give the animation file name + its extension. The file can be in a subfolder of 'Resources/Animations/'</param>
		/// <param name="names">A vector containing names for the animations being loaded. 
		/// If animation n in the file has a name at position n in the vector that name will be used instead.
		/// Leave the name at position n blank to use the name supplied by the file instead.</param>
		void AddAnimationsToSkeleton(std::string skeletonName, std::string animationsToLoad = "", std::vector<std::string> names = {});
		/// <summary>
		/// This method allows you to create a new mesh. NOTE: This method will not load in the mesh again, if it's already loaded in.
		/// </summary>
		/// <param name="mesh">The mesh you want to load in.</param>
		/// <param name="skeleton">The skeleton the mesh is bound to. Passing a nullptr will create a non-animated mesh.</param>
		/// <param name="vertices">The vertices of the mesh.</param>
		/// <param name="indices">The indices of the mesh.</param>
		/// <returns>Returns a shared_ptr of the mesh you want to create.</returns>
		std::weak_ptr<Mesh> CreateMesh(aiMesh *mesh, std::shared_ptr<Skeleton> skeleton, std::vector<Vertex> vertices, std::vector<unsigned> indices);
		/// <summary>
		/// This method allows you to get a texture with the defined name.
		/// </summary>
		/// <param name="textureName">The texture you want to find. NOTE: The texture needs to be located under 'Resources/Textures/'. You then give the texture name + extension.
		/// The texture can be in a subfolder of 'Resources/Textures/'.</param>
		/// <returns>Returns a shared pointer of the texture you want to get. Will return an empty shared pointer if the texture hasn't been loaded yet.</returns>
		std::weak_ptr<Texture> GetTexture(std::string textureName);
		/// <summary>
		/// This method allows you to create a new texture with the given name.
		/// </summary>
		/// <param name="textureName">The texture you want to create. NOTE: The texture needs to be located under 'Resources/Textures/'. You then give the texture name + extension.
		/// The texture can be in a subfolder of of 'Resources/Textures/'.</param>
		/// <returns>Returns a reference to the just created texture. Or if the texture was already created before, it'll return a reference to that texture.</returns>
		std::weak_ptr<Texture> CreateTexture(std::string textureName);
		/// <summary>
		/// Creates a new texture with the given name, using the supplied data as source.
		/// </summary>
		/// <param name="textureName">The name for the texture you want to create.</param>
		/// <param name="data">The raw data of the texture you want to create. NOTE: The data must be of RGBA8 format.</param>
		/// <param name="width">The width of the texture.</param>
		/// <param name="height">The height of the texture.</param>
		/// <returns></returns>
		std::weak_ptr<Texture> CreateTexture(std::string textureName, stbi_uc* data, int width, int height);
		
	private:

		friend class Model;
		void AddTexture(std::string textureName, std::shared_ptr<Texture> textureToAdd);
		std::weak_ptr<Mesh> GetMesh(std::vector<Vertex> vertices, std::vector<unsigned> indices);
		void ProcessModel(std::string modelName, std::shared_ptr<Model> modelToAddTo, aiNode* node, const aiScene* scene, std::shared_ptr<Skeleton> skeleton);
		std::weak_ptr<Mesh> ProcessMesh(aiMesh* mesh, const aiScene* scene, std::shared_ptr<Skeleton> skeleton);
		std::vector<std::shared_ptr<Texture>> ProcessDiffuseTextures(aiMaterial* material);
		std::vector<std::shared_ptr<Texture>> ProcessSpecularTextures(aiMaterial* material);
		std::vector<std::shared_ptr<Texture>> LoadMaterialTextures(aiMaterial* material, aiTextureType textureType, std::string typeName);

		std::vector<std::shared_ptr<Model>> loadedModels_;
		std::vector<std::shared_ptr<Skeleton>> loadedSkeletons_;
		std::vector<std::shared_ptr<Mesh>> loadedMeshes_;
		std::map<std::string, std::shared_ptr<Texture>> loadedTextures_;
	};
} // namespace Engine