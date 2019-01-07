#pragma once

#include "Engine/api.hpp"
#include "Engine/Components/Component.hpp"
#include "Engine/Components/ModelComponent.hpp"

#include <ThirdParty/EASTL-master/include/EASTL/shared_ptr.h>

namespace Engine {

	class ENGINE_API AnimationComponent : public Component
	{
	public:
		~AnimationComponent();

		/// <summary>
		/// Returns all model components owned by the entity.
		/// </summary>
		/// <returns>A vector of weak pointers to model components.</returns>
		eastl::vector<eastl::weak_ptr<ModelComponent>> GetEntityModels() const;

		/// <summary>
		/// Returns the names of all animations of the given model component.
		/// </summary>
		/// <param name="modelComponent">The model component to get the animations from.</param>
		/// <returns>A vector of strings.</returns>
		eastl::vector<eastl::string> GetModelAnimations(eastl::weak_ptr<ModelComponent> modelComponent);

		/// <summary>
		/// Returns the names of all animations of the given model component.
		/// </summary>
		/// <param name="index">The index into the model components vector to get the animations from.</param>
		/// <returns>A vector of strings.</returns>
		eastl::vector<eastl::string> GetModelAnimations(size_t index);

		/// <summary>
		/// Sets the animation of the specified model to the animation with the specified name.
		/// </summary>
		/// <param name="name">The name of the animation.</param>
		/// <param name="modelComponent">The model component to set the animation for.</param>
		/// <param name="resetTime">Whether or not the animation time needs to be reset.</param>
		void SetModelAnimation(eastl::weak_ptr<ModelComponent> modelComponent, eastl::string name, bool resetTime);
		
		/// <summary>
		/// Sets the animation of the specified model to the animation with the specified name.
		/// </summary>
		/// <param name=" index">The index of the model component.</param>
		/// <param name="name">The name of the animation.</param>
		/// <param name="resetTime">Whether or not the animation time needs to be reset.</param>
		void SetModelAnimation(size_t index, eastl::string name, bool resetTime);

		/// <summary>
		/// Returns the name of the model's current animation.
		/// </summary>
		/// <param name="modelComponent">The model component to get the name from.</param>
		/// <returns>The name of the animation.</returns>
		eastl::string GetModelCurrentAnimation(eastl::weak_ptr<ModelComponent> modelComponent);

		/// <summary>
		/// Returns the name of the model's current animation.
		/// </summary>
		/// <param name="index">The index of the model component to get the name from.</param>
		/// <returns>The name of the animation.</returns>
		eastl::string GetModelCurrentAnimation(size_t index);

		/// <summary>
		/// Resets the animation of the specified model component.
		/// </summary>
		/// <param name="ModelComponent">The model component to reset the animation for.</param>
		void ResetModelAnimation(eastl::weak_ptr<ModelComponent> modelComponent);

		/// <summary>
		/// Resets the animation of the specified model component.
		/// </summary>
		/// <param name="index">The index of the model component to reset the animation for.</param>
		void ResetModelAnimation(size_t index);

		/// <summary>
		/// Sets the progress of the model's animation to the specified time.
		/// </summary>
		/// <param name="modelComponent">The model component to affect.</param>
		/// <param name="time">The new time in seconds.</param>
		void SetModelAnimationTime(eastl::weak_ptr<ModelComponent> modelComponent, float time);

		/// <summary>
		/// Sets the progress of the model's animation to the specified time.
		/// </summary>
		/// <param name="index">The index of the model component to affect.</param>
		/// <param name="time">The new time in seconds.</param>
		void SetModelAnimationTime(size_t index, float time);

		/// <summary>
		/// Returns the progress of the model's current animation.
		/// </summary>
		/// <param name="modelComponent">The model component to get the time from.</param>
		/// <returns>The time in seconds.</returns>
		float GetModelAnimationTime(eastl::weak_ptr<ModelComponent> modelComponent);

		/// <summary>
		/// Returns the progress of the model's current animation.
		/// </summary>
		/// <param name="index">The index of the model component to get the time from.</param>
		/// <returns>The time in seconds.</returns>
		float GetModelAnimationTime(size_t index);

		/// <summary>
		/// Returns the duration of the model's current animation.
		/// </summary>
		/// <param name="modelComponent">The model component to get the duration from.</param>
		/// <returns>The duration in seconds.</returns>
		float GetModelAnimationDuration(eastl::weak_ptr<ModelComponent> modelComponent);

