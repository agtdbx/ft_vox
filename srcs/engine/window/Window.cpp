#include <engine/window/Window.hpp>

#include <engine/shader/Shader.hpp>
#include <engine/vulkan/VulkanContext.hpp>

#include <array>

//**** STATIC VARIABLES ********************************************************

static bool	framebufferResized = false;

//**** STATIC FUNCTIONS DEFINE *************************************************

static void	framebufferResizeCallback(GLFWwindow* window, int width, int height);

//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------

Window::Window(void)
{
	// Init window variables
	this->window = NULL;
	this->size = gm::Vec2i(WIN_W, WIN_H);
	this->title = WIN_TITLE;

	// Create window
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	this->window = glfwCreateWindow(this->size.x, this->size.y,
										this->title.c_str(), nullptr, nullptr);
	if (window == NULL)
		throw std::runtime_error("GLFW window creation failed");

	// Window resize handler
	glfwSetFramebufferSizeCallback(this->window, framebufferResizeCallback);

	// Init vulkan variables
	this->currentFrame = 0;
	this->surface = NULL;
	this->swapChain = NULL;
	this->renderPass = NULL;
	this->depthImage = NULL;
	this->depthImageMemory = NULL;
	this->depthImageView = NULL;

	// Init copy variables
	this->copyDevice = NULL;
	this->copyPhysicalDevice = NULL;
	this->copyCommandPool = NULL;
	this->copyCommandBuffers = NULL;
}


Window::Window(const Window &obj)
{
	// Init window variables
	this->window = NULL;
	this->size = obj.size;
	this->title = obj.title;

	// Create window
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	this->window = glfwCreateWindow(this->size.x, this->size.y,
										this->title.c_str(), nullptr, nullptr);
	if (window == NULL)
		throw std::runtime_error("GLFW window creation failed");

	// Window resize handler
	glfwSetFramebufferSizeCallback(this->window, framebufferResizeCallback);

	// Init vulkan variables
	this->currentFrame = 0;
	this->surface = NULL;
	this->swapChain = NULL;
	this->renderPass = NULL;
	this->depthImage = NULL;
	this->depthImageMemory = NULL;
	this->depthImageView = NULL;

	// Init copy variables
	this->copyDevice = NULL;
	this->copyPhysicalDevice = NULL;
	this->copyCommandPool = NULL;
	this->copyCommandBuffers = NULL;
}

//---- Destructor --------------------------------------------------------------

Window::~Window()
{
	if (this->window != NULL)
		glfwDestroyWindow(this->window);
}

//**** ACCESSORS ***************************************************************
//---- Getters -----------------------------------------------------------------

GLFWwindow	*Window::getWindow(void) const
{
	return (this->window);
}


VkSurfaceKHR	Window::getSurface(void) const
{
	return (this->surface);
}


VkRenderPass	Window::getRenderPass(void)
{
	return (this->renderPass);
}


uint32_t	Window::getCurrentFrame(void)
{
	return (this->currentFrame);
}


const gm::Vec2i	&Window::getSize(void) const
{
	return (this->size);
}


const std::string	&Window::getTitle(void) const
{
	return (this->title);
}

//---- Setters -----------------------------------------------------------------

void	Window::setSize(const gm::Vec2i &size)
{
	this->size = size;
	glfwSetWindowSize(this->window, this->size.x, this->size.y);
	this->recreateSwapChain();
}


void	Window::setTitle(const std::string &title)
{
	this->title = title;
	glfwSetWindowTitle(this->window, this->title.c_str());
}

//---- Operators ---------------------------------------------------------------

Window	&Window::operator=(const Window &obj)
{
	if (this == &obj)
		return (*this);

	this->setSize(obj.size);
	this->setTitle(obj.title);

	return (*this);
}

//**** PUBLIC METHODS **********************************************************
//---- Creation ----------------------------------------------------------------

void	Window::createSurface(VkInstance instance)
{
	if (glfwCreateWindowSurface(instance, this->window, nullptr, &this->surface) != VK_SUCCESS)
		throw std::runtime_error("Surface creation failed");
}


void	Window::init(VulkanCommandPool &commandPool)
{
	// Copy
	this->copyDevice = commandPool.getCopyDevice();
	this->copyPhysicalDevice = commandPool.getCopyPhysicalDevice();
	this->copyCommandPool = &commandPool;

	this->createSwapChain();
	this->createImageViews();
	this->createSyncObjects();
	this->createRenderPass();
	this->createDepthResources();
	this->createFramebuffers();
}


void	Window::recreateSwapChain(void)
{
	if (this->copyDevice == NULL)
		return ;

	int	width = 0;
	int	height = 0;

	// Get window size
	glfwGetFramebufferSize(this->window, &width, &height);
	// If size if 0, keep asking size
	while (width == 0 || height == 0)
	{
		glfwGetFramebufferSize(this->window, &width, &height);
		glfwWaitEvents();
	}

	this->size = gm::Vec2i(width, height);

	vkDeviceWaitIdle(this->copyDevice);

	this->destroySwapChain();

	this->createSwapChain();
	this->createImageViews();
	this->createDepthResources();
	this->createFramebuffers();
}

