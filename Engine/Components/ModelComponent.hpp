#pragma once
#include "Engine/Components/Component.hpp"
#include "Engine/api.hpp"
#include "Engine/Resources/ModelLoader.hpp"
#include "Engine/Model/Model.h"

/**
* @
* This component stores the Model (mesh+textures)* 
*/

namespace Engine
{
	class ENGINE_API ModelComponent : public Component
	{
	public:

		ModelComponent() = delete;
		/**
		 * @brief
		 * Initialize the model component with model*
		 */
		ModelComponent(Model* newModel);
		/**
		 * Initialize the model component using a path.
		 * The model loader takes it from here and check whether the model is already loaded
		 */
		ModelComponent(eastl::string path);
		~ModelComponent();

		void SetModel(Model* newModel) { model = newModel; }
		Model* GetModel() const { return model; }
	private:
		Model* model;
	};
} //namespace Engine