		/// <summary>
		/// Returns the duration of the model's current animation.
		/// </summary>
		/// <param name="index">The index of the model component to get the duration from.</param>
		/// <returns>The duration in seconds.</returns>
		float GetModelAnimationDuration(size_t index);

		/// <summary>
		/// Sets whether or not the model's animation should be paused.
		/// </summary>
		/// <param name="modelComponent">The model component to pause the animation of.</param>
		/// <param name="paused">Whether or not the animation should be paused.</param>
		void SetModelAnimationPaused(eastl::weak_ptr<ModelComponent> modelComponent, bool paused);

		/// <summary>
		/// Sets whether or not the model's animation should be paused.
		/// </summary>
		/// <param name="index">The index of the model component to pause the animation of.</param>
		/// <param name="paused">Whether or not the animation should be paused.</param>
		void SetModelAnimationPaused(size_t index, bool paused);

		/// <summary>
		/// Returns whether or not the model's animation is paused.
		/// </summary>
		/// <param name="modelComponent">The model component to get the status from.</param>
		/// <returns>Whether or not the animation is paused.</returns>
		bool isModelAnimationPaused(eastl::weak_ptr<ModelComponent> modelComponent);

		/// <summary>
		/// Returns whether or not the model's animation is paused.
		/// </summary>
		/// <param name="index">The index of the model component to get the status from.</param>
		/// <returns>Whether or not the animation is paused.</returns>
		bool isModelAnimationPaused(size_t index);

		/// <summary>
		/// Sets whether or not the model's animation should loop.
		/// </summary>
		/// <param name="modelComponent">The model component to set the status of.</param>
		/// <param name="looping">Whether or not the animation should loop.</param>
		void SetModelAnimationLooping(eastl::weak_ptr<ModelComponent> modelComponent, bool looping);

		/// <summary>
		/// Sets whether or not the model's animation should loop.
		/// </summary>
		/// <param name="index">The index of the model component to set the status of.</param>
		/// <param name="looping">Whether or not the animation should loop.</param>
		void SetModelAnimationLooping(size_t index, bool looping);

		/// <summary>
		/// Returns whether or not the model's animation is looping.
		/// </summary>
		/// <param name="modelComponent">The model component to get the status from.</param>
		/// <returns>Whether or not the animation is looping.</returns>
		bool IsModelAnimationLooping(eastl::weak_ptr<ModelComponent> modelComponent);

		/// <summary>
		/// Returns whether or not the model's animation is looping.
		/// </summary>
		/// <param name="index">The index of model component to get the status from.</param>
		/// <returns>Whether or not the animation is looping.</returns>
		bool IsModelAnimationLooping(size_t index);

		/// <summary>
		/// Sets the speed modifier of the model's animation. The default is 1.0f.
		/// </summary>
		/// <param name="modelComponent">The model component to set the speed for.</param>
		/// <param name="speed">The new speed.</param>
		void SetModelAnimationSpeed(eastl::weak_ptr<ModelComponent> modelComponent, float speed);

		/// <summary>
		/// Sets the speed modifier of the model's animation. The default is 1.0f.
		/// </summary>
		/// <param name="index">The index of the model component to set the speed for.</param>
		/// <param name="speed">The new speed.</param>
		void SetModelAnimationSpeed(size_t index, float speed);

		/// <summary>
		/// Returns the speed modifier of the model's animation.
		/// </summary>
		/// <param name="modelComponent">The model component to get the speed from.</param>
		/// <returns>The speed of the animation.</returns>
		float GetModelAnimationSpeed(eastl::weak_ptr<ModelComponent> modelComponent);

		/// <summary>
		/// Returns the speed modifier of the model's animation.
		/// </summary>
		/// <param name="index">The index of the model component to get the speed from.</param>
		/// <returns>The speed of the animation.</returns>
		float GetModelAnimationSpeed(size_t index);


	private:
		friend class Entity;

		explicit AnimationComponent() noexcept;
		
		void InitializeComponent() override;

		void Update() override;

		void OnComponentAdded(eastl::weak_ptr<Component> addedComponent) override;
		void OnComponentRemoved(eastl::weak_ptr<Component> removedComponent) override;

		eastl::vector<eastl::weak_ptr<ModelComponent>> entityModels;
	};

}
