#include "Engine/Model/Model.hpp"
#include "Engine/Mesh/Mesh.hpp" 
#include "Engine/Engine/Engine.hpp"
#include "Engine/Resources/ResourceManager.hpp"

namespace Engine {
	Model::Model(const aiScene* scene, std::string name) {
		if (name.empty())
			name = name + std::to_string(ResourceManager::Get()->loadedModels_.size());
		this->name = name;
		this->speed = 1.f;
		this->paused = false;
		this->time = 0.f;
		this->looping = true;
		this->currentAnimation = -1;
	}

	std::vector<std::shared_ptr<Mesh>>& Model::GetModelMeshes() {
		return meshes;
	}

	std::string Model::GetName() {
		return name;
	}

	void Model::SetSkeleton(std::shared_ptr<Skeleton> newSkeleton) {
		skeleton = newSkeleton;
	}

	std::shared_ptr<Skeleton> Model::GetSkeleton() const {
		return skeleton;
	}

	void Model::GetAnimations(std::vector<std::string>& output) const {
		if (skeleton != nullptr)
			skeleton->GetAnimations(output);
	}

	void Model::SetAnimation(std::string animation, bool resetTime) {
		if (skeleton != nullptr) {
			size_t index = skeleton->GetAnimationIndex(animation);
			if (index != -1) {
				currentAnimation = index;
				currentAnimationName = animation;

				if (resetTime) {
					this->time = 0.f;
				}
			}

		}
	}

	void Model::ResetAnimation() {
		currentAnimation = -1;
		currentAnimationName = "";
	}

	void Model::SetAnimationTime(float time) {
		this->time = time;
	}

	void Model::Update(const float deltaTime) {
		UpdateAnimation(deltaTime);
	}

	void Model::UpdateAnimation(const float deltaTime) {
		if (!paused && skeleton != nullptr && currentAnimation != -1) {
			//skeleton->Update(deltaTime * speed);
			if (time > skeleton->GetAnimationDuration(currentAnimation)) {
				if (looping)
					time = fmodf(time, skeleton->GetAnimationDuration(currentAnimation));
				else
					time = skeleton->GetAnimationDuration(currentAnimation);
			}
		}
	}

	float Model::GetAnimationTime() const {
		return time;
	}

	float Model::GetAnimationDuration() const {
		if (skeleton != nullptr && currentAnimation != -1) {
			return skeleton->GetAnimationDuration(currentAnimation);
		}
		return 0.0f;
	}

	void Model::SetPaused(bool paused) {
		this->paused = paused;
	}

	bool Model::IsPaused() const {
		return this->paused;
	}

	bool Model::HasAnimations() const {
		if (skeleton == nullptr)
			return false;
		return skeleton->HasAnimations();
	}

	void Model::SetLooping(bool looping) {
		this->looping = looping;
	}

	bool Model::IsLooping() const {
		return this->looping;
	}

	void Model::SetSpeed(float speed) {
		this->speed = speed;
	}

	float Model::GetSpeed() const {
		return this->speed;
	}

	std::string Model::GetCurrentAnimationName() const {
		return currentAnimationName;
	}

	size_t Model::GetCurrentAnimationIndex() const {
		return currentAnimation;
	}

	std::shared_ptr<Material> Model::GetMeshMaterial(std::shared_ptr<Mesh> mesh) {
		if (meshMaterialMap.count(mesh) > 0)
			return meshMaterialMap[mesh];
		else
			return nullptr;
	}

	void Model::SetMeshMaterial(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material) {
		meshMaterialMap[mesh] = material;
	}

	bool Model::operator==(Model& other) {
		if (name != other.name) return false;

		size_t thisMeshCount = meshes.size();
		size_t otherMeshCount = other.meshes.size();

		if (thisMeshCount != otherMeshCount) return false;

		for (size_t i = 0; i < thisMeshCount; ++i) {
			if (meshes[i] != other.meshes[i]) return false;
		}

		for each (std::pair<std::shared_ptr<Mesh>, std::shared_ptr<Material>> mat in meshMaterialMap) {
			if (other.meshMaterialMap[mat.first] != mat.second) return false;
		}

		if (other.skeleton != skeleton) return false;

		return true;
	}

	bool Model::operator!=(Model& other) {
		if (name != other.name) return true;

		size_t thisMeshCount = meshes.size();
		size_t otherMeshCount = other.meshes.size();

		if (thisMeshCount != otherMeshCount) return true;

		for (size_t i = 0; i < thisMeshCount; ++i) {
			if (meshes[i] != other.meshes[i]) return true;
		}

		for each (std::pair<std::shared_ptr<Mesh>, std::shared_ptr<Material>> mat in meshMaterialMap) {
			if (other.meshMaterialMap[mat.first] != mat.second) return true;
		}

		if (other.skeleton != skeleton) return true;

		return false;
	}

	void Model::AddMesh(std::shared_ptr<Mesh> meshToAdd) {
		meshes.push_back(meshToAdd);
	}
} // namespace Engine