#include "Engine/Resources/ModelLoader.hpp"
#include "Engine/Utility/Defines.hpp"

#if RENDERER == 1
#include "Engine/Model/OpenGLModel.hpp"
#include "Engine/Texture/OpenGLTexture.hpp"
#elif RENDERER == 2
#include "Engine/Model/VulkanModel.hpp"
#include "Engine/Texture/VulkanTexture.hpp"
#else
#include "Engine/Model/Model.hpp"
#include "Engine/Texture/Texture.hpp"
#endif

namespace Engine
{
	Model* ModelLoader::CreateNewModel(eastl::string modelName)
	{
		auto result = loadedModels.find(modelName);
		if (loadedModels.find(modelName) == loadedModels.end())
		{
#if RENDERER == 1
			OpenGLModel* model = new OpenGLModel(modelName);
#elif RENDERER == 2
			VulkanModel* model = new VulkanModel(modelName);
#else
			Model* model = new Model(modelName);
#endif
			loadedModels.insert(eastl::make_pair(modelName, model));
			return loadedModels.find(modelName)->second;
		}
		else
		{
			return result->second;
		}
	}

	void ModelLoader::AddModel(eastl::string modelName, Model* model)
	{
		if (GetModel(modelName) == nullptr) return;

		loadedModels.insert(eastl::make_pair(modelName, model));
	}

	Model* ModelLoader::GetModel(eastl::string modelName)
	{
		if (loadedModels.size() == 0) return nullptr;
		auto it = loadedModels.find(modelName);

		if (it == loadedModels.end()) return nullptr;
		return it->second;
	}

	Texture* ModelLoader::CreateTexture(eastl::string textureName)
	{
		Texture* texture = GetTexture(textureName);
		if (GetTexture(textureName) != nullptr) return texture;

#if RENDERER == 1
		texture = new OpenGLTexture(textureName);
#elif RENDERER == 2
		texture = new VulkanTexture(textureName);
#else
		texture = new Texture(textureName);
#endif
		loadedTextures.insert(eastl::make_pair(textureName, texture));

		return texture;
	}

	Texture* ModelLoader::GetTexture(eastl::string textureName)
	{
		if (loadedTextures.size() == 0) return nullptr;
		auto it = loadedTextures.find(textureName);

		if (it == loadedTextures.end()) return nullptr;
		return it->second;
	}

	void ModelLoader::CleanUp()
	{
		for (auto it = loadedModels.begin(); it != loadedModels.end(); ++it)
		{
			delete it->second;
			loadedModels.erase(it);
		}

		for (auto it = loadedTextures.begin(); it != loadedTextures.end(); ++it)
		{
			if (it->second != nullptr)
				delete it->second;
			loadedTextures.erase(it);
		}
	}

	eastl::map<eastl::string, Model*> ModelLoader::loadedModels;
	eastl::map<eastl::string, Texture*> ModelLoader::loadedTextures;
} // namespace Engine