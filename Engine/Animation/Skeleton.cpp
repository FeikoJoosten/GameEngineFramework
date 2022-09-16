#include "Engine/Animation/Skeleton.hpp"

#include "Engine/Utility/Defines.hpp"

#ifdef USING_OPENGL
#include "Engine/Texture/OpenGLTexture.hpp"
#endif
#ifdef USING_VULKAN
#include "Engine/Texture/VulkanTexture.hpp"
#endif

#include <iostream>
#include <ThirdParty/glm/glm/gtc/matrix_transform.hpp>

namespace Engine {

	Skeleton::Skeleton(const aiScene* scene) {
		this->scene = scene;

		if (scene->HasAnimations()) {

			animated = true;

			speed = 1.f;

			boneData.resize(0xff, { glm::mat4() });

			rootBone = new Bone_t;

			rootBone->boneDataIndex = -1;
			rootBone->parent = nullptr;

			ReadBones(rootBone, scene->mRootNode);

			for (unsigned int i = 0; i < scene->mNumAnimations; ++i) {
				Animation_t* animation = new Animation_t;
				animation->name = std::string(scene->mAnimations[i]->mName.C_Str());
				animation->duration = static_cast<float>(scene->mAnimations[i]->mDuration);
				animation->ticksPerSecond = static_cast<float>(scene->mAnimations[i]->mTicksPerSecond);

				animations.push_back(animation);

				animationMap[animation->name] = animations.size() - 1;

				currentAnimationIndex = animations.size() - 1;

				for (unsigned int j = 0; j < scene->mAnimations[i]->mNumChannels; ++j) {
					AnimationNode_t animationNode = {};
					aiNodeAnim* node = scene->mAnimations[i]->mChannels[j];

					std::map<std::string, Bone_t*>::iterator it = boneMap.find(std::string(node->mNodeName.C_Str()));

					if (it != boneMap.end()) {
						animationNode.bone = it->second;
					} else {
						std::cout << "[ERROR] Animation using bones not contained in skeleton" << std::endl;
						continue;
					}

					animationNode.positionKeys.resize(node->mNumPositionKeys);
					animationNode.rotationKeys.resize(node->mNumRotationKeys);
					animationNode.scalingKeys.resize(node->mNumScalingKeys);

					for (unsigned int k = 0; k < node->mNumPositionKeys; ++k) {
						AnimationPositionKey_t key = {};
						key.time = static_cast<float>(node->mPositionKeys[k].mTime);
						key.position = glm::vec3(node->mPositionKeys[k].mValue.x,
							node->mPositionKeys[k].mValue.y,
							node->mPositionKeys[k].mValue.z);

						animationNode.positionKeys[k] = key;
					}

					for (unsigned int k = 0; k < node->mNumRotationKeys; ++k) {
						AnimationRotationKey_t key = {};
						key.time = static_cast<float>(node->mRotationKeys[k].mTime);
						key.rotation = glm::quat();
						key.rotation.x = node->mRotationKeys[k].mValue.x;
						key.rotation.y = node->mRotationKeys[k].mValue.y;
						key.rotation.z = node->mRotationKeys[k].mValue.z;
						key.rotation.w = node->mRotationKeys[k].mValue.w;

						animationNode.rotationKeys[k] = key;
					}

					for (unsigned int k = 0; k < node->mNumScalingKeys; ++k) {
						AnimationScalingKey_t key = {};
						key.time = static_cast<float>(node->mScalingKeys[k].mTime);
						key.scale = glm::vec3(node->mScalingKeys[k].mValue.x,
							node->mScalingKeys[k].mValue.y,
							node->mScalingKeys[k].mValue.z);

						animationNode.scalingKeys[k] = key;
					}

					animationNode.preAnimBehaviour = node->mPreState;
					animationNode.postAnimBehaviour = node->mPostState;

					animation->nodes.push_back(animationNode);

				}

				std::vector<float> animationBuffer;

				currentAnimation = animation;

				paused = false;

				animationBuffer.resize(static_cast<size_t>(animation->duration) * 3 * 256 * 4);

				size_t currentRow = 0;

				for (float t = 0.f, inc = 1.f / animation->ticksPerSecond,
					duration = animation->duration / animation->ticksPerSecond;
					t < duration;
					t += inc) {
					time = t;
					Update(0.f);

					for (size_t j = 0, size = boneData.size(); j < size; ++j) {
						glm::mat4 transform = boneData[j].transform;

						glm::vec4 translate = transform[3];
						transform[3] = glm::vec4(0.f, 0.f, 0.f, 1.f);

						glm::vec3 scale = glm::vec3(
							glm::length(glm::vec3(transform[0][0], transform[0][1], transform[0][2])),
							glm::length(glm::vec3(transform[1][0], transform[1][1], transform[1][2])),
							glm::length(glm::vec3(transform[2][0], transform[2][1], transform[2][2]))
						);

						transform[0][0] /= scale.x;
						transform[0][1] /= scale.x;
						transform[0][2] /= scale.x;
						transform[1][0] /= scale.y;
						transform[1][1] /= scale.y;
						transform[1][2] /= scale.y;
						transform[2][0] /= scale.z;
						transform[2][1] /= scale.z;
						transform[2][2] /= scale.z;

						glm::quat rotation = glm::quat_cast(transform);

						animationBuffer[(currentRow * 256 * 4) + j * 4] = translate.x;
						animationBuffer[(currentRow * 256 * 4) + j * 4 + 1] = translate.y;
						animationBuffer[(currentRow * 256 * 4) + j * 4 + 2] = translate.z;
						animationBuffer[(currentRow * 256 * 4) + j * 4 + 3] = translate.w;

						animationBuffer[((currentRow + 1) * 256 * 4) + j * 4] = scale.x;
						animationBuffer[((currentRow + 1) * 256 * 4) + j * 4 + 1] = scale.y;
						animationBuffer[((currentRow + 1) * 256 * 4) + j * 4 + 2] = scale.z;

						animationBuffer[((currentRow + 2) * 256 * 4) + j * 4] = rotation.x;
						animationBuffer[((currentRow + 2) * 256 * 4) + j * 4 + 1] = rotation.y;
						animationBuffer[((currentRow + 2) * 256 * 4) + j * 4 + 2] = rotation.z;
						animationBuffer[((currentRow + 2) * 256 * 4) + j * 4 + 3] = rotation.w;
					}

					currentRow += 3;

				}

#ifdef USING_VULKAN
				animation->texture = std::shared_ptr<VulkanTexture>(new VulkanTexture(256, static_cast<int>(animation->duration * 3.f)));
				animation->texture->CreateTextureWithData(reinterpret_cast<stbi_uc*>(animationBuffer.data()), false, TextureDataSize::S_INT, true);

				VkSamplerCreateInfo samplerInfo = {};
				samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
				samplerInfo.magFilter = VK_FILTER_NEAREST;
				samplerInfo.minFilter = VK_FILTER_NEAREST;
				samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
				samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
				samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
				samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
				samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
				samplerInfo.anisotropyEnable = VK_FALSE;
				samplerInfo.compareEnable = VK_FALSE;
				samplerInfo.unnormalizedCoordinates = VK_TRUE;

				std::static_pointer_cast<VulkanTexture, Texture>(animation->texture)->SetSampler(samplerInfo);
#endif
#ifdef USING_OPENGL
				animation->texture = std::shared_ptr<OpenGLTexture>(new OpenGLTexture(256, static_cast<int>(animation->duration * 3.f)));
				animation->texture->CreateTextureWithData(reinterpret_cast<stbi_uc*>(animationBuffer.data()), false, TextureDataSize::S_INT, true);
#endif
			}

		} else {
			animated = false;
		}
	}

