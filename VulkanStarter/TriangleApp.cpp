#include "TriangleApp.h"

void TriangleApp::Run()
{

	InitWindow();
	InitVulkan();
	MainLoop();
	Cleanup();

}

void TriangleApp::InitVulkan()
{
	CreateInstance();
	SetupDebugCallback();
	PickPhysicalDevice();
}

void TriangleApp::MainLoop()
{
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
	}
}

void TriangleApp::Cleanup()
{
	if (enableValidationLayers)
	{
		DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
	}

	vkDestroyInstance(instance, nullptr);

	glfwDestroyWindow(window);

	glfwTerminate();
}

void TriangleApp::InitWindow()
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	window = glfwCreateWindow(WIDTH, HEIGHT, "VulkanStarter", nullptr, nullptr);
}

void TriangleApp::CreateInstance()
{
	if (enableValidationLayers && !CheckValidationLayerSupport())
	{
		throw std::runtime_error("Validation layers requested, but not available!!!");
	}

	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Triangle";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "VulkanStarter";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;
	appInfo.pNext = nullptr;

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	auto extensions = GetRequiredExtensions();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;

	if (enableValidationLayers)
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();

		populateDebugMessengerCreateInfo(debugCreateInfo);
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)& debugCreateInfo;
	}
	else
	{
		createInfo.enabledLayerCount = 0;
		createInfo.pNext = nullptr;
	}

	VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);

	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create instance!");
	}


	//for the extension names
	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	std::vector<VkExtensionProperties> vExtensions(extensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, vExtensions.data());

	std::cout << "available extensions:" << std::endl;

	for (const auto& extension : vExtensions) {
		std::cout << "\t" << extension.extensionName << std::endl;
	}

}

bool TriangleApp::CheckValidationLayerSupport()
{
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr); //for what ---?

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data()); //same thing here---?

	for (const char* layerName : validationLayers)
	{
		bool layerFound = false;

		for (const VkLayerProperties& layerProperties : availableLayers)
		{
			if (strcmp(layerName, layerProperties.layerName) == 0)
			{
				layerFound = true;
				break;
			}
		}
		if (!layerFound)
		{
			return false;
		}
	}

	return true;
}

std::vector<const char*> TriangleApp::GetRequiredExtensions()
{
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	if (enableValidationLayers)
	{
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return extensions;
}

VKAPI_ATTR VkBool32 VKAPI_CALL TriangleApp::DebugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData)
{
	std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

	return VK_FALSE;
}
void TriangleApp::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
	createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = DebugCallback;
}

void TriangleApp::PickPhysicalDevice()
{
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

	if (deviceCount == 0)
	{
		throw runtime_error("failed to find GPUs with vulkan support");
	}

	vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
	for (const auto& device : devices)
	{
		if (IsDeviceSuitable(device))
		{
			physicalDevice = device;
			break;
		}
	}

	if (physicalDevice == VK_NULL_HANDLE)
	{
		throw runtime_error("failed to find a suitable GPU. ");
	}

	
}

bool TriangleApp::IsDeviceSuitable(VkPhysicalDevice device)
{
	QueueFamilyIndices indices = FindQueueFamilies(device);

	return indices.m_IsComplete();
}

QueueFamilyIndices TriangleApp::FindQueueFamilies(VkPhysicalDevice device)
{
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for (const auto& queueFamily : queueFamilies) 
	{
		if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) 
		{
			indices.graphicsFamily = i;
		}

		if (indices.m_IsComplete()) 
		{
			break;
		}

		i++;
	}

	return indices;
}


VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
{
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr)
	{
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}
	else
	{
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr)
	{
		func(instance, debugMessenger, pAllocator);
	}
}

void TriangleApp::SetupDebugCallback()
{
	if (!enableValidationLayers)
		return;

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
	populateDebugMessengerCreateInfo(debugCreateInfo);

	if (CreateDebugUtilsMessengerEXT(instance, &debugCreateInfo, nullptr, &debugMessenger) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to set up debug messenger!");
	}
}
