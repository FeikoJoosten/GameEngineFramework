#pragma once
#include "Engine/api.hpp"
#include "Engine/Model/Model.hpp"
#include "Engine/Components/Component.hpp"

namespace Engine
{
	class TransformComponent;

	/// <summary>
	/// This component is used to keep track of the meshes and textures to render. NOTE: only the Entity class is allowed to create this component.
	/// </summary>
	class ENGINE_API ModelComponent : public Component
	{
	private:
		friend class Entity;
		ModelComponent() = default;
		/// <summary>
		/// Create a model component with the give Model.
		/// </summary>
		explicit ModelComponent(eastl::shared_ptr<Model> newModel) noexcept;

		/// <summary>
		/// Create a model component with the given path.
		/// </summary>
		/// <param name="path">This path value is used to load the default mesh for this model component.</param>
		/// <returns></returns>
		explicit ModelComponent(eastl::string path) noexcept;

		/// <summary>
		/// This method is used to initialize your component in. This method is called after the setting of the owner. 
		/// </summary>
		void InitializeComponent() override;

		virtual ~ModelComponent();

	public:
		/// <summary>
		/// This method allows you to change the model to render of this model component.
		/// </summary>
		/// <param name="newModel">The new model to use.</param>
		void SetModel(eastl::shared_ptr<Model> newModel);

		/// <summary>
		/// This method allows you to change the model to render of this model component.
		/// </summary>
		/// <param name="newModel">The path of the new model to use.</param>
		void SetModel(const eastl::string& path);

		/// <summary>
		/// This method will return the current model of this model component.
		/// </summary>
		/// <returns></returns>
		eastl::weak_ptr<Model> GetModel() const;

		/// <summary>
		/// Saves the data of this component to a archive 
		/// </summary>
		/// <param name="ar"> the archive that you want to save to </param>
		template <typename archive>
		void SaveComponent(archive ar);

		/// <summary>
		/// loads the data from the archive to this component
		/// </summary>
		/// <param name="ar"> the archive that you want to load from </param>
		template<typename archive>
		void LoadComponent(archive ar);

	private:
		void Update() override;
		void Render();
		void OnComponentAdded(eastl::weak_ptr<Component> addedComponent) override;
		void OnComponentRemoved(eastl::weak_ptr<Component> removedComponent) override;

		eastl::shared_ptr<Model> model;
		eastl::weak_ptr<TransformComponent> transformComponent;
		eastl::string path;
	};
} //namespace Engine