	void Skeleton::LoadAnimationSet(const aiScene* scene, std::vector<std::string> names) {
		if (scene->HasAnimations()) {

			for (unsigned int i = 0; i < scene->mNumAnimations; ++i) {
				Animation_t* animation = new Animation_t;
				if (names.size() > i) {
					animation->name = names[i];
					if (animation->name == "")
						animation->name = std::string(scene->mAnimations[i]->mName.C_Str());
				} else {
					animation->name = std::string(scene->mAnimations[i]->mName.C_Str());
				}
				animation->duration = static_cast<float>(scene->mAnimations[i]->mDuration);
				animation->ticksPerSecond = static_cast<float>(scene->mAnimations[i]->mTicksPerSecond);

				animations.push_back(animation);

				animationMap[animation->name] = animations.size() - 1;

				currentAnimationIndex = animations.size() - 1;

				for (unsigned int j = 0; j < scene->mAnimations[i]->mNumChannels; ++j) {
					AnimationNode_t animationNode = {};
					aiNodeAnim* node = scene->mAnimations[i]->mChannels[j];

					std::map<std::string, Bone_t*>::iterator it = boneMap.find(std::string(node->mNodeName.C_Str()));

					if (it != boneMap.end()) {
						animationNode.bone = it->second;
					} else {
						std::cout << "[ERROR] Animation using bones not contained in skeleton" << std::endl;
						continue;
					}

					animationNode.positionKeys.resize(node->mNumPositionKeys);
					animationNode.rotationKeys.resize(node->mNumRotationKeys);
					animationNode.scalingKeys.resize(node->mNumScalingKeys);

					for (unsigned int k = 0; k < node->mNumPositionKeys; ++k) {
						AnimationPositionKey_t key = {};
						key.time = static_cast<float>(node->mPositionKeys[k].mTime);
						key.position = glm::vec3(node->mPositionKeys[k].mValue.x,
							node->mPositionKeys[k].mValue.y,
							node->mPositionKeys[k].mValue.z);

						animationNode.positionKeys[k] = key;
					}

					for (unsigned int k = 0; k < node->mNumRotationKeys; ++k) {
						AnimationRotationKey_t key = {};
						key.time = static_cast<float>(node->mRotationKeys[k].mTime);
						key.rotation = glm::quat();
						key.rotation.x = node->mRotationKeys[k].mValue.x;
						key.rotation.y = node->mRotationKeys[k].mValue.y;
						key.rotation.z = node->mRotationKeys[k].mValue.z;
						key.rotation.w = node->mRotationKeys[k].mValue.w;

						animationNode.rotationKeys[k] = key;
					}

					for (unsigned int k = 0; k < node->mNumScalingKeys; ++k) {
						AnimationScalingKey_t key = {};
						key.time = static_cast<float>(node->mScalingKeys[k].mTime);
						key.scale = glm::vec3(node->mScalingKeys[k].mValue.x,
							node->mScalingKeys[k].mValue.y,
							node->mScalingKeys[k].mValue.z);

						animationNode.scalingKeys[k] = key;
					}

					animationNode.preAnimBehaviour = node->mPreState;
					animationNode.postAnimBehaviour = node->mPostState;

					animation->nodes.push_back(animationNode);

				}

				std::vector<float> animationBuffer;

				currentAnimation = animation;

				paused = false;

				animationBuffer.resize(static_cast<size_t>(animation->duration) * 3 * 256 * 4);

				size_t currentRow = 0;

				for (float t = 0.f, inc = 1.f / animation->ticksPerSecond,
					duration = animation->duration / animation->ticksPerSecond;
					t < duration;
					t += inc) {
					time = t;
					Update(0.f);

					for (size_t i = 0, size = boneData.size(); i < size; ++i) {
						glm::mat4 transform = boneData[i].transform;

						glm::vec4 translate = transform[3];
						transform[3] = glm::vec4(0.f, 0.f, 0.f, 1.f);

						glm::vec3 scale = glm::vec3(
							glm::length(glm::vec3(transform[0][0], transform[0][1], transform[0][2])),
							glm::length(glm::vec3(transform[1][0], transform[1][1], transform[1][2])),
							glm::length(glm::vec3(transform[2][0], transform[2][1], transform[2][2]))
						);

						transform[0][0] /= scale.x;
						transform[0][1] /= scale.x;
						transform[0][2] /= scale.x;
						transform[1][0] /= scale.y;
						transform[1][1] /= scale.y;
						transform[1][2] /= scale.y;
						transform[2][0] /= scale.z;
						transform[2][1] /= scale.z;
						transform[2][2] /= scale.z;

						glm::quat rotation = glm::quat_cast(transform);

						animationBuffer[(currentRow * 256 * 4) + i * 4] = translate.x;
						animationBuffer[(currentRow * 256 * 4) + i * 4 + 1] = translate.y;
						animationBuffer[(currentRow * 256 * 4) + i * 4 + 2] = translate.z;
						animationBuffer[(currentRow * 256 * 4) + i * 4 + 3] = translate.w;

						animationBuffer[((currentRow + 1) * 256 * 4) + i * 4] = scale.x;
						animationBuffer[((currentRow + 1) * 256 * 4) + i * 4 + 1] = scale.y;
						animationBuffer[((currentRow + 1) * 256 * 4) + i * 4 + 2] = scale.z;

						animationBuffer[((currentRow + 2) * 256 * 4) + i * 4] = rotation.x;
						animationBuffer[((currentRow + 2) * 256 * 4) + i * 4 + 1] = rotation.y;
						animationBuffer[((currentRow + 2) * 256 * 4) + i * 4 + 2] = rotation.z;
						animationBuffer[((currentRow + 2) * 256 * 4) + i * 4 + 3] = rotation.w;
					}

					currentRow += 3;

				}

#ifdef USING_VULKAN
				animation->texture = std::shared_ptr<VulkanTexture>(new VulkanTexture(256, static_cast<int>(animation->duration * 3.f)));
				animation->texture->CreateTextureWithData(reinterpret_cast<stbi_uc*>(animationBuffer.data()), false, TextureDataSize::S_INT, true);

				VkSamplerCreateInfo samplerInfo = {};
				samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
				samplerInfo.magFilter = VK_FILTER_NEAREST;
				samplerInfo.minFilter = VK_FILTER_NEAREST;
				samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
				samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
				samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
				samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
				samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
				samplerInfo.anisotropyEnable = VK_FALSE;
				samplerInfo.compareEnable = VK_FALSE;
				samplerInfo.unnormalizedCoordinates = VK_TRUE;

				std::static_pointer_cast<VulkanTexture, Texture>(animation->texture)->SetSampler(samplerInfo);
#endif
#ifdef USING_OPENGL
				animation->texture = std::shared_ptr<OpenGLTexture>(new OpenGLTexture(256, static_cast<int>(animation->duration * 3.f)));
				animation->texture->CreateTextureWithData(reinterpret_cast<stbi_uc*>(animationBuffer.data()), false, TextureDataSize::S_INT, true);
#endif

			}

		} else {
			std::cout << "[WARNING] No animations detected in file" << std::endl;
		}
	}


