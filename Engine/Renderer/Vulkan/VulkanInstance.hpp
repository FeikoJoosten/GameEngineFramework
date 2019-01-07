#pragma once
#include "Engine/Utility/Defines.hpp"
#ifdef USING_VULKAN
#include <ThirdParty/Vulkan/Include/vulkan/vulkan.h>

#include "Engine/api.hpp"

namespace Engine {

	class ENGINE_API VulkanAppData {
	public:
		char* appName = "";
		int versionMajor = 0;
		int versionMinor = 0;
		int versionPatch = 1;
	};

	enum ENGINE_API DebugLevel {
		LEVEL_NONE					=	0,
		LEVEL_ERROR					=	VK_DEBUG_REPORT_ERROR_BIT_EXT,
		LEVEL_WARNINGS				=	VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT,
		LEVEL_PERFORMANCE_WARNING		=	VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT,
		LEVEL_INFORMATION				=	VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT |
									VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT | VK_DEBUG_REPORT_INFORMATION_BIT_EXT,
		LEVEL_DEBUG					=	VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT |
									VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT | VK_DEBUG_REPORT_INFORMATION_BIT_EXT |
									VK_DEBUG_REPORT_DEBUG_BIT_EXT
	};

	class ENGINE_LOCAL VulkanInstance
	{
	public:
		VulkanInstance(bool debugEnabled, DebugLevel debugLevel, VulkanAppData engineData = {}, VulkanAppData appData = {}, PFN_vkDebugReportCallbackEXT debugCallback = nullptr);
		~VulkanInstance();

		VkInstance GetInstance() const noexcept;

	protected:
		VkInstance instance;
		VkDebugReportCallbackEXT debugReportCallback;
		bool debugEnabled;
	};

}

#endif // USING_VULKAN