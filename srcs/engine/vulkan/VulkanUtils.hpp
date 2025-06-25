#ifndef VULKAN_UTILS_HPP
# define VULKAN_UTILS_HPP

# include <define.hpp>
# include <engine/vulkan/VulkanCommandPool.hpp>

# include <optional>

//**** STRUCTS *****************************************************************
typedef struct QueueFamilyIndices_s
{
	std::optional<uint32_t>	graphicsFamily;
	std::optional<uint32_t>	presentFamily;

	bool	isComplete(void)
	{
		return (this->graphicsFamily.has_value() && this->presentFamily.has_value());
	}
}	QueueFamilyIndices;


// Swap chain struct
typedef struct SwapChainSupportDetails_s
{
	VkSurfaceCapabilitiesKHR		capabilities;
	std::vector<VkSurfaceFormatKHR>	formats;
	std::vector<VkPresentModeKHR>	presentModes;
}	SwapChainSupportDetails;

//**** FUNCTIONS ***************************************************************
//---- Finds -------------------------------------------------------------------
/**
 * @brief Find type of memory.
 *
 * @param physicalDevice The physicalDevice of VulkanContext class.
 * @param typeFilter Filter for type as bit mask.
 * @param properties Properties to find a specific type, use as bit mask.
 *
 * @return The memory type found as an unsigned int 32.
 * @exception Throw a runtime_error if not find any suitable memory with parameters.
 */
uint32_t	findMemoryType(
				VkPhysicalDevice physicalDevice,
				uint32_t typeFilter, VkMemoryPropertyFlags properties);
/**
 * @brief Find queue families.
 *
 * @param physicalDevice The physicalDevice of VulkanContext class.
 * @param surface The surface of VulkanContext class.
 *
 * @return Families find.
 * @exception Throw an runtime_error if no family is find.
 */
QueueFamilyIndices	findQueueFamilies(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
/**
 * @brief Find depth format.
 *
 * @param physicalDevice The physicalDevice of VulkanContext class.
 *
 * @return Format for depth.
 * @exception Throw an runtime_error if there is no suitable format found.
 */
VkFormat findDepthFormat(VkPhysicalDevice physicalDevice);

//---- Query -------------------------------------------------------------------
/**
 * @brief Query swap chain support.
 *
 * @param device The device of VulkanContext class.
 * @param surface The surface of VulkanContext class.
 *
 * @return The swap chain support.
 */
SwapChainSupportDetails	querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);

//---- Choose ------------------------------------------------------------------
/**
 * @brief Choose swap format for surface.
 *
 * @return Swap surface format.
 */
VkSurfaceFormatKHR	chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);
/**
 * @brief Choose swap format for present.
 *
 * @return Swap present format.
 */
VkPresentModeKHR	chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);
/**
 * @brief Choose swap extend.
 *
 * @return Swap extend.
 */
VkExtent2D	chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow *window);

//---- Creates -----------------------------------------------------------------
/**
 * @brief Create and allocate buffer usable with vulkan.
 *
 * @param device The device of VulkanContext class.
 * @param physicalDevice The physicalDevice of VulkanContext class.
 * @param size The wanted size of buffer.
 * @param usage Usage of the buffer, needed for optimisation.
 * @param properties Properties wanted of for the buffer.
 * @param buffer The buffer to create.
 * @param bufferMemory The memory allocated for the buffer.
 *
 * @exception Throw an runtime_error if the creation failed.
 */
void	createVulkanBuffer(
			VkDevice device, VkPhysicalDevice physicalDevice,
			VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
			VkBuffer &buffer, VkDeviceMemory &bufferMemory);
/**
 * @brief Create and allocate image usable with vulkan.
 *
 * @param device The device of VulkanContext class.
 * @param physicalDevice The physicalDevice of VulkanContext class.
 * @param width The width of image.
 * @param height The height of image.
 * @param format The format of image.
 * @param tiling The tiling of image.
 * @param usage The usage of image. The usage change the optimisation for image.
 * @param properties The properties of image.
 * @param image The image to create.
 * @param imageMemory The memory allocated for the image.
 *
 * @exception Throw an runtime_error if the creation failed.
 */
void	createVulkanImage(
			VkDevice device, VkPhysicalDevice physicalDevice,
			uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling,
			VkImageUsageFlags usage, VkMemoryPropertyFlags properties,
			VkImage &image, VkDeviceMemory &imageMemory);