	Skeleton::~Skeleton() {
		for each (std::pair<std::string, Bone_t*> bone in boneMap) {
			delete bone.second;
		}
		for (size_t i = 0, size = animations.size(); i < size; ++i) {
			animations[i]->texture.reset();
			delete animations[i];
		}
	}

	void Skeleton::GetAnimations(std::vector<std::string>& output) {
		for each (const std::pair<std::string, size_t>&animation in animationMap) {
			output.push_back(animation.first);
		}
	}

	void Skeleton::SetAnimation(std::string animation, bool resetTime) {
		std::map<std::string, size_t>::iterator it = animationMap.find(animation);

		if (it != animationMap.end() && it->second < animations.size()) {
			currentAnimation = animations[it->second];
		} else {
			currentAnimation = nullptr;
		}

		if (resetTime)
			time = 0.f;
	}

	void Skeleton::UpdateBoneTreeTransformData() {
		UpdateBoneTransformData(rootBone);
		UpdateBoneBuffers();
	}

	void Skeleton::UpdateBoneBuffers() {}

	glm::mat4 Skeleton::InterpolateScale(float time, AnimationNode_t node) {
		glm::vec3 scale;

		time *= currentAnimation->ticksPerSecond * speed;

		if (node.scalingKeys.size() == 1) {
			scale = node.scalingKeys[0].scale;
		} else {
			size_t frameIndex = 0;
			for (size_t i = 0, size = node.scalingKeys.size(); i < size - 1; ++i) {
				if (time < node.scalingKeys[i + 1].time) {
					frameIndex = i;
					break;
				}
			}

			AnimationScalingKey_t currFrame = node.scalingKeys[frameIndex];
			AnimationScalingKey_t nextFrame;
			if (frameIndex + 1 == node.scalingKeys.size()) {
				nextFrame = node.scalingKeys[frameIndex];
			} else {
				nextFrame = node.scalingKeys[frameIndex + 1];
			}

			float delta = (time - currFrame.time) / (nextFrame.time - currFrame.time);

			delta = fmaxf(0.f, fminf(delta, 1.f));

			scale = glm::mix(currFrame.scale, nextFrame.scale, delta);
		}
		return glm::scale(glm::mat4(), scale);
	}

