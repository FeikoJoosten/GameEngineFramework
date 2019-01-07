#include "Engine/Model/Model.hpp"
#include "Engine/Mesh/Mesh.hpp" 
#include "Engine/engine.hpp"
#include <ThirdParty/EASTL-master/include/EASTL/string.h>

namespace Engine
{
	Model::Model(const aiScene* scene, eastl::string name)
	{
		if (name == "")
			name = name + std::to_string(Engine::GetEngine().lock()->GetResourceManager().lock()->loadedModels_.size()).c_str();
		this->name = name;
		this->speed = 1.f;
		this->paused = true;
		this->time = 0.f;
		this->looping = true;
		this->currentAnimation = -1;
	}

	eastl::vector<eastl::shared_ptr<Mesh>>& Model::GetModelMeshes()
	{
		return meshes;
	}

	eastl::string Model::GetName()
	{
		return name;
	}

	void Model::SetSkeleton(eastl::shared_ptr<Skeleton> skeleton)
	{
		this->skeleton = skeleton;
	}

	eastl::shared_ptr<Skeleton> Model::GetSkeleton() const
	{
		return skeleton;
	}

	eastl::vector<eastl::string> Model::GetAnimations()
	{
		if (skeleton != nullptr)
			return skeleton->GetAnimations();
		else
			return eastl::vector<eastl::string>();
	}

	void Model::SetAnimation(eastl::string animation, bool resetTime)
	{
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

	void Model::ResetAnimation()
	{
		currentAnimation = -1;
		currentAnimationName = "";
	}

	void Model::SetAnimationTime(float time)
	{
		this->time = time;
	}

	void Model::Update(float deltaTime)
	{

	}

	void Model::UpdateAnimation(float deltaTime)
	{
		if (!paused&&skeleton != nullptr && currentAnimation != -1) {
			this->time += deltaTime * speed;
			if (this->time > skeleton->GetAnimationDuration(currentAnimation)) {
				if (looping)
					this->time = fmodf(this->time, skeleton->GetAnimationDuration(currentAnimation));
				else
					this->time = skeleton->GetAnimationDuration(currentAnimation);
			}
		}
	}

	float Model::GetAnimationTime() const
	{
		return this->time;
	}

	float Model::GetAnimationDuration() const
	{
		if (skeleton != nullptr && currentAnimation != -1) {
			return skeleton->GetAnimationDuration(currentAnimation);
		}
		return 0.0f;
	}

	void Model::SetPaused(bool paused)
	{
		this->paused = paused;
	}

	bool Model::IsPaused() const
	{
		return this->paused;
	}

	bool Model::HasAnimations() const
	{
		if (skeleton == nullptr)
			return false;
		return skeleton->HasAnimations();
	}

	void Model::SetLooping(bool looping)
	{
		this->looping = looping;
	}

	bool Model::IsLooping() const
	{
		return this->looping;
	}

	void Model::SetSpeed(float speed)
	{
		this->speed = speed;
	}

	float Model::GetSpeed() const
	{
		return this->speed;
	}

	eastl::string Model::GetCurrentAnimationName() const
	{
		return currentAnimationName;
	}

	size_t Model::GetCurrentAnimationIndex() const
	{
		return currentAnimation;
	}

	eastl::shared_ptr<Material> Model::GetMeshMaterial(eastl::shared_ptr<Mesh> mesh)
	{
		if (meshMaterialMap.count(mesh) > 0)
			return meshMaterialMap[mesh];
		else
			return nullptr;
	}

	void Model::SetMeshMaterial(eastl::shared_ptr<Mesh> mesh, eastl::shared_ptr<Material> material)
	{
		meshMaterialMap[mesh] = material;
	}

	bool Model::operator==(Model& other)
	{
		if (name != other.name) return false;

		size_t thisMeshCount = meshes.size();
		size_t otherMeshCount = other.meshes.size();

		if (thisMeshCount != otherMeshCount) return false;

		for (size_t i = 0; i < thisMeshCount; ++i)
		{
			if (meshes[i] != other.meshes[i]) return false;
		}

		for each (eastl::pair<eastl::shared_ptr<Mesh>, eastl::shared_ptr<Material>> mat in meshMaterialMap)
		{
			if (other.meshMaterialMap[mat.first] != mat.second) return false;
		}

		if (other.skeleton != skeleton) return false;

		return true;
	}

	bool Model::operator!=(Model& other)
	{
		if (name != other.name) return true;

		size_t thisMeshCount = meshes.size();
		size_t otherMeshCount = other.meshes.size();

		if (thisMeshCount != otherMeshCount) return true;

		for (size_t i = 0; i < thisMeshCount; ++i)
		{
			if (meshes[i] != other.meshes[i]) return true;
		}

		for each (eastl::pair<eastl::shared_ptr<Mesh>, eastl::shared_ptr<Material>> mat in meshMaterialMap)
		{
			if (other.meshMaterialMap[mat.first] != mat.second) return true;
		}

		if (other.skeleton != skeleton) return true;

		return false;
	}

	void Model::AddMesh(eastl::shared_ptr<Mesh> meshToAdd)
	{
		meshes.push_back(meshToAdd);
	}
} // namespace Engine