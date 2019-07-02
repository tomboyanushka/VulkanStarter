#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <functional>
#include <cstdlib>
#include <cstring>
#include <optional>

using namespace std;

const int WIDTH = 1920;
const int HEIGHT = 1080;

const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

VkResult CreateDebugUtilsMessengerEXT(
	VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
	const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

struct QueueFamilyIndices 
{
	std::optional<uint32_t> graphicsFamily;

	bool m_IsComplete() //m_ --> function in a struct
	{
		return graphicsFamily.has_value();
	}
};

class TriangleApp
{

public:
	void Run();


private:
	GLFWwindow* window = {};
	VkInstance instance = {};
	VkDebugUtilsMessengerEXT debugMessenger = {};

	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDevice device;
	VkQueue graphicsQueue;

	void InitVulkan();
	void MainLoop();
	void Cleanup();
	void InitWindow();
	void CreateInstance();
	bool CheckValidationLayerSupport();
	std::vector<const char*> GetRequiredExtensions();
	
	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData);
	
	void SetupDebugCallback();
	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	
	void PickPhysicalDevice();
	bool IsDeviceSuitable(VkPhysicalDevice device);

	QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);

	//you can create multiple logical devices from the same physical device
	void CreateLogicalDevice(); 

};

