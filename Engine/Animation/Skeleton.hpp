// Written by Koen Buitenhuis
#pragma once

#include <ThirdParty/glm/glm/glm.hpp>
#include <ThirdParty/glm/glm/gtc/quaternion.hpp>
#include <ThirdParty/assimp/include/assimp/scene.h>
#include <ThirdParty/assimp/include/assimp/Importer.hpp>

#include <ThirdParty/EASTL-master/include/EASTL/shared_ptr.h>
#include <ThirdParty/EASTL-master/include/EASTL/vector.h>
#include <ThirdParty/EASTL-master/include/EASTL/map.h>

#include "Engine/Texture/Texture.hpp"

namespace Engine {

	class ENGINE_API Skeleton
	{
	protected:

		struct AnimationNode;

	public:
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
		void LoadAnimationSet(const aiScene* scene, eastl::vector<eastl::string> names = {});

		/// <summary>
		/// Destroys the Data, cleaning up the bone tree.
		/// </summary>
		virtual ~Skeleton();

		/// <summary>
		/// Returns a list of the loaded animations as a vector of names. Use these names to load a specific animation.
		/// </summary>
		/// <returns>A vector containing animation names</returns>
		eastl::vector<eastl::string> GetAnimations();

		/// <summary>
		/// Sets the current animation of the mesh.
		/// </summary>
		/// <param name="animation">The name of the animation to use.</param>
		/// <param name="resetTime">If the animation should be reset to the starting point.</param>
		void SetAnimation(eastl::string animation, bool resetTime);

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
		size_t GetAnimationIndex(eastl::string animation);

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
			eastl::string name;
			int boneDataIndex;
			glm::mat4 transform;
			glm::mat4 defaultTransform;
			glm::mat4 offset;
			struct Bone* parent;
			aiNode* node;
			eastl::vector<struct Bone*> childBones;
		}Bone_t;

		/// <summary>
		/// Returns a map that uses the name of a bone as key to return the corresponding bone.
		/// </summary>
		/// <returns>A map containing pointers to bones using the name as key.</returns>
		eastl::map<eastl::string, Bone_t*> GetBoneMap();

		/// <summary>
		/// Returns the root bone of the skeleton.
		/// </summary>
		/// <returns>A pointer to the root bone of the skeleton.</returns>
		Bone_t* GetRootBone();

		/// <summary>
		/// Sets the name of the skeleton.
		/// </summary>
		/// <param name="name">The new name.</param>
		void SetName(eastl::string name);

		/// <summary>
		/// Returns the name of the skeleton (normally the path).
		/// </summary>
		/// <returns>The name of the skeleton.</returns>
		eastl::string GetName();

	protected:

		bool animated;

		typedef struct Animation {
			eastl::string name;
			eastl::vector<struct AnimationNode> nodes;
			float duration; // In ticks
			float ticksPerSecond;
			eastl::shared_ptr<Texture> texture;
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
			eastl::vector<AnimationPositionKey_t> positionKeys;
			eastl::vector<AnimationRotationKey_t> rotationKeys;
			eastl::vector<AnimationScalingKey_t>	scalingKeys;
			aiAnimBehaviour preAnimBehaviour;
			aiAnimBehaviour postAnimBehaviour;
		}AnimationNode_t;

		Bone_t * rootBone;

		eastl::vector<BoneData_t> boneData;

		eastl::map<eastl::string, size_t> animationMap;

		eastl::vector<Animation_t*> animations;

		const aiScene* scene;

		Animation_t* currentAnimation;

		size_t currentAnimationIndex;

		float time;

		float speed;

		bool paused;

		bool looping;

		eastl::map<eastl::string, Bone_t*> boneMap;

		eastl::string name;

		void ReadBones(Bone_t* bone, aiNode* node);

		void DestroyBone(Bone_t* bone);

		void SetBoneTransform(eastl::string bone, glm::mat4 transform);

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