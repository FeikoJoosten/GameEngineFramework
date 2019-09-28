#pragma once

#include "Engine/api.hpp"
#include "Engine/Model/Model.hpp"

#include <ThirdParty/assimp/include/assimp/Importer.hpp>
#include <ThirdParty/assimp/include/assimp/scene.h>
#include <ThirdParty/EASTL-master/include/EASTL/map.h>
#include <ThirdParty/EASTL-master/include/EASTL/shared_ptr.h>

namespace Engine
{
	/// <summary>
	/// This class is used to create models, load in meshes and textures. NOTE: Only the Engine is allowed to create this object.
	/// </summary>
	class ENGINE_API ResourceManager
	{
	public:
		/// <summary>
		/// This method allows you to get a model with a specific name.
		/// </summary>
		/// <param name="modelName">The model name you are looking for.</param>
		/// <returns>Returns a shared pointer to the model is it was found. Otherwise it will return a empty shared pointer.</returns>
		eastl::weak_ptr<Model> GetModel(eastl::string modelName);
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
		eastl::weak_ptr<Model> CreateModel(eastl::string modelName, eastl::string meshToLoad = "", eastl::string skeleton = "");
		/// <summary>
		/// This will load the skeleton data from the file specified. NOTe: This method will not load in the skeleton again, if it's already loaded in.
		/// </summary>
		/// <param name="skeletonToLoad">The file to load the skeleton from. NOTE: The skeleton needs to be located under 'Resources/Models/'
		/// You then give the mesh name + its extension. The skeleton can be in a subfolder of 'Resources/Models/'</param>
		/// <returns>Will return a new skeleton.</returns>
		eastl::weak_ptr<Skeleton> CreateSkeleton(eastl::string skeletonToLoad);
		/// <summary>
		/// This method loads animations from a file, and adds them to the skeleton with the specified name.
		/// </summary>
		/// <param name="skeletonName">The model to add animations to.</param>
		/// <param name="animationsToLoad">The file containing the animations. NOTE: The animation file needs to be located under 'Resources/Animations/'
		/// You then give the animation file name + its extension. The file can be in a subfolder of 'Resources/Animations/'</param>
		/// <param name="names">A vector containing names for the animations being loaded. 
		/// If animation n in the file has a name at position n in the vector that name will be used instead.
		/// Leave the name at position n blank to use the name supplied by the file instead.</param>
		void AddAnimationsToSkeleton(eastl::string skeletonName, eastl::string animationsToLoad = "", eastl::vector<eastl::string> names = {});
		/// <summary>
		/// This method allows you to create a new mesh. NOTE: This method will not load in the mesh again, if it's already loaded in.
		/// </summary>
		/// <param name="mesh">The mesh you want to load in.</param>
		/// <param name="skeleton">The skeleton the mesh is bound to. Passing a nullptr will create a non-animated mesh.</param>
		/// <param name="vertices">The vertices of the mesh.</param>
		/// <param name="indices">The indices of the mesh.</param>
		/// <returns>Returns a shared_ptr of the mesh you want to create.</returns>
		eastl::weak_ptr<Mesh> CreateMesh(aiMesh *mesh, eastl::shared_ptr<Skeleton> skeleton, eastl::vector<Vertex> vertices, eastl::vector<unsigned> indices);
		/// <summary>
		/// This method allows you to get a texture with the defined name.
		/// </summary>
		/// <param name="textureName">The texture you want to find. NOTE: The texture needs to be located under 'Resources/Textures/'. You then give the texture name + extension.
		/// The texture can be in a subfolder of 'Resources/Textures/'.</param>
		/// <returns>Returns a shared pointer of the texture you want to get. Will return an empty shared pointer if the texture hasn't been loaded yet.</returns>
		eastl::weak_ptr<Texture> GetTexture(eastl::string textureName);
		/// <summary>
		/// This method allows you to create a new texture with the given name.
		/// </summary>
		/// <param name="textureName">The texture you want to create. NOTE: The texture needs to be located under 'Resources/Textures/'. You then give the texture name + extension.
		/// The texture can be in a subfolder of of 'Resources/Textures/'.</param>
		/// <returns>Returns a reference to the just created texture. Or if the texture was already created before, it'll return a reference to that texture.</returns>
		eastl::weak_ptr<Texture> CreateTexture(eastl::string textureName);
		/// <summary>
		/// Creates a new texture with the given name, using the supplied data as source.
		/// </summary>
		/// <param name="textureName">The name for the texture you want to create.</param>
		/// <param name="data">The raw data of the texture you want to create. NOTE: The data must be of RGBA8 format.</param>
		/// <param name="width">The width of the texture.</param>
		/// <param name="height">The height of the texture.</param>
		/// <returns></returns>
		eastl::weak_ptr<Texture> CreateTexture(eastl::string textureName, stbi_uc* data, int width, int height);

	private:
		friend class Engine;
		ResourceManager() = default;
	public:
		~ResourceManager() = default;
	private:

		friend class Model;
		void AddTexture(eastl::string textureName, eastl::shared_ptr<Texture> textureToAdd);
		eastl::weak_ptr<Mesh> GetMesh(eastl::vector<Vertex> vertices, eastl::vector<unsigned> indices);
		void ProcessModel(eastl::string modelName, eastl::shared_ptr<Model> modelToAddTo, aiNode* node, const aiScene* scene, eastl::shared_ptr<Skeleton> skeleton);
		eastl::weak_ptr<Mesh> ProcessMesh(aiMesh* mesh, const aiScene* scene, eastl::shared_ptr<Skeleton> skeleton);
		eastl::vector<eastl::shared_ptr<Texture>> ProcessDiffuseTextures(aiMaterial* material);
		eastl::vector<eastl::shared_ptr<Texture>> ProcessSpecularTextures(aiMaterial* material);
		eastl::vector<eastl::shared_ptr<Texture>> LoadMaterialTextures(aiMaterial* material, aiTextureType textureType, eastl::string typeName);

		eastl::vector<eastl::shared_ptr<Model>> loadedModels_;
		eastl::vector<eastl::shared_ptr<Skeleton>> loadedSkeletons_;
		eastl::vector<eastl::shared_ptr<Mesh>> loadedMeshes_;
		eastl::map<eastl::string, eastl::shared_ptr<Texture>> loadedTextures_;
	};
} // namespace Engine