	glm::mat4 Skeleton::InterpolateRotation(float time, AnimationNode_t node) {
		glm::quat rotation;

		time *= currentAnimation->ticksPerSecond * speed;

		if (node.rotationKeys.size() == 1) {
			rotation = node.rotationKeys[0].rotation;
		} else {
			size_t frameIndex = 0;
			for (size_t i = 0, size = node.rotationKeys.size(); i < size - 1; ++i) {
				if (time < node.rotationKeys[i + 1].time) {
					frameIndex = i;
					break;
				}
			}

			AnimationRotationKey_t currFrame = node.rotationKeys[frameIndex];
			AnimationRotationKey_t nextFrame;
			if (frameIndex + 1 == node.rotationKeys.size()) {
				nextFrame = node.rotationKeys[frameIndex];
			} else {
				nextFrame = node.rotationKeys[frameIndex + 1];
			}

			float delta = (time - currFrame.time) / (nextFrame.time - currFrame.time);

			delta = fmaxf(0.f, fminf(delta, 1.f));

			rotation = glm::lerp(currFrame.rotation, nextFrame.rotation, delta);

		}

		return glm::mat4_cast(rotation);
	}

	glm::mat4 Skeleton::InterpolatePosition(float time, AnimationNode_t node) {
		glm::vec3 position;

		time *= currentAnimation->ticksPerSecond * speed;

		if (node.positionKeys.size() == 1) {
			position = node.positionKeys[0].position;
		} else {
			size_t frameIndex = 0;
			for (size_t i = 0, size = node.positionKeys.size(); i < size - 1; ++i) {
				if (time < node.positionKeys[i + 1].time) {
					frameIndex = i;
					break;
				}
			}

			AnimationPositionKey_t currFrame = node.positionKeys[frameIndex];
			AnimationPositionKey_t nextFrame;
			if (frameIndex + 1 == node.positionKeys.size()) {
				nextFrame = node.positionKeys[frameIndex];
			} else {
				nextFrame = node.positionKeys[frameIndex + 1];
			}

			float delta = (time - currFrame.time) / (nextFrame.time - currFrame.time);

			delta = fmaxf(0.f, fminf(delta, 1.f));

			position = glm::mix(currFrame.position, nextFrame.position, delta);
		}
		return glm::translate(glm::mat4(), position);
	}

