#include "Engine/Renderer/Vulkan/VulkanInstance.hpp"
#include <EASTL-master/include/EASTL/vector.h>

#ifdef USING_VULKAN

#if _WIN64
#include <ThirdParty/glfw-3.2.1/x64/include/GLFW/glfw3.h>
#include <ThirdParty/glfw-3.2.1/x64/include/GLFW/glfw3native.h>
#else
#include <ThirdParty/glfw-3.2.1/x86/include/GLFW/glfw3.h>
#include <ThirdParty/glfw-3.2.1/x86/include/GLFW/glfw3native.h>
#endif

#include <iostream>
#include <sstream>
#include <ThirdParty/EASTL-master/include/EASTL/string.h>


static VkResult CreateDebugReportCallbackEXT(VkInstance instance, const VkDebugReportCallbackCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugReportCallbackEXT* pCallback) {
	auto func = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");
	if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pCallback);
	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}
static void DestroyDebugReportCallbackEXT(VkInstance instance, VkDebugReportCallbackEXT callback, const VkAllocationCallbacks* pAllocator) {
	auto func = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");
	if (func != nullptr) {
		func(instance, callback, pAllocator);
	}
}

static VKAPI_ATTR VkBool32 VKAPI_CALL defaultDebugCallback(
	VkDebugReportFlagsEXT flags,
	VkDebugReportObjectTypeEXT objType,
	uint64_t obj,
	size_t location,
	int32_t code,
	const char* layerPrefix,
	const char* msg,
	void* userData) {
	eastl::string stream;
	switch (flags) {
	case VK_DEBUG_REPORT_INFORMATION_BIT_EXT:
		stream += "[INFO] ";
		break;
	case VK_DEBUG_REPORT_WARNING_BIT_EXT:
		stream += "[WARNING] ";
		break;
	case VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT:
		stream += "[PERFORMANCE] ";
		break;
	case VK_DEBUG_REPORT_ERROR_BIT_EXT:
		stream += "[ERROR] ";
		break;
	case VK_DEBUG_REPORT_DEBUG_BIT_EXT:
		stream += "[DEBUG] ";
		break;
	}
	stream += "[LAYER:";
	stream += layerPrefix;
	stream += "] ";
	stream += "[CODE:";
	stream += code;
	stream += "] ";
	stream += " [OBJ:";
	stream += obj;
	stream += "] [LOCATION:";
	stream += location;
	stream += "] ";
	stream += msg;
	//hex::CLogger::CLog(stream.str().c_str(), hex::ELogChannel::LogChannel_Graphics, importance);
	std::cout << stream.c_str() << std::endl;
	return VK_FALSE;
}

namespace Engine {

	VulkanInstance::VulkanInstance(bool debugEnabled, DebugLevel debugLevel, VulkanAppData engineData, VulkanAppData appData, PFN_vkDebugReportCallbackEXT debugCallback)
	{
		VkApplicationInfo appInfo = {};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pEngineName = engineData.appName;
		appInfo.engineVersion = VK_MAKE_VERSION(engineData.versionMajor, engineData.versionMinor, engineData.versionPatch);
		appInfo.pApplicationName = appData.appName;
		appInfo.applicationVersion = VK_MAKE_VERSION(appData.versionMajor, appData.versionMinor, appData.versionPatch);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		VkInstanceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

		unsigned int glfwExtensionsCount = 0;
		const char** glfwExtensionsConst;
		char** glfwExtensions;
		glfwExtensionsConst = glfwGetRequiredInstanceExtensions(&glfwExtensionsCount);
		glfwExtensions = (char**)glfwExtensionsConst;

		eastl::vector<const char*> extensions;
		for (unsigned int i = 0; i < glfwExtensionsCount; i++) {
			extensions.push_back(glfwExtensions[i]);
		}
		if (debugEnabled) {
			extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
		}

		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();

		eastl::vector<const char*> layers;
		if (debugEnabled) {
			layers.push_back("VK_LAYER_LUNARG_standard_validation");
			//layers.push_back("VK_LAYER_LUNARG_monitor");
		}

		createInfo.enabledLayerCount = static_cast<uint32_t>(layers.size());
		createInfo.ppEnabledLayerNames = layers.data();

		VkResult res = vkCreateInstance(&createInfo, nullptr, &instance);
		if (res != VK_SUCCESS) {
			eastl::string s = eastl::string("[ERROR] [CODE:") + std::to_string(res).c_str() + "] Creating vulkan instance failed";
			std::cout << s.c_str() << std::endl;
		}

		this->debugEnabled = (debugLevel == DebugLevel::LEVEL_NONE) ? false : debugEnabled;

		if (this->debugEnabled) {
			VkDebugReportCallbackCreateInfoEXT debugCreateInfo = {};
			debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
			debugCreateInfo.flags = static_cast<uint32_t>(debugLevel);
			debugCreateInfo.pfnCallback = (debugCallback == nullptr) ? defaultDebugCallback : debugCallback;

			VkResult res = CreateDebugReportCallbackEXT(instance, &debugCreateInfo, nullptr, &debugReportCallback);
		}
	}

	VulkanInstance::~VulkanInstance()
	{
		if (debugEnabled)
			DestroyDebugReportCallbackEXT(instance, debugReportCallback, nullptr);
		vkDestroyInstance(instance, nullptr);
	}

	VkInstance VulkanInstance::GetInstance() const noexcept
	{
		return instance;
	}
}

#endif // USING_VULKAN