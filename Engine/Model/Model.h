#ifndef MODEL_H
#define MODEL_H

#include "Engine/api.hpp"
#include "Engine/Mesh/Mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <ThirdParty/EASTL-master/include/EASTL/string.h>
#include <ThirdParty/EASTL-master/include/EASTL/vector.h>

namespace Engine
{
	//class Mesh;
	//class Texture;

	class ENGINE_API Model
	{
	public:
		Model() = default;

		/*
		 * @returns the model's meshes - each mesh contains a set of vertices, indicies and textures
		 */
		eastl::vector<Mesh*>& GetModelMeshes() { return meshes; };

		virtual ~Model() = default;
	protected:
		//Model data
		eastl::vector<Mesh*> meshes;
		eastl::vector<Texture*> loadedTextures;
		eastl::string directory;

		//load the model in
		void LoadModel(eastl::string path);
		//Process the nodes and pushesh the info into the meshes vector
		virtual void ProcessNode(aiNode* node, const aiScene* scene);
		virtual eastl::vector<Texture*> ProcessTextures(aiMaterial* material);
		//Process the mesh
		Mesh* ProcessMesh(aiMesh* mesh, const aiScene* scene);
		eastl::vector<Texture*> LoadMaterialTextures(aiMaterial* material, aiTextureType type, eastl::string type_name);

		friend class ModelLoader;
		explicit Model(eastl::string path)
		{
			LoadModel(eastl::string("Resources/Models/" + path));
		}
	};
} // namespace Engine
#endif