//---- Destroy -----------------------------------------------------------------

void	Window::destroy(VkInstance instance)
{
	// Free swap chain
	this->destroySwapChain();

	// Free render pass
	if (this->renderPass != NULL)
		vkDestroyRenderPass(this->copyDevice, this->renderPass, nullptr);

	// Free frames data
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		if (this->renderFinishedSemaphores[i] != NULL)
			vkDestroySemaphore(this->copyDevice, this->renderFinishedSemaphores[i], nullptr);

		if (this->imageAvailableSemaphores[i] != NULL)
			vkDestroySemaphore(this->copyDevice, this->imageAvailableSemaphores[i], nullptr);

		if (this->inFlightFences[i] != NULL)
			vkDestroyFence(this->copyDevice, this->inFlightFences[i], nullptr);
	}

	// Free surface
	if (this->surface != NULL)
		vkDestroySurfaceKHR(instance, this->surface, nullptr);

}

//---- Draw --------------------------------------------------------------------

void	Window::startDraw(void)
{
	// Wait the end of render of previous frame
	vkWaitForFences(this->copyDevice, 1, &this->inFlightFences[this->currentFrame], VK_TRUE, UINT64_MAX);

	// Get an image from swap chain
	VkResult result = vkAcquireNextImageKHR(this->copyDevice, this->swapChain, UINT64_MAX, this->imageAvailableSemaphores[this->currentFrame], VK_NULL_HANDLE, &this->imageIndex);

	// Check if we need to recreate swap chain (because window size change)
	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		this->recreateSwapChain();
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		throw std::runtime_error("Swap chain image aquisition failed");

	// Reset the fence only if we will use it (avoid deadlock when recreate swap chain)
	vkResetFences(this->copyDevice, 1, &this->inFlightFences[this->currentFrame]);

	this->copyCommandBuffers = this->copyCommandPool->getCommandBuffers().data();
	vkResetCommandBuffer(this->copyCommandBuffers[this->currentFrame], 0);
}


void	Window::drawMesh(Mesh &mesh,  Shader &shader)
{
	VkCommandBuffer commandBuffer = this->copyCommandBuffers[this->currentFrame];
	VkPipelineLayout pipelineLayout = shader.getPipelineLayout();
	VkPipeline graphicsPipeline = shader.getGraphicsPipeline();
	std::vector<VkDescriptorSet> &descriptorSets = shader.getDescriptorSets();

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0; // Optional
	beginInfo.pInheritanceInfo = nullptr; // Optional

	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
		throw std::runtime_error("Begin record of command buffer failed");

	// Define render process
	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = this->renderPass;
	renderPassInfo.framebuffer = this->swapChainFramebuffers[this->imageIndex];
	renderPassInfo.renderArea.offset = {0, 0};
	renderPassInfo.renderArea.extent = this->swapChainExtent;

	// Depth clear
	std::array<VkClearValue, 2> clearValues{};
	clearValues[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
	clearValues[1].depthStencil = {1.0f, 0};

	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();

	// Start render process
	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

	// Define draw region size
	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(this->swapChainExtent.width);
	viewport.height = static_cast<float>(this->swapChainExtent.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

	// Define draw region mask
	VkRect2D scissor{};
	scissor.offset = {0, 0};
	scissor.extent = this->swapChainExtent;
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	// Use vertex and index buffers
	VkBuffer vertexBuffers[] = {mesh.getVertexBuffer()};
	VkDeviceSize offsets[] = {0};
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
	vkCmdBindIndexBuffer(commandBuffer, mesh.getIndexBuffer(), 0, VK_INDEX_TYPE_UINT32);

	// Bind uniform
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSets[currentFrame], 0, nullptr);

	// Draw with index
	vkCmdDrawIndexed(commandBuffer, mesh.getNbIndex(), 1, 0, 0, 0);

	// Wait drawing end
	vkCmdEndRenderPass(commandBuffer);
	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
		throw std::runtime_error("Command buffur record failed");
}


void	Window::endDraw(VulkanContext &context)
{
	VkQueue graphicsQueue = context.getGraphicsQueue();
	VkQueue presentQueue = context.getPresentQueue();

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = {this->imageAvailableSemaphores[this->currentFrame]};
	VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &this->copyCommandBuffers[this->currentFrame];

	VkSemaphore signalSemaphores[] = {this->renderFinishedSemaphores[this->currentFrame]};
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, this->inFlightFences[this->currentFrame]) != VK_SUCCESS)
		throw std::runtime_error("Draw command buffer submit failed");

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = {this->swapChain};
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &this->imageIndex;
	presentInfo.pResults = nullptr; // Optional

	VkResult result = vkQueuePresentKHR(presentQueue, &presentInfo);
	// Check if we need to recreate swap chain (because window size change)
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized)
	{
		framebufferResized = false;
		this->recreateSwapChain();
	}
	else if (result != VK_SUCCESS)
		throw std::runtime_error("Swap chain image presentation failed");

	this->currentFrame = (this->currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

//**** STATIC METHODS **********************************************************
//**** PRIVATE METHODS *********************************************************

void	Window::createSwapChain(void)
{
	SwapChainSupportDetails swapChainSupport = querySwapChainSupport(this->copyPhysicalDevice, this->surface);

	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
	VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
	VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities, this->window);

	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
		imageCount = swapChainSupport.capabilities.maxImageCount;

	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = this->surface;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	QueueFamilyIndices QueueIndices = findQueueFamilies(this->copyPhysicalDevice, this->surface);
	uint32_t queueFamilyIndices[] = {QueueIndices.graphicsFamily.value(), QueueIndices.presentFamily.value()};

	if (QueueIndices.graphicsFamily != QueueIndices.presentFamily)
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0; // Optional
		createInfo.pQueueFamilyIndices = nullptr; // Optional
	}
	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(this->copyDevice, &createInfo, nullptr, &this->swapChain) != VK_SUCCESS)
		throw std::runtime_error("Swap chain creation failed");

	vkGetSwapchainImagesKHR(this->copyDevice, this->swapChain, &imageCount, nullptr);
	swapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(this->copyDevice, this->swapChain, &imageCount, swapChainImages.data());

	this->swapChainImageFormat = surfaceFormat.format;
	this->swapChainExtent = extent;
}