/**
 * @brief Create and allocate image array usable with vulkan.
 *
 * @param device The device of VulkanContext class.
 * @param physicalDevice The physicalDevice of VulkanContext class.
 * @param width The width of images.
 * @param height The height of images.
 * @param format The format of images.
 * @param tiling The tiling of images.
 * @param usage The usage of images. The usage change the optimisation for image.
 * @param properties The properties of images.
 * @param image The image to create.
 * @param imageMemory The memory allocated for the image.
 * @param arraySize The size of the array.
 *
 * @exception Throw an runtime_error if the creation failed.
 */
void	createVulkanImageArray(
			VkDevice device, VkPhysicalDevice physicalDevice,
			uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling,
			VkImageUsageFlags usage, VkMemoryPropertyFlags properties,
			VkImage &image, VkDeviceMemory &imageMemory, uint32_t arraySize);
/**
 * @brief Create an image view from an image.
 *
 * @param device The device of VulkanContext class.
 * @param image The image used for create the image view.
 * @param format The format of image view.
 * @param aspectFlags The flags of image view.
 *
 * @return The image view created.
 * @exception Throw an runtime_error if the creation failed.
 */
VkImageView	createVulkanImageView(
				VkDevice device,
				VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
/**
 * @brief Create an image array view from an image array.
 *
 * @param device The device of VulkanContext class.
 * @param image The image used for create the image view.
 * @param format The format of image view.
 * @param aspectFlags The flags of image view.
 * @param arraySize The size of the array.
 *
 * @return The image view created.
 * @exception Throw an runtime_error if the creation failed.
 */
VkImageView	createVulkanImageArrayView(
				VkDevice device,
				VkImage image, VkFormat format, VkImageAspectFlags aspectFlags,
				uint32_t arraySize);

//---- Copies ------------------------------------------------------------------
/**
 * @brief Copy srcBuffer to dstBuffer.
 *
 * @param commandPool The command pool for run the copy.
 * @param srcBuffer The buffer that will be copied.
 * @param dstBuffer Where the buffer will be copied.
 * @param size The size of buffer that will be copied. Can be smaller thant srcBuffer size for copy only a part.
 */
void	copyBuffer(
			const VulkanCommandPool &commandPool,
			VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
/**
 * @brief Copy srcBuffer to dstBuffer.
 *
 * @param commandBuffer The command buffer for run the copy.
 * @param srcBuffer The buffer that will be copied.
 * @param dstBuffer Where the buffer will be copied.
 * @param size The size of buffer that will be copied. Can be smaller thant srcBuffer size for copy only a part.
 */
void	copyBufferAsync(
			VkCommandBuffer commandBuffer,
			VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
/**
 * @brief Copy buffer to image.
 *
 * @param commandPool The command pool for run the copy.
 * @param buffer The buffer that will be copied.
 * @param image Image where the buffer will be copied.
 * @param width Width of image.
 * @param height Height of image.
 */
void	copyBufferToImage(
			const VulkanCommandPool &commandPool,
			VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
/**
 * @brief Copy buffer to image array.
 *
 * @param commandPool The command pool for run the copy.
 * @param buffer The buffer that will be copied.
 * @param image Image array where the buffer will be copied.
 * @param width Width of image.
 * @param height Height of image.
 * @param nbLayer Number of layer into the array.
 */
void	copyBufferToImageArray(
			const VulkanCommandPool &commandPool,
			VkBuffer buffer, VkImage image,
			uint32_t width, uint32_t height,
			uint32_t nbLayer);

//---- Others ------------------------------------------------------------------
/**
 * @brief Change an image layout.
 *
 * @param commandPool The command pool for run the copy.
 * @param image The image to change.
 * @param format The format of image.
 * @param oldLayout The current layout of image.
 * @param newLayout The new layout wanted for image.
 */
void	transitionImageLayout(
			const VulkanCommandPool &commandPool,
			VkImage image, VkFormat format,
			VkImageLayout oldLayout, VkImageLayout newLayout);
/**
 * @brief Change an image layout.
 *
 * @param commandPool The command pool for run the copy.
 * @param image The image to change.
 * @param format The format of image.
 * @param oldLayout The current layout of image.
 * @param newLayout The new layout wanted for image.
 * @param nbLayer The number of layer of the image.
 */
void	transitionImageArrayLayout(
			const VulkanCommandPool &commandPool,
			VkImage image, VkFormat format,
			VkImageLayout oldLayout, VkImageLayout newLayout,
			uint32_t nbLayer);

#endif
