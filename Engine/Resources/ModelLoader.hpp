#ifndef MODEL_LOADER_HPP
#define MODEL_LOADER_HPP

#include "Engine/Model/Model.h"
#include <ThirdParty/EASTL-master/include/EASTL/map.h>

namespace Engine
{
	class ENGINE_API ModelLoader
	{
	public:
		static Model* CreateNewModel(eastl::string modelName);
		static void AddModel(eastl::string modelName, Model* model);
		static Model* GetModel(eastl::string modelName);
		static Texture* CreateTexture(eastl::string textureName);
		static Texture* GetTexture(eastl::string textureName);
		static void CleanUp();
	private:
		static eastl::map <eastl::string, Model*> loadedModels;
		static eastl::map<eastl::string, Texture*> loadedTextures;
	};
} //namespace Engine


#endif
