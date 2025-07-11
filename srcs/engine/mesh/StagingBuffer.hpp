#ifndef STAGING_BUFFER_HPP
# define STAGING_BUFFER_HPP

# include <engine/vulkan/VulkanCommandPool.hpp>
# include <engine/vulkan/VulkanUtils.hpp>

/**
 * @brief Struct for temporary buffer for mesh creation
 */
struct StagingBuffer
{
	VkDeviceSize	offset = 0;
	VkDeviceSize	size = 0;
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
		this->size = bufferSize;

		// Create buffers
		createVulkanBuffer(commandPool.getCopyDevice(), commandPool.getCopyPhysicalDevice(),
							this->size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
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
				VulkanCommandPool &commandPool,
				VkDeviceSize bufferSize)
	{
		if (bufferSize <= this->size)
			return ;

		this->destroy(commandPool.getCopyDevice());

		this->size = bufferSize;

		// Create buffers
		createVulkanBuffer(commandPool.getCopyDevice(), commandPool.getCopyPhysicalDevice(),
							this->size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
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

		this->offset = 0;
		this->size = 0;
	}

};

//**** FUNCTIONS ***************************************************************

#endif