	float Skeleton::GetAnimationDuration(size_t animation) {
		if (animation < animations.size()) {
			return animations[animation]->duration / animations[animation]->ticksPerSecond;
		} else {
			return 0.f;
		}

	}

	void Skeleton::Update(float deltaTime) {
		if (paused)
			return;

		time += deltaTime;

		if (looping)
			time = fmodf(time, GetAnimationDuration(currentAnimationIndex));

		if (currentAnimation != nullptr) {

			for (size_t i = 0, size = currentAnimation->nodes.size(); i < size; ++i) {
				Bone_t* bone = currentAnimation->nodes[i].bone;

				bone->transform = InterpolatePosition(time, currentAnimation->nodes[i]) *
					InterpolateRotation(time, currentAnimation->nodes[i]) *
					InterpolateScale(time, currentAnimation->nodes[i]);
			}

		} else {
			for each (std::pair<std::string, Bone_t*> bone in boneMap) {
				bone.second->transform = bone.second->defaultTransform;
			}
		}

		UpdateBoneTreeTransformData();
	}

	bool Skeleton::HasAnimations() {
		return animated;
	}

	Texture* Skeleton::GetAnimationData(size_t animation) {
		if (animation < animations.size()) {
			return animations[animation]->texture.get();
		} else {
			return nullptr;
		}
	}

