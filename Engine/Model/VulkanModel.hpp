#pragma once

#include "Engine/Model/Model.h"

namespace Engine
{
	class ENGINE_API VulkanModel : public Model
	{
	public:
		VulkanModel() = delete;
		explicit VulkanModel(eastl::string path)
		{
			LoadModel(eastl::string("Resources/Models/" +path));
		}
	private:
		void ProcessNode(aiNode* node, const aiScene* scene) override;
		eastl::vector<Texture*> ProcessTextures(aiMaterial* material) override;
	};
} //namespace Engine