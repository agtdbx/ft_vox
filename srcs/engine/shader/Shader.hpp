#ifndef SHADER_HPP
# define SHADER_HPP

# include <define.hpp>
# include <engine/engine.hpp>
# include <engine/window/Window.hpp>
# include <engine/textures/TextureManager.hpp>

# include <string>
# include <fstream>
# include <gmath.hpp>

enum FaceCulling
{
	FCUL_NONE,
	FCUL_CLOCK,
	FCUL_COUNTER,
};


enum UBOLocation
{
	UBO_VERTEX,
	UBO_FRAGMENT,
};


struct UBOType
{
	size_t		size;
	UBOLocation	location;
};


//**** STATIC DEFINE FUNCTIONS *************************************************

static inline std::vector<const Image *>	getImages(const TextureManager &textureManager, const std::vector<std::string> &imageIds);
static inline std::vector<char>	readFile(const std::string &filename);
static inline VkShaderModule	createShaderModule(VkDevice device, const std::vector<char> &code);

/**
 * @brief Class for shader with vulkan.
 */
class Shader
{
public:
//**** PUBLIC ATTRIBUTS ********************************************************
//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------
	/**
	 * @brief Default contructor of Shader class.
	 *
	 * @return The default Shader.
	 */
	Shader(void);
	/**
	 * @brief Copy constructor of Shader class.
	 *
	 * @param obj The Shader to copy.
	 *
	 * @return The Shader copied from parameter.
	 */
	Shader(const Shader &obj);

//---- Destructor --------------------------------------------------------------
	/**
	 * @brief Destructor of Shader class.
	 */
	~Shader();

//**** ACCESSORS ***************************************************************
//---- Getters -----------------------------------------------------------------
	/**
	 * @brief Getter of pipeline layout.
	 *
	 * @return The pipeline layout.
	 */
	VkPipelineLayout	getPipelineLayout(void);
	/**
	 * @brief Getter of graphic pipeline.
	 *
	 * @return The graphic pipeline.
	 */
	VkPipeline	getGraphicsPipeline(void);
	/**
	 * @brief Getter of descriptor sets.
	 *
	 * @return The descriptor sets.
	 */
	std::vector<VkDescriptorSet>	&getDescriptorSets(void);

//---- Setters -----------------------------------------------------------------
//---- Operators ---------------------------------------------------------------
	/**
	 * @brief Copy operator of Shader class.
	 *
	 * @param obj The Shader to copy.
	 *
	 * @return The Shader copied from parameter.
	 */
	Shader	&operator=(const Shader &obj);

//**** PUBLIC METHODS **********************************************************
	/**
	 * @brief Init shader from parameters.
	 *
	 * @param engine The engine struct.
	 * @param faceCulling How do face culling. Clock wise, counter or disable it.
	 * @param vertexPath Path to compile vertex shader file.
	 * @param fragmentPath Path to compile fragment shader file.
	 */
	template<typename VertexType>
	void	init(
				Engine &engine, FaceCulling faceCulling,
				std::string vertexPath, std::string fragmentPath)
	{
		VkDevice	device = engine.context.getDevice();
		VkPhysicalDevice	physicalDevice = engine.context.getPhysicalDevice();

		this->createDescriptorSetLayout(device, 0);
		this->createGraphicsPipeline<VertexType>(device, engine.window, vertexPath, fragmentPath,
										faceCulling);
		this->createUniformBuffers(device, physicalDevice);
		this->createDescriptorPool(device, 0);
		this->createDescriptorSets(device, {});
	}
	/**
	 * @brief Init shader from parameters.
	 *
	 * @param engine The engine struct.
	 * @param faceCulling How do face culling. Clock wise, counter or disable it.
	 * @param vertexPath Path to compile vertex shader file.
	 * @param fragmentPath Path to compile fragment shader file.
	 * @param uboTypes Vector of ubo types.
	 */
	template<typename VertexType>
	void	init(
				Engine &engine, FaceCulling faceCulling,
				std::string vertexPath, std::string fragmentPath,
				const std::vector<UBOType> &uboTypes)
	{
		VkDevice	device = engine.context.getDevice();
		VkPhysicalDevice	physicalDevice = engine.context.getPhysicalDevice();

		this->uboTypes = uboTypes;

		this->createDescriptorSetLayout(device, 0);
		this->createGraphicsPipeline<VertexType>(device, engine.window, vertexPath, fragmentPath,
										faceCulling);
		this->createUniformBuffers(device, physicalDevice);
		this->createDescriptorPool(device, 0);
		this->createDescriptorSets(device, {});
	}
	/**
	 * @brief Init shader from parameters.
	 *
	 * @param engine The engine struct.
	 * @param faceCulling How do face culling. Clock wise, counter or disable it.
	 * @param vertexPath Path to compile vertex shader file.
	 * @param fragmentPath Path to compile fragment shader file.
	 * @param uboTypes Vector of ubo types.
	 * @param imageIds Vector of image id to used in shader.
	 */
	template<typename VertexType>
	void	init(
				Engine &engine, FaceCulling faceCulling,
				std::string vertexPath, std::string fragmentPath,
				const std::vector<UBOType> &uboTypes,
				const std::vector<std::string> &imageIds)
	{
		VkDevice	device = engine.context.getDevice();
		VkPhysicalDevice	physicalDevice = engine.context.getPhysicalDevice();

		this->uboTypes = uboTypes;

		std::vector<const Image *> images = getImages(engine.textureManager, imageIds);

		this->createDescriptorSetLayout(device, images.size());
		this->createGraphicsPipeline<VertexType>(device, engine.window, vertexPath, fragmentPath,
										faceCulling);
		this->createUniformBuffers(device, physicalDevice);
		this->createDescriptorPool(device, images.size());
		this->createDescriptorSets(device, images);
	}
	/**
	 * @brief Destroy vulkan's allocate attributs.
	 *
	 * @param engine The engine struct.
	 */
	void	destroy(Engine &engine);
	/**
	 * @brief Update uniform values used by shader.destroy
	 *
 	 * @param window Window class of the engine.
	 * @param ubo Pointer of uniform values struct used for update.
	 * @param uboId Id of ubo in init vector. Id isn't check for speed, will crash if pass an incorect id.
	 */
	void	updateUBO(Window &window, void *ubo, int uboId);

//**** STATIC METHODS **********************************************************

private:
//**** PRIVATE ATTRIBUTS *******************************************************
	std::vector<UBOType>			uboTypes;
	VkDescriptorSetLayout			descriptorSetLayout;
	VkPipelineLayout				pipelineLayout;
	VkPipeline						graphicsPipeline;
	std::vector<VkBuffer>			uniformBuffers;
	std::vector<VkDeviceMemory>		uniformBuffersMemory;
	std::vector<void*>				uniformBuffersMapped;
	VkDescriptorPool				descriptorPool;
	std::vector<VkDescriptorSet>	descriptorSets;

//**** PRIVATE METHODS *********************************************************
	/**
	 * @brief Create descriptor set layout.
	 *
	 * @param device The device of VulkanContext class.
	 * @param nbImages The number of image used in shader.
	 */
	void	createDescriptorSetLayout(VkDevice device, size_t nbImages);
	/**
	 * @brief Create graphic pipeline.
	 *
	 * @param device The device of VulkanContext class.
	 * @param window The Window class.
	 * @param vertexPath Path to compile vertex shader file.
	 * @param fragmentPath Path to compile fragment shader file.
	 */
	template<typename VertexType>
	void	createGraphicsPipeline(
				VkDevice device, Window &window,
				std::string vertexPath, std::string fragmentPath,
				FaceCulling faceCulling)
	{
		// Read files
		std::vector<char> vertShaderCode = readFile(vertexPath);
		std::vector<char> fragShaderCode = readFile(fragmentPath);

		// Create shaders
		VkShaderModule vertShaderModule = createShaderModule(device, vertShaderCode);
		VkShaderModule fragShaderModule = createShaderModule(device, fragShaderCode);

		// Create vertex shader stage
		VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertShaderStageInfo.module = vertShaderModule;
		vertShaderStageInfo.pName = "main";

		// Create fragment shader stage
		VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
		fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageInfo.module = fragShaderModule;
		fragShaderStageInfo.pName = "main";

		// Create shader pipeline
		VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

		// Define vertex input
		auto bindingDescription = VertexType::getBindingDescription();
		auto attributeDescriptions = VertexType::getAttributeDescriptions();

		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = 1;
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
		vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
		vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

		// Define in which way vertexes will be used (for triangle here)
		VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssembly.primitiveRestartEnable = VK_FALSE;

		// Create the viewport (region of pixel stored)
		VkPipelineViewportStateCreateInfo viewportState{};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.scissorCount = 1;

		// Define the rasterization pipeline
		VkPipelineRasterizationStateCreateInfo rasterizer{};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE; // Make VK_TRUE for disable rasterization
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL; // Set draw mode (fill / line / point)
		rasterizer.lineWidth = 1.0f;
		if (faceCulling == FCUL_NONE)
			rasterizer.cullMode = VK_CULL_MODE_NONE;
		else
		{
			rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
			if (faceCulling == FCUL_CLOCK)
				rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
			else
				rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		}
		rasterizer.depthBiasEnable = VK_FALSE;
		rasterizer.depthBiasConstantFactor = 0.0f; // Optional
		rasterizer.depthBiasClamp = 0.0f; // Optional
		rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

		// Define multisampling
		VkPipelineMultisampleStateCreateInfo multisampling{};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisampling.minSampleShading = 1.0f; // Optional
		multisampling.pSampleMask = nullptr; // Optional
		multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
		multisampling.alphaToOneEnable = VK_FALSE; // Optional

		// Define color blending (by alpha here)
		VkPipelineColorBlendAttachmentState colorBlendAttachment;
		colorBlendAttachment.blendEnable = VK_FALSE;
		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
													VK_COLOR_COMPONENT_G_BIT |
													VK_COLOR_COMPONENT_B_BIT |
													VK_COLOR_COMPONENT_A_BIT;

		// Define blend constants
		VkPipelineColorBlendStateCreateInfo colorBlending{};
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;
		colorBlending.blendConstants[0] = 0.0f; // Optional
		colorBlending.blendConstants[1] = 0.0f; // Optional
		colorBlending.blendConstants[2] = 0.0f; // Optional
		colorBlending.blendConstants[3] = 0.0f; // Optional

		// Define dynamic state
		std::vector<VkDynamicState> dynamicStates = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};

