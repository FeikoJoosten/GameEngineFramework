#include "Engine/Material/VulkanMaterial.hpp"
#ifdef USING_VULKAN
#include "Engine/Renderer/VulkanRenderer.hpp"

namespace Engine {

	VkDescriptorSetLayout VulkanMaterial::CreateMaterialDescriptorSetLayout(VulkanLogicalDevice * device)
	{
		std::array<VkDescriptorSetLayoutBinding, 4> descriptorSetBindings;

		descriptorSetBindings[0] = {};
		descriptorSetBindings[0].binding = 0;
		descriptorSetBindings[0].descriptorCount = 1;
		descriptorSetBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorSetBindings[0].pImmutableSamplers = nullptr;
		descriptorSetBindings[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		descriptorSetBindings[1] = {};
		descriptorSetBindings[1].binding = 1;
		descriptorSetBindings[1].descriptorCount = 1;
		descriptorSetBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorSetBindings[1].pImmutableSamplers = nullptr;
		descriptorSetBindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		descriptorSetBindings[2] = {};
		descriptorSetBindings[2].binding = 2;
		descriptorSetBindings[2].descriptorCount = 1;
		descriptorSetBindings[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorSetBindings[2].pImmutableSamplers = nullptr;
		descriptorSetBindings[2].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		descriptorSetBindings[3] = {};
		descriptorSetBindings[3].binding = 3;
		descriptorSetBindings[3].descriptorCount = 1;
		descriptorSetBindings[3].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorSetBindings[3].pImmutableSamplers = nullptr;
		descriptorSetBindings[3].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		VkDescriptorSetLayoutCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		info.bindingCount = static_cast<uint32_t>(descriptorSetBindings.size());
		info.pBindings = descriptorSetBindings.data();
		info.flags = 0;

		VkDescriptorSetLayout layout;
		vkCreateDescriptorSetLayout(device->GetDevice(), &info, nullptr, &layout);

		return layout;
	}

	VulkanMaterial::VulkanMaterial(const aiScene* scene, uint32_t materialIndex, eastl::string modelName) :
		Material(scene, materialIndex, modelName)
	{
		materialDataBuffer_ = eastl::unique_ptr<VulkanBuffer>(new VulkanBuffer(device_, allocator_,
			static_cast<uint32_t>(sizeof(MaterialData_t)), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, true, commandPool_));

		UpdateMaterialData();
	}
	
	VulkanMaterial::~VulkanMaterial()
	{
		materialDataBuffer_.reset();
	}

	VkDescriptorSet VulkanMaterial::CreateMaterialDescriptorSet(size_t threadID, size_t pipelineID, size_t set, VkDescriptorSetLayout layout)
	{
		if (materialDescriptorSets_.size() <= pipelineID) {
			materialDescriptorSets_.resize(pipelineID + 1);
		}
		if (materialDescriptorSets_[pipelineID].size() <= set) {
			materialDescriptorSets_[pipelineID].resize(set + 1);
		}
		if (materialDescriptorSets_[pipelineID][set].size() <= threadID) {
			materialDescriptorSets_[pipelineID][set].resize(threadID + 1, VK_NULL_HANDLE);
		}


		VkDescriptorSetLayout layouts[] = { layout };
		renderer_->GetDescriptorPool(threadID)->AllocateDescriptorSet(1, layouts, &materialDescriptorSets_[pipelineID][set][threadID]);
		eastl::shared_ptr<VulkanTexture> missing = eastl::static_pointer_cast<VulkanTexture, Texture>(missingTexture);
		if (diffuseTexture != nullptr) {
			eastl::shared_ptr<VulkanTexture> diffuse = eastl::static_pointer_cast<VulkanTexture, Texture>(diffuseTexture);
			renderer_->GetDescriptorPool(threadID)->DescriptorSetBindToImage(materialDescriptorSets_[pipelineID][set][threadID], VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
				diffuse->GetImageView(),
				diffuse->GetSampler(),
				0, 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1);
		}
		else
			renderer_->GetDescriptorPool(threadID)->DescriptorSetBindToImage(materialDescriptorSets_[pipelineID][set][threadID], VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
				missing->GetImageView(),
				missing->GetSampler(),
				0, 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1);
		
		if (specularTexture != nullptr) {
			eastl::shared_ptr<VulkanTexture> specular = eastl::static_pointer_cast<VulkanTexture, Texture>(specularTexture);
			renderer_->GetDescriptorPool(threadID)->DescriptorSetBindToImage(materialDescriptorSets_[pipelineID][set][threadID], VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
				specular->GetImageView(),
				specular->GetSampler(),
				1, 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1);
		}
		else
			renderer_->GetDescriptorPool(threadID)->DescriptorSetBindToImage(materialDescriptorSets_[pipelineID][set][threadID], VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
				missing->GetImageView(),
				missing->GetSampler(),
				1, 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1);

		if (bumpMapTexture != nullptr) {
			eastl::shared_ptr<VulkanTexture> bumpMap = eastl::static_pointer_cast<VulkanTexture, Texture>(bumpMapTexture);
			renderer_->GetDescriptorPool(threadID)->DescriptorSetBindToImage(materialDescriptorSets_[pipelineID][set][threadID], VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
				bumpMap->GetImageView(),
				bumpMap->GetSampler(),
				2, 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1);
		}
		else
			renderer_->GetDescriptorPool(threadID)->DescriptorSetBindToImage(materialDescriptorSets_[pipelineID][set][threadID], VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
				missing->GetImageView(),
				missing->GetSampler(),
				2, 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1);

		renderer_->GetDescriptorPool(threadID)->DescriptorSetBindToBuffer(materialDescriptorSets_[pipelineID][set][threadID], materialDataBuffer_->GetBuffer(),
			0, static_cast<VkDeviceSize>(sizeof(MaterialData_t)), 3, 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1);

		return materialDescriptorSets_[pipelineID][set][threadID];
	}

	VkDescriptorSet VulkanMaterial::GetMaterialDescriptorSet(size_t threadID, size_t pipelineID, size_t set)
	{
		if (pipelineID >= materialDescriptorSets_.size())
			return VK_NULL_HANDLE;
		if (set >= materialDescriptorSets_[pipelineID].size())
			return VK_NULL_HANDLE;
		if (threadID >= materialDescriptorSets_[pipelineID][set].size())
			return VK_NULL_HANDLE;
		return materialDescriptorSets_[pipelineID][set][threadID];
	}

	void VulkanMaterial::SetDiffuseTexture(eastl::shared_ptr<Texture> diffuseTexture)
	{
		Material::SetDiffuseTexture(diffuseTexture);

		for (size_t i = 0, size = materialDescriptorSets_.size(); i < size; ++i) {
			for (size_t j = 0, size2 = materialDescriptorSets_[i].size(); j < size2; ++j) {
				for (size_t k = 0, size3 = materialDescriptorSets_[i][j].size(); k < size3; ++k) {
					if (materialDescriptorSets_[i][j][k] != VK_NULL_HANDLE) {
						if (diffuseTexture != nullptr) {
							eastl::shared_ptr<VulkanTexture> diffuse = eastl::static_pointer_cast<VulkanTexture, Texture>(diffuseTexture);
							descriptorPool_->DescriptorSetBindToImage(materialDescriptorSets_[i][j][k], VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
								diffuse->GetImageView(),
								diffuse->GetSampler(),
								0, 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1);
						}
						else {
							eastl::shared_ptr<VulkanTexture> missing = eastl::static_pointer_cast<VulkanTexture, Texture>(missingTexture);
							descriptorPool_->DescriptorSetBindToImage(materialDescriptorSets_[i][j][k], VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
								missing->GetImageView(),
								missing->GetSampler(),
								0, 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1);
						}
					}
				}
			}
		}
	}

	void VulkanMaterial::SetBumpMapTexture(eastl::shared_ptr<Texture> bumpMapTexture)
	{
		Material::SetBumpMapTexture(bumpMapTexture);

		for (size_t i = 0, size = materialDescriptorSets_.size(); i < size; ++i) {
			for (size_t j = 0, size2 = materialDescriptorSets_[i].size(); j < size2; ++j) {
				for (size_t k = 0, size3 = materialDescriptorSets_[i][j].size(); k < size3; ++k) {
					if (materialDescriptorSets_[i][j][k] != VK_NULL_HANDLE) {
						if (bumpMapTexture != nullptr) {
							eastl::shared_ptr<VulkanTexture> bumpMap = eastl::static_pointer_cast<VulkanTexture, Texture>(bumpMapTexture);
							descriptorPool_->DescriptorSetBindToImage(materialDescriptorSets_[i][j][k], VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
								bumpMap->GetImageView(),
								bumpMap->GetSampler(),
								2, 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1);
						}
						else {
							eastl::shared_ptr<VulkanTexture> missing = eastl::static_pointer_cast<VulkanTexture, Texture>(missingTexture);
							descriptorPool_->DescriptorSetBindToImage(materialDescriptorSets_[i][j][k], VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
								missing->GetImageView(),
								missing->GetSampler(),
								2, 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1);
						}
					}
				}
			}
		}
	}

	void VulkanMaterial::SetSpecularTexture(eastl::shared_ptr<Texture> specularTexture)
	{
		Material::SetSpecularTexture(specularTexture);

		for (size_t i = 0, size = materialDescriptorSets_.size(); i < size; ++i) {
			for (size_t j = 0, size2 = materialDescriptorSets_[i].size(); j < size2; ++j) {
				for (size_t k = 0, size3 = materialDescriptorSets_[i][j].size(); k < size3; ++k) {
					if (materialDescriptorSets_[i][j][k] != VK_NULL_HANDLE) {
						if (diffuseTexture != nullptr) {
							eastl::shared_ptr<VulkanTexture> specular = eastl::static_pointer_cast<VulkanTexture, Texture>(specularTexture);
							descriptorPool_->DescriptorSetBindToImage(materialDescriptorSets_[i][j][k], VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
								specular->GetImageView(),
								specular->GetSampler(),
								1, 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1);
						}
						else {
							eastl::shared_ptr<VulkanTexture> missing = eastl::static_pointer_cast<VulkanTexture, Texture>(missingTexture);
							descriptorPool_->DescriptorSetBindToImage(materialDescriptorSets_[i][j][k], VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
								missing->GetImageView(),
								missing->GetSampler(),
								1, 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1);
						}
					}
				}
			}
		}
	}

	void VulkanMaterial::UpdateMaterialData()
	{
		if (materialDataBuffer_ != nullptr)
			materialDataBuffer_->UpdateBuffer(&materialData_, 0, static_cast<uint32_t>(sizeof(MaterialData_t)));
	}

	VulkanRenderer* VulkanMaterial::renderer_ = nullptr;
	VulkanLogicalDevice* VulkanMaterial::device_ = nullptr;
	VulkanDescriptorPool* VulkanMaterial::descriptorPool_ = nullptr;
	VkCommandPool VulkanMaterial::commandPool_ = VK_NULL_HANDLE;
	VmaAllocator VulkanMaterial::allocator_ = VK_NULL_HANDLE;

	void VulkanMaterial::InitMaterialData(VulkanRenderer * renderer, VulkanLogicalDevice * device, VulkanDescriptorPool * descriptorPool, VmaAllocator allocator)
	{
		VulkanMaterial::renderer_ = renderer;
		VulkanMaterial::device_ = device;
		VulkanMaterial::descriptorPool_ = descriptorPool;
		VulkanMaterial::commandPool_ = renderer->GetGraphicsCommandPool();
		VulkanMaterial::allocator_ = allocator;
	}

} // namespace Engine
#endif // USING_VULKAN