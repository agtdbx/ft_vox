#ifndef VULKAN_COMMAND_POOL_HPP
# define VULKAN_COMMAND_POOL_HPP

# include <define.hpp>

# include <vector>

/**
 * @brief Class for Vulkan command pool.
 */
class VulkanCommandPool
{
public:
//**** PUBLIC ATTRIBUTS ********************************************************
//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------
	/**
	 * @brief Default contructor of VulkanCommandPool class.
	 *
	 * @return The default VulkanCommandPool that isn't working.
	 */
	VulkanCommandPool(void);
	/**
	 * @brief Contructor of VulkanCommandPool class.
	 * All parameters will be copied into the class for easy use.
	 *
	 * @param device The device of VulkanContext class.
 	 * @param physicalDevice The physicalDevice of VulkanContext class.
 	 * @param surface The surface of VulkanContext class.
	 * @param graphicsQueue The graphics queue to where execute the command.
	 *
	 * @return The VulkanCommandPool.
	 * @exception Throw a runtime_error if pool creation failed.
	 */
	VulkanCommandPool(VkDevice device, VkPhysicalDevice physicalDevice,
						VkSurfaceKHR surface, VkQueue graphicsQueue);

//---- Destructor --------------------------------------------------------------
	/**
	 * @brief Destructor of VulkanCommandPool class.
	 */
	~VulkanCommandPool();

//**** ACCESSORS ***************************************************************
//---- Getters -----------------------------------------------------------------
	/**
	 * @brief Getter for command buffers.
	 *
	 * @return A vector of command buffer.
	 */
	std::vector<VkCommandBuffer>	&getCommandBuffers(void);
	/**
	 * @brief Get the copy of device.
	 *
	 * @return The copy of device.
	 */
	VkDevice	getCopyDevice();
	/**
	 * @brief Get the copy of physical device.
	 *
	 * @return The copy of physical device.
	 */
	VkPhysicalDevice	getCopyPhysicalDevice();

//---- Setters -----------------------------------------------------------------
//---- Operators ---------------------------------------------------------------
//**** PUBLIC METHODS **********************************************************
//---- Creation ----------------------------------------------------------------
	/**
	 * @brief Create a command pool.
	 * All parameters will be copied into the class for easy use.
	 *
	 * @param device The device of VulkanContext class.
 	 * @param physicalDevice The physicalDevice of VulkanContext class.
 	 * @param surface The surface of VulkanContext class.
	 * @param graphicsQueue The graphics queue to where execute the command.
	 *
	 * @warning Will destroy the previous command pool if there is one.
	 * @exception Throw a runtime_error if pool creation failed.
	 *
	 */
	void	create(VkDevice device, VkPhysicalDevice physicalDevice,
						VkSurfaceKHR surface, VkQueue graphicsQueue);

//---- Free --------------------------------------------------------------------
	/**
	 * @brief Destroy method for free allocated memory.
	 */
	void	destroy(VkDevice device);

//---- Commands ----------------------------------------------------------------
	/**
	 * @brief Create an empty command.
	 *
	 * @return The empty command.
	 * @exception Throw an runtime_error if the command pool is destroy or not create.
	 */
	VkCommandBuffer	beginSingleTimeCommands(void) const;
	/**
	 * @brief Execute, wait execution and free command.
	 *
	 * @param commandBuffer The command to execute and free.
	 *
	 * @exception Throw an runtime_error if the command pool is destroy or not create.
	 */
	void	endSingleTimeCommands(VkCommandBuffer commandBuffer) const;

//**** STATIC METHODS **********************************************************

private:
//**** PRIVATE ATTRIBUTS *******************************************************
	VkCommandPool					commandPool;
	std::vector<VkCommandBuffer>	commandBuffers;
//---- Copy --------------------------------------------------------------------
	VkDevice						copyDevice;
	VkPhysicalDevice				copyPhysicalDevice;
	VkSurfaceKHR					copySurface;
	VkQueue							copyGraphicsQueue;

//**** PRIVATE METHODS *********************************************************
};

//**** FUNCTIONS ***************************************************************

#endif


