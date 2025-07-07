#ifndef VULKANCONTEXT_HPP
# define VULKANCONTEXT_HPP

# include <define.hpp>
# include <engine/vulkan/VulkanCommandPool.hpp>
# include <engine/vulkan/VulkanQueue.hpp>
# include <engine/window/Window.hpp>

# include <gmath.hpp>

# ifdef DEBUG
const bool enableValidationLayers = true;
# else
const bool enableValidationLayers = false;
# endif

// Validation layers
# include <vector>
const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

// Device extensions
const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};


/**
 * @brief Class for Vulkan context.
 */
class VulkanContext
{
public:
//**** PUBLIC ATTRIBUTS ********************************************************
//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------
	/**
	 * @brief Default contructor of VulkanContext class.
	 *
	 * @return The default VulkanContext that isn't working.
	 */
	VulkanContext(void);

//---- Destructor --------------------------------------------------------------
	/**
	 * @brief Destructor of VulkanContext class.
	 */
	~VulkanContext();

//**** ACCESSORS ***************************************************************
//---- Getters -----------------------------------------------------------------
	/**
	 * @brief Getter for device.
	 *
	 * @return The vulkan device.
	 */
	VkDevice	getDevice(void) const;
	/**
	 * @brief Getter for physical device.
	 *
	 * @return The vulkan physical device.
	 */
	VkPhysicalDevice	getPhysicalDevice(void) const;
	/**
	 * @brief Getter for instance.
	 *
	 * @return The vulkan instance.
	 */
	VkInstance	getInstance(void) const;
	/**
	 * @brief Getter for graphic queue.
	 *
	 * @return The vulkan graphic queue.
	 */
	const VulkanQueue	&getGraphicsQueue(void) const;
	/**
	 * @brief Getter for present queue.
	 *
	 * @return The vulkan present queue.
	 */
	const VulkanQueue	&getPresentQueue(void) const;
	/**
	 * @brief Getter for transfer queue.
	 *
	 * @return The vulkan transfer queue.
	 */
	const VulkanQueue	&getTransferQueue(void) const;

//---- Setters -----------------------------------------------------------------
//---- Operators ---------------------------------------------------------------
//**** PUBLIC METHODS **********************************************************
//---- Creation ----------------------------------------------------------------
	/**
	 * @brief Init context, command pool and window.
	 *
	 * @param commandPool The command pool to create.
	 * @param window The window to create.
	 */
	void	init(VulkanCommandPool &commandPool, Window &window);

//---- Free --------------------------------------------------------------------
	/**
	 * @brief Destroy context allocate attribut.
	 */
	void	destroy(void);

//**** STATIC METHODS **********************************************************

private:
//**** PRIVATE ATTRIBUTS *******************************************************
	VkInstance					instance;
	VkDebugUtilsMessengerEXT	debugMessenger;
	VkPhysicalDevice			physicalDevice;
	VkDevice					device;
	VulkanQueue					graphicsQueue, presentQueue, transferQueue;

//**** PRIVATE METHODS *********************************************************
//---- Init sub part -----------------------------------------------------------
	/**
	 * @brief Check the validation layer support.
	 *
	 * @return True if the layer are supported, false else.
	 */
	bool	checkValidationLayerSupport(void);
	/**
	 * @brief Create vulkan instance.
	 *
	 * The instance is the connection between the projet and vulkan.
	 */
	void	createInstance(void);
	/**
	 * @brief Find physical device.
	 */
	void	findPhysicalDevice(Window &window);
	/**
	 * @brief Create device from physical device.
	 */
	void	createLogicalDevice(Window &window);

//---- Utils -------------------------------------------------------------------
	/**
	 * @brief Setup print function of debug messages of validation layers.
	 */
	void	setupDebugMessenger(void);
	/**
	 * @brief Fill debug messenger struct.
	 */
	void	populateDebugMessengerCreateInfo(
				VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	/**
	 * @brief Get requiered extensions.
	 */
	std::vector<const char*>	getRequiredExtensions(void);
};

#endif
