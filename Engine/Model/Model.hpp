#pragma once
#include "Engine/Api/Api.hpp"
#include "Engine/Mesh/Mesh.hpp"
#include "Engine/Animation/Skeleton.hpp"
#include "Engine/Material/Material.hpp"

namespace Engine
{
	/// <summary>
	/// This object is a storage container for meshes.
	/// </summary>
	class ENGINE_API Model
	{
	public:
		/// <summary>
		/// This method allows you to get a reference of all the meshes in this model.
		/// </summary>
		/// <returns>Returns a shared pointer vector of the meshes bound to this model.</returns>
		std::vector<std::shared_ptr<Mesh>>& GetModelMeshes();

		/// <summary>
		/// This method allows you to get the name of this model.
		/// </summary>
		/// <returns>Returns the name of this model as a std::string.</returns>
		std::string GetName();

		void SetSkeleton(std::shared_ptr<Skeleton> newSkeleton);

		/// <summary>
		/// Returns the animation data of the model. 
		/// The animation data contains the skeleton of the model,
		/// As well as the progress of the animation and a list of animation that can be used by the model.
		/// </summary>
		/// <returns>The animation data of this model.</returns>
		std::shared_ptr<Skeleton> GetSkeleton() const;

		/// <summary>
		/// Returns a list of the loaded animations as a vector of names. Use these names to load a specific animation.
		/// </summary>
		/// <returns>A vector containing animation names</returns>
		void GetAnimations(std::vector<std::string>& output) const;

		/// <summary>
		/// Sets the current animation of the mesh.
		/// </summary>
		/// <param name="animation">The name of the animation to use.</param>
		/// <param name="resetTime">If the animation should be reset to the starting point.</param>
		void SetAnimation(std::string animation, bool resetTime);

		/// <summary>
		/// Sets the current animation to 0, returning the model to the default pose.
		/// </summary>
		void ResetAnimation();

		/// <summary>
		/// Sets the point in the animation the model is in. Set to 0 to reset the animation. 
		/// Animation automatically loops unless specified otherwise by assimp.
		/// </summary>
		/// <param name="time">The time in seconds to set the animation to.</param>
		void SetAnimationTime(float time);

		/// <summary>
		/// Updates the model (doesn't do anything right now).
		/// </summary>
		/// <param name="deltaTime">Time since last frame in seconds.</param>
		void Update(float deltaTime);
		
		/// <summary>
		/// Returns the current progress of the animation.
		/// </summary>
		/// <returns>The time of the animation in seconds (wraps around when looping).</returns>
		float GetAnimationTime() const;

		/// <summary>
		/// Returns the duration of a single loop of the animation.
		/// </summary>
		/// <returns>Duration of the animation in seconds.</returns>
		float GetAnimationDuration() const;

		/// <summary>
		/// Pauses or unpauses the animation.
		/// </summary>
		/// <param name="paused">Boolean indicating the state of the animation</param>
		void SetPaused(bool paused);

		/// <summary>
		/// Tells if the animation is paused or not.
		/// </summary>
		/// <returns>Boolean containing the status of the animation.</returns>
		bool IsPaused() const;

		/// <summary>
		/// Whether or not any animations have been loaded.
		/// </summary>
		/// <returns>Boolean indicating if animations have been loaded or not.</returns>
		bool HasAnimations() const;

		/// <summary>
		/// Whether or not the animation should loop after finishing.
		/// </summary>
		/// <param name="looping">boolean indicating if the animation should loop or not.</param>
		void SetLooping(bool looping);

		/// <summary>
		/// Returns a boolean indicating if the animation is looping or not.
		/// </summary>
		/// <returns>Boolean indicating if the animation is looping or not.</returns>
		bool IsLooping() const;

		/// <summary>
		/// Set the speed modifier of the animation. Default is 1.f.
		/// </summary>
		/// <param name="speed">The new speed modifier.</param>
		void SetSpeed(float speed);

		/// <summary>
		/// Returns the speed modifier of the current animation.
		/// </summary>
		/// <returns>The speed modifier.</returns>
		float GetSpeed() const;

		/// <summary>
		/// Returns the name of the currently selected animation.
		/// </summary>
		/// <returns>The name of the currently selected animation.</returns>
		std::string GetCurrentAnimationName() const;

		/// <summary>
		/// Returns the index of the currently selected animation.
		/// </summary>
		/// <returns>The index of the current animation as a size_t.</returns>
		size_t GetCurrentAnimationIndex() const;

		/// <summary>
		/// Returns the material used by this model for the specified mesh.
		/// If the model doesn't contain a material for the specified mesh a nullptr is returned.
		/// </summary>
		/// <param name="mesh">The mesh to get the material for.</param>
		/// <returns>The material associated with the mesh.</returns>
		std::shared_ptr<Material> GetMeshMaterial(std::shared_ptr<Mesh> mesh);

		/// <summary>
		/// Associates a material with the given mesh. 
		/// Warning: If building for vulkan, make sure that the specified material is created as a VulkanMaterial.
		/// </summary>
		/// <param name="mesh">The mesh to use the material for.</param>
		/// <param name="material">The material to bind to the mesh. 
		/// Note: If you update the material, any mesh that's associated with this material will change.</param>
		void SetMeshMaterial(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material);

		/// <summary>
		/// This method allows you to compare a model against another model.
		/// </summary>
		/// <param name="other">The model you want to compare against.</param>
		/// <returns>Returns true if both models are equal.</returns>
		bool operator==(Model& other);
		/// <summary>
		/// This method allows you to compare a model against another model.
		/// </summary>
		/// <param name="other">The model you want to compare against.</param>
		/// <returns>Returns true if the models are not equal.</returns>
		bool operator!=(Model& other);
	protected:
		//Model data

		std::string name;
		friend class ResourceManager;
		explicit Model(const aiScene* scene, std::string name = "");
	public:
		~Model() = default;
	protected:

		void UpdateAnimation(float deltaTime);

		std::shared_ptr<Skeleton> skeleton;

		void AddMesh(std::shared_ptr<Mesh> meshToAdd);
		std::vector<std::shared_ptr<Mesh>> meshes;

		std::map<std::shared_ptr<Mesh>, std::shared_ptr<Material>> meshMaterialMap;

#pragma region AnimationData

		float speed;

		bool looping;

		bool paused;

		float time;

		size_t currentAnimation;

		std::string currentAnimationName;

#pragma endregion
	};
} // namespace Engine