// ImGui GLFW binding with Vulkan + shaders
// FIXME: Changes of ImTextureID aren't supported by this binding! Please, someone add it!

// You can copy and use unmodified imgui_impl_* files in your project. See main.cpp for an example of using this.
// If you use this binding you'll need to call 5 functions: ImGui_ImplXXXX_Init(), ImGui_ImplXXX_CreateFontsTexture(), ImGui_ImplXXXX_NewFrame(), ImGui_ImplXXXX_Render() and ImGui_ImplXXXX_Shutdown().
// If you are new to ImGui, see examples/README.txt and documentation at the top of imgui.cpp.
// https://github.com/ocornut/imgui

#pragma once
struct GLFWwindow;

#ifdef USING_VULKAN

#define IMGUI_VK_QUEUED_FRAMES 2

struct ImGui_ImplGlfwVulkan_Init_Data
{
	VkAllocationCallbacks* allocator;
	VkPhysicalDevice       gpu;
	VkDevice               device;
	VkRenderPass           render_pass;
	VkQueue				   graphics_queue;
	uint32_t               sub_pass;
	VkPipelineCache        pipeline_cache;
	VkDescriptorPool       descriptor_pool;
	void(*check_vk_result)(VkResult err);
};

IMGUI_API bool        ImGui_ImplGlfwVulkan_Init(GLFWwindow* window, ImGui_ImplGlfwVulkan_Init_Data *init_data);
IMGUI_API void        ImGui_ImplGlfwVulkan_Shutdown();
IMGUI_API void        ImGui_ImplGlfwVulkan_NewFrame();
IMGUI_API void        ImGui_ImplGlfwVulkan_Render(VkCommandBuffer command_buffer);

// Use if you want to reset your rendering device without losing ImGui state.
IMGUI_API void        ImGui_ImplGlfwVulkan_InvalidateFontUploadObjects();
IMGUI_API void        ImGui_ImplGlfwVulkan_InvalidateDeviceObjects();
IMGUI_API bool        ImGui_ImplGlfwVulkan_CreateFontsTexture(VkCommandBuffer command_buffer);
IMGUI_API bool        ImGui_ImplGlfwVulkan_CreateDeviceObjects();

#endif