	size_t Skeleton::GetAnimationIndex(std::string animation) {
		std::map<std::string, size_t>::iterator it = animationMap.find(animation);

		if (it != animationMap.end() && it->second < animations.size()) {
			return it->second;
		} else {
			return -1;
		}
	}

	float Skeleton::GetAnimationTicksPerSecond(size_t animation) {
		if (animation < animations.size()) {
			return animations[animation]->ticksPerSecond;
		} else {
			return 0.f;
		}
	}

	std::map<std::string, Skeleton::Bone_t*> Skeleton::GetBoneMap() {
		return boneMap;
	}

	Skeleton::Bone_t* Skeleton::GetRootBone() {
		return rootBone;
	}

	void Skeleton::SetName(std::string name) {
		this->name = name;
	}

	std::string Skeleton::GetName() {
		return name;
	}

	void Skeleton::ReadBones(Bone_t* bone, aiNode* node) {
		bone->childBones.resize(static_cast<size_t>(node->mNumChildren));

		bone->name = std::string(node->mName.C_Str());

		bone->node = node;

		bone->boneDataIndex = static_cast<int>(boneMap.size());

		boneData[bone->boneDataIndex].transform = TransformToMat4(node->mTransformation);

		bone->transform = TransformToMat4(node->mTransformation);
		bone->defaultTransform = bone->transform;

		boneMap[bone->name] = bone;

		for (size_t i = 0, size = bone->childBones.size(); i < size; ++i) {
			bone->childBones[i] = new Bone_t;
			bone->childBones[i]->parent = bone;
			bone->childBones[i]->boneDataIndex = -1;
			ReadBones(bone->childBones[i], node->mChildren[i]);
		}
	}

	void Skeleton::DestroyBone(Bone_t* bone) {
		for (size_t i = 0, size = bone->childBones.size(); i < size; ++i) {
			DestroyBone(bone->childBones[i]);
		}
		delete bone;
	}

	void Skeleton::SetBoneTransform(std::string bone, glm::mat4 transform) {
		std::map<std::string, Bone_t*>::iterator it = boneMap.find(bone);
		if (it != boneMap.end()) {
			SetBoneTransform(it->second, transform);
		}
	}

	void Skeleton::SetBoneTransform(Bone_t* bone, glm::mat4 transform) {
		bone->transform = transform;
	}

	void Skeleton::UpdateBoneTransformData(Bone_t* bone) {
		if (bone->parent != nullptr) {
			boneData[bone->boneDataIndex].transform = boneData[bone->parent->boneDataIndex].transform * bone->transform;
		} else {
			boneData[bone->boneDataIndex].transform = bone->transform;
		}

		for (size_t i = 0, size = bone->childBones.size(); i < size; ++i) {
			UpdateBoneTransformData(bone->childBones[i]);
		}
	}

	glm::mat4 Skeleton::TransformToMat4(aiMatrix4x4 transform) {
		return glm::mat4(transform.a1, transform.b1, transform.c1, transform.d1,
			transform.a2, transform.b2, transform.c2, transform.d2,
			transform.a3, transform.b3, transform.c3, transform.d3,
			transform.a4, transform.b4, transform.c4, transform.d4);
	}

}