#pragma once

#include "Engine/Utility/Defines.hpp"

#ifdef USING_VULKAN

#include "Engine/Particle System/Emitter.hpp"

#include "Engine/Renderer/Vulkan/VulkanBuffer.hpp"
#include "Engine/Renderer/Vulkan/VulkanComputePipeline.hpp"
#include "Engine/Renderer/Vulkan/VulkanLogicalDevice.hpp"

#include <ThirdParty/EASTL-master/include/EASTL/unique_ptr.h>

namespace Engine {

	class VulkanRenderer;

	class VulkanEmitter : public Emitter
	{
	public:
		VulkanEmitter();
		~VulkanEmitter();

		void Compile() override;

	protected:
		eastl::unique_ptr<VulkanBuffer> particleBuffer;
		eastl::unique_ptr<VulkanBuffer> stagingBuffer;

		bool bufferQueueFamilyTransitionsNeseccary;

		int renderQueueFamily;
		int computeQueueFamily;

		eastl::unique_ptr<VulkanComputePipeline> pipeline;

		static VulkanLogicalDevice* device;
		static VulkanRenderer* renderer;
		static VmaAllocator allocator;
		static VkCommandPool commandPool;

		static void InitParticleEmitter(VulkanRenderer* renderer, VulkanLogicalDevice* device, VmaAllocator allocator, VkCommandPool commandPool);

	};

}

#endif