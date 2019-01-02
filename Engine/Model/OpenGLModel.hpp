#pragma once

#include "Engine/api.hpp"
#include "Engine/Model/Model.h"

namespace Engine
{
	class ENGINE_API OpenGLModel : public Model
	{
	public:
		OpenGLModel() = delete;
		explicit OpenGLModel(eastl::string path)
		{
			LoadModel(eastl::string("Resources/Models/" + path));
		}
	private:
		void ProcessNode(aiNode* node, const aiScene* scene) override;
		eastl::vector<Texture*> ProcessTextures(aiMaterial* material) override;
	};
} // namespace Engine
