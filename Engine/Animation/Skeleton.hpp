// Written by Koen Buitenhuis
#pragma once

#include "Engine/Texture/Texture.hpp"

#include <map>
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>

namespace Engine {

	class ENGINE_API Skeleton
	{
	protected:

		struct AnimationNode;

	public:
		friend class Model;
		/// <summary>
		/// Creates a new animation data object, loading in the animation data from the provided scene.
		/// </summary>
		/// <param name="scene">scene containing the model skeleton and animation data.</param>
		Skeleton(const aiScene* scene);

		/// <summary>
		/// Loads in animation data form the provided scene.
		/// If the animation data does not match the skeleton of the model (using bone names that are not in the skeleton)
		/// a error gets reported but the data will be loaded.
		/// The skeleton data used will be the data from the scene used to first initialize the object.
		/// </summary>
		/// <param name="scene">Scene to load additional animations from.</param>
		/// <param name="names">The optional names of the animation sets to load.</param>
		void LoadAnimationSet(const aiScene* scene, std::vector<std::string> names = {});

		/// <summary>
		/// Destroys the Data, cleaning up the bone tree.
		/// </summary>
		virtual ~Skeleton();

		/// <summary>
		/// Returns a list of the loaded animations as a vector of names. Use these names to load a specific animation.
		/// </summary>
		/// <returns>A vector containing animation names</returns>
		void GetAnimations(std::vector<std::string>& output);

		/// <summary>
		/// Sets the current animation of the mesh.
		/// </summary>
		/// <param name="animation">The name of the animation to use.</param>
		/// <param name="resetTime">If the animation should be reset to the starting point.</param>
		void SetAnimation(std::string animation, bool resetTime);

		/// <summary>
		/// Returns the duration of a single loop of the animation.
		/// </summary>
		/// <param name="animation">The index of the animation to return the duration of.</param>
		/// <returns>Duration of the animation in seconds.</returns>
		float GetAnimationDuration(size_t animation);
		
		/// <summary>
		/// Whether or not any animations have been loaded.
		/// </summary>
		/// <returns>Boolean indicating if animations have been loaded or not.</returns>
		bool HasAnimations();

		/// <summary>
		/// Returns a texture containing the animation snapshots for the current animation.
		/// </summary>
		/// <param name="animation">The animation to get the data for.</param>
		/// <returns>A shared pointer to a texture containing the data.</returns>
		Texture* GetAnimationData(size_t animation);

		/// <summary>
		/// Returns a texture containing the animation snapshots for the current animation.
		/// </summary>
		/// <param name="animation">The name of the animation to get the data for.</param>
		/// <returns>A shared pointer to a texture containing the data.</returns>
		size_t GetAnimationIndex(std::string animation);

		/// <summary>
		/// Returns the number of animation ticks per second.
		/// This is equal to the number of animation snapshots per second.
		/// </summary>
		/// <param name="animation">The animation to get the data from.</param>
		/// <returns>The number of ticks per second as a float.</returns>
		float GetAnimationTicksPerSecond(size_t animation);

		/// <summary>
		/// A structure containing information a bone.
		/// </summary>
		typedef struct Bone {
			std::string name;
			int boneDataIndex;
			glm::mat4 transform;
			glm::mat4 defaultTransform;
			glm::mat4 offset;
			struct Bone* parent;
			aiNode* node;
			std::vector<struct Bone*> childBones;
		}Bone_t;

		/// <summary>
		/// Returns a map that uses the name of a bone as key to return the corresponding bone.
		/// </summary>
		/// <returns>A map containing pointers to bones using the name as key.</returns>
		std::map<std::string, Bone_t*> GetBoneMap();

		/// <summary>
		/// Returns the root bone of the skeleton.
		/// </summary>
		/// <returns>A pointer to the root bone of the skeleton.</returns>
		Bone_t* GetRootBone();

		/// <summary>
		/// Sets the name of the skeleton.
		/// </summary>
		/// <param name="name">The new name.</param>
		void SetName(std::string name);

		/// <summary>
		/// Returns the name of the skeleton (normally the path).
		/// </summary>
		/// <returns>The name of the skeleton.</returns>
		std::string GetName();

	protected:

		bool animated;

		typedef struct Animation {
			std::string name;
			std::vector<struct AnimationNode> nodes;
			float duration; // In ticks
			float ticksPerSecond;
			std::shared_ptr<Texture> texture;
		}Animation_t;

		typedef struct {
			glm::mat4 transform;
		}BoneData_t;

		typedef struct {
			float time;
			glm::vec3 position;
		}AnimationPositionKey_t;

		typedef struct {
			float time;
			glm::quat rotation;
		}AnimationRotationKey_t;

		typedef struct {
			float time;
			glm::vec3 scale;
		}AnimationScalingKey_t;

		typedef struct AnimationNode {
			Bone_t* bone;
			std::vector<AnimationPositionKey_t> positionKeys;
			std::vector<AnimationRotationKey_t> rotationKeys;
			std::vector<AnimationScalingKey_t>	scalingKeys;
			aiAnimBehaviour preAnimBehaviour;
			aiAnimBehaviour postAnimBehaviour;
		}AnimationNode_t;

		Bone_t * rootBone;

		std::vector<BoneData_t> boneData;

		std::map<std::string, size_t> animationMap;

		std::vector<Animation_t*> animations;

		const aiScene* scene;

		Animation_t* currentAnimation;

		size_t currentAnimationIndex;

		float time;

		float speed;

		bool paused;

		bool looping;

		std::map<std::string, Bone_t*> boneMap;

		std::string name;

		void ReadBones(Bone_t* bone, aiNode* node);

		void DestroyBone(Bone_t* bone);

		void SetBoneTransform(std::string bone, glm::mat4 transform);

		void SetBoneTransform(Bone_t* bone, glm::mat4 transform);

		void UpdateBoneTransformData(Bone_t* bone);

		void UpdateBoneTreeTransformData();

		void Update(float deltaTime);

		virtual void UpdateBoneBuffers();

		glm::mat4 InterpolateScale(float time, AnimationNode_t node);
		glm::mat4 InterpolateRotation(float time, AnimationNode_t node);
		glm::mat4 InterpolatePosition(float time, AnimationNode_t node);

		glm::mat4 TransformToMat4(aiMatrix4x4 transform);
	};

}