void	Window::createImageViews(void)
{
	this->swapChainImageViews.resize(swapChainImages.size());

	// Create each image
	for (size_t i = 0; i < swapChainImages.size(); i++)
		this->swapChainImageViews[i] = createVulkanImageView(
										this->copyDevice, this->swapChainImages[i],
										this->swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);
}


void	Window::createSyncObjects(void)
{
	imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		if (vkCreateSemaphore(this->copyDevice, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(this->copyDevice, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
			vkCreateFence(this->copyDevice, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS)
			throw std::runtime_error("Semaphore or fence creation failed");
	}
}


void	Window::createRenderPass(void)
{
	// Define image buffer format
	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = this->swapChainImageFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	// Define subpasses (can be used for post processing)
	VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	// Define depth
	VkAttachmentDescription depthAttachment{};
	depthAttachment.format = findDepthFormat(this->copyPhysicalDevice);
	depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthAttachmentRef{};
	depthAttachmentRef.attachment = 1;
	depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	// Define the way data will be given to vertex shader
	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;
	subpass.pDepthStencilAttachment = &depthAttachmentRef;

	// Define dependencies
	VkSubpassDependency dependency{};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

	// Create render pass
	std::array<VkAttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};
	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	renderPassInfo.pAttachments = attachments.data();
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	if (vkCreateRenderPass(this->copyDevice, &renderPassInfo, nullptr, &this->renderPass) != VK_SUCCESS)
		throw std::runtime_error("Render pass creation failed");
}


void	Window::createDepthResources(void)
{
	VkFormat depthFormat = findDepthFormat(this->copyPhysicalDevice);

	createVulkanImage(
		this->copyDevice, this->copyPhysicalDevice,
		this->swapChainExtent.width, this->swapChainExtent.height, depthFormat,
		VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, this->depthImage, this->depthImageMemory);
	this->depthImageView = createVulkanImageView(
								this->copyDevice, this->depthImage,
								depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);

	transitionImageLayout(
		*this->copyCommandPool, this->depthImage, depthFormat,
		VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
}


void	Window::createFramebuffers(void)
{
	this->swapChainFramebuffers.resize(this->swapChainImageViews.size());

	// Create each framebuffer image
	for (size_t i = 0; i < this->swapChainImageViews.size(); i++)
	{
		std::array<VkImageView, 2> attachments = {
			this->swapChainImageViews[i],
			this->depthImageView
		};

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = this->renderPass;
		framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = this->swapChainExtent.width;
		framebufferInfo.height = this->swapChainExtent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(this->copyDevice, &framebufferInfo, nullptr, &this->swapChainFramebuffers[i]) != VK_SUCCESS)
			throw std::runtime_error("Framebuffer image creation failed");
	}
}


void	Window::destroySwapChain(void)
{
	if (this->copyDevice == NULL)
		return ;

	// Depth resources
	if (this->depthImageView != NULL)
		vkDestroyImageView(this->copyDevice, this->depthImageView, nullptr);
	if (this->depthImage != NULL)
		vkDestroyImage(this->copyDevice, this->depthImage, nullptr);
	if (this->depthImageMemory != NULL)
		vkFreeMemory(this->copyDevice, this->depthImageMemory, nullptr);

	// Frame buffers
	for (VkFramebuffer framebuffer : this->swapChainFramebuffers)
		vkDestroyFramebuffer(this->copyDevice, framebuffer, nullptr);

	// Image view
	for (VkImageView imageView : this->swapChainImageViews)
		vkDestroyImageView(this->copyDevice, imageView, nullptr);

	// Swap chain
	if (this->swapChain != NULL)
		vkDestroySwapchainKHR(this->copyDevice, this->swapChain, nullptr);
}

//**** FUNCTIONS ***************************************************************
//**** STATIC FUNCTIONS ********************************************************

static void	framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
	framebufferResized = true;
}
