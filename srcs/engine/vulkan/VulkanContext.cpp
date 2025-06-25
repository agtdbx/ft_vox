#include <engine/vulkan/VulkanContext.hpp>

#include <engine/vulkan/VulkanUtils.hpp>

#include <stdexcept>
#include <algorithm>
#include <cstring>
#include <cstdint>
#include <fstream>
#include <limits>
#include <chrono>
#include <set>

//**** STATIC FUNCTIONS DEFINITIONS ********************************************

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
										VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
										VkDebugUtilsMessageTypeFlagsEXT messageType,
										const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
										void *pUserData);
static VkResult	CreateDebugUtilsMessengerEXT(
					VkInstance instance,
					const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
					const VkAllocationCallbacks *pAllocator,
					VkDebugUtilsMessengerEXT *pDebugMessenger);
static void	DestroyDebugUtilsMessengerEXT(
				VkInstance instance,
				VkDebugUtilsMessengerEXT debugMessenger,
				const VkAllocationCallbacks* pAllocator);
static bool	checkDeviceExtensionSupport(VkPhysicalDevice device);
static bool	isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface);

//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------

VulkanContext::VulkanContext(void)
{
	this->instance = NULL;
	this->device = NULL;
	this->transferQueue = NULL;
}

//---- Destructor --------------------------------------------------------------

VulkanContext::~VulkanContext()
{
}

//**** ACCESSORS ***************************************************************
//---- Getters -----------------------------------------------------------------

VkDevice	VulkanContext::getDevice(void) const
{
	return (this->device);
}


VkPhysicalDevice	VulkanContext::getPhysicalDevice(void) const
{
	return (this->physicalDevice);
}


VkInstance	VulkanContext::getInstance(void) const
{
	return (this->instance);
}


const VulkanQueue	&VulkanContext::getGraphicsQueue(void) const
{
	return (this->graphicsQueue);
}


const VulkanQueue	&VulkanContext::getPresentQueue(void) const
{
	return (this->presentQueue);
}


const VulkanQueue	&VulkanContext::getTransferQueue(int id) const
{
	return (this->transferQueue[id]);
}

//---- Setters -----------------------------------------------------------------
//---- Operators ---------------------------------------------------------------
//**** PUBLIC METHODS **********************************************************

void	VulkanContext::init(VulkanCommandPool &commandPool, Window &window)
{
	this->createInstance();
	this->setupDebugMessenger();

	window.createSurface(this->instance);

	this->findPhysicalDevice(window);
	this->createLogicalDevice(window);

	commandPool.create(this->device, this->physicalDevice,
						window.getSurface(), this->graphicsQueue);

	window.init(*this, commandPool);
}


void	VulkanContext::destroy(void)
{
	if (this->transferQueue != NULL)
		delete [] this->transferQueue;

	if (this->device != NULL)
	{
		vkDestroyDevice(this->device, nullptr);
		this->device = NULL;
	}

	if (enableValidationLayers)
		DestroyDebugUtilsMessengerEXT(this->instance, this->debugMessenger, nullptr);

	if (this->instance != NULL)
	{
		vkDestroyInstance(this->instance, nullptr);
		this->instance = NULL;
	}
}

//**** PRIVATE METHODS *********************************************************
//---- Init sub part -----------------------------------------------------------

bool	VulkanContext::checkValidationLayerSupport(void)
{
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char* layerName : validationLayers)
	{
		bool layerFound = false;

		for (const auto& layerProperties : availableLayers)
		{
			if (strcmp(layerName, layerProperties.layerName) == 0)
			{
				layerFound = true;
				break;
			}
		}

		if (!layerFound)
			return (false);
	}

	return (true);
}


void	VulkanContext::createInstance(void)
{
	// Check validation layer if needed
	if (enableValidationLayers && !this->checkValidationLayerSupport())
		throw std::runtime_error("Validation layers failed");

	// Data about the app
	VkApplicationInfo	appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = WIN_TITLE;
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = ENGINE_TITLE;
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	// Device creation info struct
	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	// Get extensions required for device creation
	std::vector<const char*>	extensions = this->getRequiredExtensions();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
	if (enableValidationLayers)
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();

		populateDebugMessengerCreateInfo(debugCreateInfo);
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
	}
	else
	{
		createInfo.enabledLayerCount = 0;
		createInfo.pNext = nullptr;
	}

	// Create vulkan instance
	if (vkCreateInstance(&createInfo, nullptr, &this->instance) != VK_SUCCESS)
		throw std::runtime_error("Vulkan instance creation failed");
}