		VkPipelineDynamicStateCreateInfo dynamicState{};
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
		dynamicState.pDynamicStates = dynamicStates.data();

		// Create pipeline layout
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 1;
		pipelineLayoutInfo.pSetLayouts = &this->descriptorSetLayout;
		pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
		pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

		if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &this->pipelineLayout) != VK_SUCCESS)
			throw std::runtime_error("Pipeline layout creation failed");

		VkPipelineDepthStencilStateCreateInfo depthStencil{};
		depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencil.depthTestEnable = VK_TRUE;
		depthStencil.depthWriteEnable = VK_TRUE;
		depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
		depthStencil.depthBoundsTestEnable = VK_FALSE;
		depthStencil.minDepthBounds = 0.0f; // Optional
		depthStencil.maxDepthBounds = 1.0f; // Optional
		depthStencil.stencilTestEnable = VK_FALSE;
		depthStencil.front = {}; // Optional
		depthStencil.back = {}; // Optional

		// Create graphic pipeline
		VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = shaderStages;
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pDepthStencilState = &depthStencil;
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDynamicState = &dynamicState;
		pipelineInfo.layout = pipelineLayout;
		pipelineInfo.renderPass = window.getRenderPass();
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
		pipelineInfo.basePipelineIndex = -1; // Optional

		if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &this->graphicsPipeline) != VK_SUCCESS)
			throw std::runtime_error("Graphics pipeline creation failed");

		// Free shaders
		vkDestroyShaderModule(device, fragShaderModule, nullptr);
		vkDestroyShaderModule(device, vertShaderModule, nullptr);
	}
	/**
	 * @brief Create uniform buffers to store uniform values used by shader.
	 *
	 * @param device The device of VulkanContext class.
	 * @param physicalDevice The physical device of VulkanContext class.
	 */
	void	createUniformBuffers(VkDevice device, VkPhysicalDevice physicalDevice);
	/**
	 * @brief Create descriptor pool.
	 *
	 * @param device The device of VulkanContext class.
	 * @param nbImages The number of image used in shader.
	 */
	void	createDescriptorPool(VkDevice device, size_t nbImages);
	/**
	 * @brief Create descriptor sets.
	 *
	 * @param device The device of VulkanContext class.
	 * @param images The vector of image that will be used in shader.
	 */
	void	createDescriptorSets(
				VkDevice device,
				const std::vector<const Image *> &images);
};

//**** FUNCTIONS ***************************************************************
//**** STATIC FUNCTIONS ********************************************************

static inline std::vector<char>	readFile(const std::string &filename)
{
	// Open file in binary mode and put cursor at the end
	std::ifstream file(filename, std::ios::ate | std::ios::binary);
	if (!file.is_open())
		throw std::runtime_error("Open file '" + filename + "' failed");

	// Get the size of the file
	size_t fileSize = (size_t) file.tellg();
	std::vector<char> buffer(fileSize);

	// Put cursor an the beginning of the file
	file.seekg(0);

	// Read the file
	file.read(buffer.data(), fileSize);
	file.close();

	return (buffer);
}


static inline VkShaderModule	createShaderModule(VkDevice device, const std::vector<char> &code)
{
	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
		throw std::runtime_error("Shader creation failed");

	return (shaderModule);
}


static inline std::vector<const Image *>	getImages(const TextureManager &textureManager, const std::vector<std::string> &imageIds)
{
	std::vector<const Image *>	images;
	for (const std::string &imageId : imageIds)
	{
		const Image *image = textureManager.getImage(imageId);
		if (!image)
			throw std::runtime_error("Invalid image id '" + imageId + "'");
		images.push_back(image);
	}

	return (images);
}

#endif
