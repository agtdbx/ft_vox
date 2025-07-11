#ifndef SHADER_HPP
# define SHADER_HPP

# include <define.hpp>
# include <engine/engine.hpp>
# include <engine/window/Window.hpp>
# include <engine/textures/TextureManager.hpp>
# include <engine/shader/ShaderParam.hpp>

# include <string>
# include <fstream>
# include <gmath.hpp>

enum DepthMode
{
	DEPTH_DISABLE = 0b00,
	DEPTH_READ = 0b01,
	DEPTH_WRITE = 0b10,
	DEPTH_READ_WRITE = 0b11,
};

enum FaceCulling
{
	FCUL_NONE,
	FCUL_CLOCK,
	FCUL_COUNTER,
};

enum DrawMode
{
	DRAW_POLYGON,
	DRAW_LINE,
	DRAW_POINT,
};

enum AlphaMode
{
	ALPHA_OFF,
	ALPHA_ON,
};

//**** STATIC DEFINE FUNCTIONS *************************************************

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
	 * @param depthMode How do depth test. None, read, write or read_write.
	 * @param faceCulling How do face culling. Clock wise, counter or disable it.
	 * @param drawMode How to draw triangle. Point, line or polygone (fill).
	 * @param alphaMode How do alpha. On or off.
	 * @param vertexPath Path to compile vertex shader file.
	 * @param fragmentPath Path to compile fragment shader file.
	 */
	template<typename VertexType>
	void	init(Engine &engine,
				DepthMode depthMode, FaceCulling faceCulling, DrawMode drawMode, AlphaMode alphaMode,
				std::string vertexPath, std::string fragmentPath)
	{
		VkDevice	device = engine.context.getDevice();

		this->createDescriptorSetLayout(device);
		this->createGraphicsPipeline<VertexType>(device, engine.window, vertexPath, fragmentPath,
										depthMode, faceCulling, drawMode, alphaMode);
	}
	/**
	 * @brief Init shader from parameters.
	 *
	 * @param engine The engine struct.
	 * @param depthMode How do depth test. None, read, write or read_write.
	 * @param faceCulling How do face culling. Clock wise, counter or disable it.
	 * @param drawMode How do draw triangle. Point, line or polygone (fill).
	 * @param alphaMode How do alpha. On or off.
	 * @param vertexPath Path to compile vertex shader file.
	 * @param fragmentPath Path to compile fragment shader file.
	 * @param bufferInfos Vector of buffer info.
	 */
	template<typename VertexType>
	void	init(Engine &engine,
				DepthMode depthMode, FaceCulling faceCulling, DrawMode drawMode, AlphaMode alphaMode,
				std::string vertexPath, std::string fragmentPath,
				const std::vector<BufferInfo> &bufferInfos)
	{
		VkDevice	device = engine.context.getDevice();

		this->bufferInfos = bufferInfos;

		this->createDescriptorSetLayout(device);
		this->createGraphicsPipeline<VertexType>(device, engine.window, vertexPath, fragmentPath,
										depthMode, faceCulling, drawMode, alphaMode);
	}
	/**
	 * @brief Init shader from parameters.
	 *
	 * @param engine The engine struct.
	 * @param depthMode How do depth test. None, read, write or read_write.
	 * @param faceCulling How do face culling. Clock wise, counter or disable it.
	 * @param drawMode How do draw triangle. Point, line or polygone (fill).
	 * @param alphaMode How do alpha. On or off.
	 * @param vertexPath Path to compile vertex shader file.
	 * @param fragmentPath Path to compile fragment shader file.
	 * @param bufferInfos Vector of buffer info.
	 * @param imageInfos Vector of image info.
	 */
	template<typename VertexType>
	void	init(Engine &engine,
				DepthMode depthMode, FaceCulling faceCulling, DrawMode drawMode, AlphaMode alphaMode,
				std::string vertexPath, std::string fragmentPath,
				const std::vector<BufferInfo> &bufferInfos,
				const std::vector<ImageInfo> &imageInfos)
	{
		VkDevice	device = engine.context.getDevice();

		this->bufferInfos = bufferInfos;
		this->imageInfos = imageInfos;

		this->createDescriptorSetLayout(device);
		this->createGraphicsPipeline<VertexType>(device, engine.window, vertexPath, fragmentPath,
										depthMode, faceCulling, drawMode, alphaMode);
	}
	/**
	 * @brief Init a shaderParam
	 *
	 * @param engine The engine struct.
	 * @param shaderParam The shaderParam to init.
	 */
	void	initShaderParam(
				Engine &engine,
				ShaderParam &shaderParam);
	/**
	 * @brief Init a shaderParam
	 *
	 * @param engine The engine struct.
	 * @param shaderParam The shaderParam to init.
	 * @param imageIds Vector of image id to used in shader.
	 */
	void	initShaderParam(
				Engine &engine,
				ShaderParam &shaderParam,
				const std::vector<std::string> &imageIds);
	/**
	 * @brief Destroy vulkan's allocate attributs.
	 *
	 * @param engine The engine struct.
	 */
	void	destroy(Engine &engine);

//**** STATIC METHODS **********************************************************

private:
//**** PRIVATE ATTRIBUTS *******************************************************
	std::vector<BufferInfo>			bufferInfos;
	std::vector<ImageInfo>			imageInfos;
	VkDescriptorSetLayout			descriptorSetLayout;
	VkPipelineLayout				pipelineLayout;
	VkPipeline						graphicsPipeline;

//**** PRIVATE METHODS *********************************************************
	/**
	 * @brief Create descriptor set layout.
	 *
	 * @param device The device of VulkanContext class.
	 */
	void	createDescriptorSetLayout(VkDevice device);
	/**
	 * @brief Create graphic pipeline.
	 *
	 * @param device The device of VulkanContext class.
	 * @param window The Window class.
	 * @param vertexPath Path to compile vertex shader file.
	 * @param fragmentPath Path to compile fragment shader file.
	 * @param depthMode How do depth test. None, read, write or read_write.
	 * @param faceCulling How do face culling. Clock wise, counter or disable it.
	 * @param drawMode How do draw triangle. Point, line or polygone (fill).
	 * @param alphaMode How do alpha. On or off.
	 */
	template<typename VertexType>
	void	createGraphicsPipeline(
				VkDevice device, Window &window,
				std::string vertexPath, std::string fragmentPath,
				DepthMode depthMode, FaceCulling faceCulling, DrawMode drawMode, AlphaMode alphaMode)
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
		if (drawMode == DRAW_POLYGON)
			rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		else if (drawMode == DRAW_LINE)
			rasterizer.polygonMode = VK_POLYGON_MODE_LINE;
		else
			rasterizer.polygonMode = VK_POLYGON_MODE_POINT;
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
		if (alphaMode == ALPHA_OFF)
		{
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
		}
		else
		{
			colorBlendAttachment.blendEnable = VK_TRUE;
			colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
			colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
			colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
			colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
			colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
			colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
			colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
														VK_COLOR_COMPONENT_G_BIT |
														VK_COLOR_COMPONENT_B_BIT |
														VK_COLOR_COMPONENT_A_BIT;
		}

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

		if (depthMode & DEPTH_READ)
			depthStencil.depthTestEnable = VK_TRUE;
		else
			depthStencil.depthTestEnable = VK_FALSE;
		if (depthMode & DEPTH_WRITE)
			depthStencil.depthWriteEnable = VK_TRUE;
		else
			depthStencil.depthWriteEnable = VK_FALSE;

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

#endif
