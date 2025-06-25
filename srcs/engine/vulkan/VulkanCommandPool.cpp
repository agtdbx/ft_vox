#include <engine/vulkan/VulkanCommandPool.hpp>

#include <engine/vulkan/VulkanUtils.hpp>

#include <stdexcept>

//**** STATIC FUNCTIONS DEFINE *************************************************
//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------

VulkanCommandPool::VulkanCommandPool(void)
{
	this->commandPool = NULL;
	this->copyDevice = NULL;
	this->copyPhysicalDevice = NULL;
	this->copySurface = NULL;
	this->copyQueue = NULL;
}


VulkanCommandPool::VulkanCommandPool(VkDevice device, VkPhysicalDevice physicalDevice,
										VkSurfaceKHR surface, const VulkanQueue &queue)
{
	this->copyDevice = device;
	this->copyPhysicalDevice = physicalDevice;
	this->copySurface = surface;
	this->copyQueue = queue.value;

	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = queue.id;

	if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS)
		throw std::runtime_error("Command pool creation failed");
}

//---- Destructor --------------------------------------------------------------

VulkanCommandPool::~VulkanCommandPool()
{
}

//**** ACCESSORS ***************************************************************
//---- Getters -----------------------------------------------------------------

std::vector<VkCommandBuffer>	&VulkanCommandPool::getCommandBuffers(void)
{
	return (this->commandBuffers);
}


VkDevice	VulkanCommandPool::getCopyDevice()
{
	return (this->copyDevice);
}


VkPhysicalDevice	VulkanCommandPool::getCopyPhysicalDevice()
{
	return (this->copyPhysicalDevice);
}

//---- Setters -----------------------------------------------------------------
//---- Operators ---------------------------------------------------------------
//**** PUBLIC METHODS **********************************************************
//---- Creation ----------------------------------------------------------------

void	VulkanCommandPool::create(VkDevice device, VkPhysicalDevice physicalDevice,
									VkSurfaceKHR surface, const VulkanQueue &queue)
{
	if (this->commandPool != NULL)
		this->destroy(device);

	this->copyDevice = device;
	this->copyPhysicalDevice = physicalDevice;
	this->copySurface = surface;
	this->copyQueue = queue.value;

	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = queue.id;

	if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS)
		throw std::runtime_error("Command pool creation failed");

	this->commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = this->commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t) this->commandBuffers.size();

	if (vkAllocateCommandBuffers(device, &allocInfo, this->commandBuffers.data()) != VK_SUCCESS)
		throw std::runtime_error("Command buffers allocation failed");
}

//---- Free --------------------------------------------------------------------

void	VulkanCommandPool::destroy(VkDevice device)
{
	if (this->commandPool != NULL)
	{
		vkDestroyCommandPool(device, this->commandPool, nullptr);
		this->commandPool = NULL;
		this->copyDevice = NULL;
		this->copyPhysicalDevice = NULL;
		this->copySurface = NULL;
		this->copyQueue = NULL;
	}
}

//---- Commands ----------------------------------------------------------------

VkCommandBuffer	VulkanCommandPool::beginSingleTimeCommands(void) const
{
	if (this->commandPool == NULL)
		throw std::runtime_error("No command pool");

	// Allocate command
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = this->commandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(this->copyDevice, &allocInfo, &commandBuffer);

	// Prepare command
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	return (commandBuffer);
}


void	VulkanCommandPool::endSingleTimeCommands(VkCommandBuffer commandBuffer) const
{
	if (this->commandPool == NULL)
		throw std::runtime_error("No command pool");

	vkEndCommandBuffer(commandBuffer);

	// Start command and wait it's execution
	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(this->copyQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(this->copyQueue);

	// Release command
	vkFreeCommandBuffers(this->copyDevice, this->commandPool, 1, &commandBuffer);
}

//**** STATIC METHODS **********************************************************
//**** PRIVATE METHODS *********************************************************
//**** FUNCTIONS ***************************************************************
//**** STATIC FUNCTIONS ********************************************************
