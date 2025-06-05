#ifndef WINDOW_HPP
# define WINDOW_HPP

# include <define.hpp>
# include <engine/vulkan/VulkanUtils.hpp>
# include <engine/vulkan/VulkanCommandPool.hpp>
# include <engine/mesh/Mesh.hpp>

# include <gmath.hpp>
# include <string>

class Shader;
class VulkanContext;

/**
 * @brief Class for window and attach process of it.
 */
class Window
{
public:
//**** PUBLIC ATTRIBUTS ********************************************************
//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------
	/**
	 * @brief Default contructor of Window class.
	 *
	 * @return The default Window.
	 */
	Window(void);
	/**
	 * @brief Copy constructor of Window class.
	 *
	 * @param obj The Window to copy.
	 *
	 * @return The Window copied from parameter.
	 */
	Window(const Window &obj);

//---- Destructor --------------------------------------------------------------
	/**
	 * @brief Destructor of Window class.
	 */
	~Window();

//**** ACCESSORS ***************************************************************
//---- Getters -----------------------------------------------------------------
	/**
	 * @brief Getter of glfw window.
	 *
	 * @return Glfw window pointer.
	 */
	GLFWwindow	*getWindow(void) const;
	/**
	 * @brief Getter of vulkan surface.
	 *
	 * @return Vulkan surface.
	 */
	VkSurfaceKHR	getSurface(void) const;
	/**
	 * @brief Getter of vulkan render pass.
	 *
	 * @return Vulkan render pass.
	 */
	VkRenderPass	getRenderPass(void);
	/**
	 * @brief Getter of current frame.
	 *
	 * @return Current frame index as uint32.
	 */
	uint32_t	getCurrentFrame(void);
	/**
	 * @brief Getter of window size.
	 *
	 * @return Window size as Vec2i.
	 */
	const gm::Vec2i	&getSize(void) const;
	/**
	 * @brief Getter of window title.
	 *
	 * @return Window title as string.
	 */
	const std::string	&getTitle(void) const;

//---- Setters -----------------------------------------------------------------
	/**
	 * @brief Setter of window size.
	 *
	 * @param size New size of window.
	 */
	void	setSize(const gm::Vec2i &size);
	/**
	 * @brief Setter of window title.
	 *
	 * @param title New title of window.
	 */
	void	setTitle(const std::string &title);

//---- Operators ---------------------------------------------------------------
	/**
	 * @brief Copy operator of Window class.
	 *
	 * @param obj The Window to copy.
	 *
	 * @return The Window copied from parameter.
	 */
	Window	&operator=(const Window &obj);

//**** PUBLIC METHODS **********************************************************
//---- Creation ----------------------------------------------------------------
	/**
	 * @brief Create vulkan surface.
	 *
	 * @param instance The instance of VulkanContext class.
	 */
	void	createSurface(VkInstance instance);
	/**
	 * @brief Init swap chain, image views and sync objects.
	 *
	 * @param commandPool The command pool for run vulkan commands.
	 *
	 * @exception Throw an runtime_error if a creation failed.
	 */
	void	init(VulkanCommandPool &commandPool);
	/**
	 * @brief Recreate the swap chain. Work only if you already have call init.
	 */
	void	recreateSwapChain(void);
//---- Destroy -----------------------------------------------------------------
	/**
	 * @brief Destroy vulkan's attribut of window.
	 *
	 * @param instance The instance of VulkanContext class.
	 */
	void	destroy(VkInstance instance);
//---- Draw --------------------------------------------------------------------
	/**
	 * @brief Prepare drawing.
	 *
	 * @warning Will crash if you don't have call init method before. (No check for speed).
	 */
	void	startDraw(void);
	/**
	 * @brief Draw a mesh with a render pipeline.
	 *
	 * @param mesh Mesh to draw.
	 * @param shader Shader used to draw mesh.
	 */
	template<typename VertexType>
	void	drawMesh(Mesh<VertexType> &mesh, Shader &shader)
	{
		VkCommandBuffer commandBuffer = this->copyCommandBuffers[this->currentFrame];
		VkPipelineLayout pipelineLayout;
		VkPipeline graphicsPipeline;
		std::vector<VkDescriptorSet> descriptorSets;

		getShaderInfo(pipelineLayout,graphicsPipeline, descriptorSets, shader);

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
	/**
	 * @brief Finish and apply draw onto window.
	 *
	 * @param graphicsQueue Vulkan contexte graphics queue.
	 * @param presentQueue Vulkan contexte present queue.
	 */
	void	endDraw(VulkanContext &context);

//**** STATIC METHODS **********************************************************

private:
//**** PRIVATE ATTRIBUTS *******************************************************
//---- Window ------------------------------------------------------------------
	GLFWwindow						*window;
	gm::Vec2i						size;
	std::string						title;
	VkSurfaceKHR					surface;
//---- Swap chain --------------------------------------------------------------
	uint32_t						imageIndex;
	uint32_t						currentFrame;
	VkSwapchainKHR					swapChain;
	std::vector<VkImage>			swapChainImages;
	VkFormat						swapChainImageFormat;
	VkExtent2D						swapChainExtent;
	std::vector<VkImageView>		swapChainImageViews;
	std::vector<VkSemaphore>		imageAvailableSemaphores;
	std::vector<VkSemaphore>		renderFinishedSemaphores;
	std::vector<VkFence>			inFlightFences;
	VkRenderPass					renderPass;
	VkImage							depthImage;
	VkDeviceMemory					depthImageMemory;
	VkImageView						depthImageView;
	std::vector<VkFramebuffer>		swapChainFramebuffers;
//---- Copy --------------------------------------------------------------------
	VkDevice						copyDevice;
	VkPhysicalDevice				copyPhysicalDevice;
	VulkanCommandPool				*copyCommandPool;
	VkCommandBuffer					*copyCommandBuffers;

//**** PRIVATE METHODS *********************************************************
//---- Creation ----------------------------------------------------------------
	/**
	 * @brief Create swap chain images, image format and extend.
	 */
	void	createSwapChain(void);
	/**
	 * @brief Create swap chain image views.
	 */
	void	createImageViews(void);
	/**
	 * @brief Create sync objects.
	 */
	void	createSyncObjects(void);
	/**
	 * @brief Create render pass.
	 */
	void	createRenderPass(void);
	/**
	 * @brief Create depth image, memory and image view.
	 */
	void	createDepthResources(void);
	/**
	 * @brief Create frame buffers.
	 */
	void	createFramebuffers(void);

//---- Swap chain --------------------------------------------------------------
	/**
	 * @brief Destroy vulkan swap chain.
	 */
	void	destroySwapChain(void);

	void	getShaderInfo(
				VkPipelineLayout &pipelineLayout,
				VkPipeline &graphicsPipeline,
				std::vector<VkDescriptorSet> &descriptorSets,
				Shader &shader);
};

//**** FUNCTIONS ***************************************************************

#endif
