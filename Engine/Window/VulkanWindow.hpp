#pragma once

#include "Engine/Utility/Defines.hpp"
#ifdef USING_VULKAN
#include "Engine/Engine/Engine.hpp"
#define GLFW_INCLUDE_VULKAN
#include "Engine/Window/Window.hpp"

#include <vulkan/vulkan.h>

namespace Engine {
	class ENGINE_API VulkanWindow : public Window {
		friend std::shared_ptr<Window> Engine::GetWindow() noexcept;

		VulkanWindow() noexcept;
	public:
		VulkanWindow(const VulkanWindow& other) = delete;
		VulkanWindow(VulkanWindow&& other) noexcept = delete;
		virtual ~VulkanWindow() override = default;

		VulkanWindow& operator=(const VulkanWindow& other) = delete;
		VulkanWindow& operator=(VulkanWindow&& other) noexcept = delete;
	};
} //namespace Engine
#endif