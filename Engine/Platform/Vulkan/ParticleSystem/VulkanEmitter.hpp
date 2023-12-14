#pragma once

#include "Engine/Utility/Defines.hpp"

#ifdef USING_VULKAN
#include "Engine/Api/Api.hpp"
#include "Engine/Particle System/Emitter.hpp"
#include "Engine/Platform/Vulkan/Renderer/VulkanBuffer.hpp"
#include "Engine/Platform/Vulkan/Renderer/VulkanComputePipeline.hpp"
#include "Engine/Platform/Vulkan/Renderer/VulkanLogicalDevice.hpp"

#include <memory>

namespace Engine {

	class VulkanRenderer;

	class ENGINE_LOCAL VulkanEmitter : public Emitter
	{
	public:
		VulkanEmitter();
		~VulkanEmitter();

		void Compile() override;

	protected:
		std::unique_ptr<VulkanBuffer> particleBuffer;
		std::unique_ptr<VulkanBuffer> stagingBuffer;

		bool bufferQueueFamilyTransitionsNeseccary;

		int renderQueueFamily;
		int computeQueueFamily;

		std::unique_ptr<VulkanComputePipeline> pipeline;

		static VulkanLogicalDevice* device;
		static VulkanRenderer* renderer;
		static VmaAllocator allocator;
		static VkCommandPool commandPool;

		static void InitParticleEmitter(VulkanRenderer* renderer, VulkanLogicalDevice* device, VmaAllocator allocator, VkCommandPool commandPool);

	};

}

#endif