#ifndef STAGING_BUFFER_HPP
# define STAGING_BUFFER_HPP

# include <engine/vulkan/VulkanCommandPool.hpp>
# include <engine/vulkan/VulkanUtils.hpp>

/**
 * @brief Struct for temporary buffer for mesh creation
 */
struct StagingBuffer
{
	VkDeviceSize	bufferSize = 0;
	VkBuffer		buffer = NULL;
	VkDeviceMemory	memory = NULL;

	/**
	 * @brief Create buffers.
	 *
 	 * @param commandPool The command pool for creating buffers.
 	 * @param bufferSize The size to allocate for buffers.
	 */
	void	create(
				VulkanCommandPool &commandPool, VkDeviceSize bufferSize)
	{
		VkDevice			copyDevice = commandPool.getCopyDevice();
		VkPhysicalDevice	copyPhysicalDevice = commandPool.getCopyPhysicalDevice();

		this->bufferSize = bufferSize;

		// Create buffers
		createVulkanBuffer(copyDevice, copyPhysicalDevice,
							this->bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
							VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
							this->buffer, this->memory);
	}
	/**
	 * @brief Create new buffer if the size if greater than the current one.
	 *
 	 * @param commandPool The command pool for creating buffers.
 	 * @param bufferSize The size to allocate for buffers.
	 */
	void	update(
				VulkanCommandPool &commandPool, VkDeviceSize bufferSize)
	{
		if (bufferSize <= this->bufferSize)
			return ;

		VkDevice			copyDevice = commandPool.getCopyDevice();
		VkPhysicalDevice	copyPhysicalDevice = commandPool.getCopyPhysicalDevice();

		this->destroy(copyDevice);

		this->bufferSize = bufferSize;

		// Create buffers
		createVulkanBuffer(copyDevice, copyPhysicalDevice,
							this->bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
							VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
							this->buffer, this->memory);
	}
	/**
	 * @brief Create buffers.
	 *
 	 * @param device The device in vulkan context class.
	 */
	void	destroy(VkDevice device)
	{
		if (this->buffer != NULL)
			vkDestroyBuffer(device, this->buffer, nullptr);

		if (this->memory != NULL)
			vkFreeMemory(device, this->memory, nullptr);
	}

};

//**** FUNCTIONS ***************************************************************

#endif