void	VulkanContext::findPhysicalDevice(Window &window)
{
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
	if (deviceCount == 0)
		throw std::runtime_error("Find GPU failed");

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

	for (const auto& device : devices)
	{
		if (isDeviceSuitable(device, window.getSurface()))
		{
			this->physicalDevice = device;
			break;
		}
	}
	if (this->physicalDevice == VK_NULL_HANDLE)
		throw std::runtime_error("Find GPU compatible with vulkan failed");
}


void	VulkanContext::createLogicalDevice(Window &window)
{
	QueueFamilyIndices queueIndices = findQueueFamilies(physicalDevice,
														window.getSurface());

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = {queueIndices.graphicsFamily.value(),
												queueIndices.presentFamily.value()};

	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies) {
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures deviceFeatures{};
	deviceFeatures.samplerAnisotropy = VK_TRUE;
	deviceFeatures.fillModeNonSolid = VK_TRUE;

	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pEnabledFeatures = &deviceFeatures;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();

	if (enableValidationLayers)
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();

	}
	else
		createInfo.enabledLayerCount = 0;

	if (vkCreateDevice(this->physicalDevice, &createInfo, nullptr, &this->device) != VK_SUCCESS)
		throw std::runtime_error("Logical device creation failed");

	this->graphicsQueue.id = queueIndices.graphicsFamily.value();
	vkGetDeviceQueue(this->device, queueIndices.graphicsFamily.value(), 0, &this->graphicsQueue.value);
	this->presentQueue.id = queueIndices.presentFamily.value();
	vkGetDeviceQueue(this->device, queueIndices.presentFamily.value(), 0, &this->presentQueue.value);

	this->transferQueue = new VulkanQueue[MAP_NB_THREAD];

	for (int i = 0; i < MAP_NB_THREAD; i++)
	{
		this->transferQueue[i].id = queueIndices.transferFamily.value();
		vkGetDeviceQueue(this->device, queueIndices.transferFamily.value(), 0, &this->transferQueue[i].value);
	}
}

//---- Utils -------------------------------------------------------------------

void	VulkanContext::setupDebugMessenger(void)
{
	if (!enableValidationLayers)
		return;

	VkDebugUtilsMessengerCreateInfoEXT createInfo;
	this->populateDebugMessengerCreateInfo(createInfo);

	if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &this->debugMessenger) != VK_SUCCESS) {
		throw std::runtime_error("Debug messenger set failed");
	}
}


void	VulkanContext::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo)
{
	createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = debugCallback;
	createInfo.pUserData = nullptr; // Optional
}


std::vector<const char*>	VulkanContext::getRequiredExtensions(void)
{
	uint32_t	glfwExtensionCount = 0;
	const char	**glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	if (enableValidationLayers)
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

	return (extensions);
}

//**** STATIC FUNCTIONS ********************************************************

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
										VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
										VkDebugUtilsMessageTypeFlagsEXT messageType,
										const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
										void *pUserData)
{
	if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
		std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
	return (VK_FALSE);
}


static VkResult	CreateDebugUtilsMessengerEXT(
					VkInstance instance,
					const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
					const VkAllocationCallbacks *pAllocator,
					VkDebugUtilsMessengerEXT *pDebugMessenger)
{
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr)
		return (func(instance, pCreateInfo, pAllocator, pDebugMessenger));
	return (VK_ERROR_EXTENSION_NOT_PRESENT);
}


static void	DestroyDebugUtilsMessengerEXT(
				VkInstance instance,
				VkDebugUtilsMessengerEXT debugMessenger,
				const VkAllocationCallbacks* pAllocator)
{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr)
		func(instance, debugMessenger, pAllocator);
}


static bool	checkDeviceExtensionSupport(VkPhysicalDevice physicalDevice)
{
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, availableExtensions.data());

	std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

	for (const auto& extension : availableExtensions)
		requiredExtensions.erase(extension.extensionName);

	return (requiredExtensions.empty());
}


static bool	isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface)
{
	QueueFamilyIndices	queueFamilyIndices = findQueueFamilies(device, surface);
	bool extensionsSupported = checkDeviceExtensionSupport(device);
	bool swapChainAdequate = false;

	if (extensionsSupported)
	{
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device, surface);
		swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
	}

	VkPhysicalDeviceFeatures supportedFeatures;
	vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

	return (queueFamilyIndices.isComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy);
}
