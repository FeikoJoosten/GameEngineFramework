#include "Engine//Platform/Vulkan/ParticleSystem/VulkanEmitter.hpp"

#ifdef USING_VULKAN

#include "Engine/Platform/Vulkan/Renderer/VulkanRenderer.hpp"

namespace Engine {

	VulkanEmitter::VulkanEmitter() : bufferQueueFamilyTransitionsNeseccary(false), renderQueueFamily(0),
	                                computeQueueFamily(0)
	{
	}


	VulkanEmitter::~VulkanEmitter()
	{
	}

	void VulkanEmitter::Compile()
	{
		Emitter::Compile();

		if (device->GetPhysicalDevice()->GetQueueFamilies().compute == device->GetPhysicalDevice()->GetQueueFamilies().graphics) {
			bufferQueueFamilyTransitionsNeseccary = false;
		}
		else {
			bufferQueueFamilyTransitionsNeseccary = true;
			computeQueueFamily = device->GetPhysicalDevice()->GetQueueFamilies().compute;
			renderQueueFamily = device->GetPhysicalDevice()->GetQueueFamilies().graphics;
		}

		pipeline = std::unique_ptr<VulkanComputePipeline>(new VulkanComputePipeline(device));

		pipeline->SetComputeShader("ParticleUpdate.comp.spv");


	}

	VulkanLogicalDevice* VulkanEmitter::device = nullptr;
	VulkanRenderer* VulkanEmitter::renderer = nullptr;
	VmaAllocator VulkanEmitter::allocator = VK_NULL_HANDLE;
	VkCommandPool VulkanEmitter::commandPool = VK_NULL_HANDLE;

	void VulkanEmitter::InitParticleEmitter(VulkanRenderer* renderer, VulkanLogicalDevice * device, VmaAllocator allocator, VkCommandPool commandPool)
	{
		VulkanEmitter::device = device;
		VulkanEmitter::renderer = renderer;
		VulkanEmitter::allocator = allocator;
		VulkanEmitter::commandPool = commandPool;
	}

}

#endif