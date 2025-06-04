#include <engine/shader/Shader.hpp>

#include <stdexcept>
#include <algorithm>
#include <cstring>
#include <fstream>

//**** STATIC FUNCTIONS DEFINE *************************************************

static std::vector<char>	readFile(const std::string &filename);
static VkShaderModule	createShaderModule(VkDevice device, const std::vector<char> &code);
static std::vector<const Image *>	getImages(const TextureManager &textureManager, const std::vector<std::string> &imageIds);

//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------

Shader::Shader(void)
{
	this->uboStructSize = 0;
	this->descriptorSetLayout = NULL;
	this->pipelineLayout = NULL;
	this->graphicsPipeline = NULL;
	this->descriptorPool = NULL;
}


Shader::Shader(const Shader &obj)
{
	this->uboStructSize = 0;
	this->descriptorSetLayout = NULL;
	this->pipelineLayout = NULL;
	this->graphicsPipeline = NULL;
	this->descriptorPool = NULL;
}

//---- Destructor --------------------------------------------------------------

Shader::~Shader()
{
}

//**** ACCESSORS ***************************************************************
//---- Getters -----------------------------------------------------------------

VkPipelineLayout	Shader::getPipelineLayout(void)
{
	return (this->pipelineLayout);
}


VkPipeline	Shader::getGraphicsPipeline(void)
{
	return (this->graphicsPipeline);
}


std::vector<VkDescriptorSet>	&Shader::getDescriptorSets(void)
{
	return (this->descriptorSets);
}

//---- Setters -----------------------------------------------------------------
//---- Operators ---------------------------------------------------------------

Shader	&Shader::operator=(const Shader &obj)
{
	if (this == &obj)
		return (*this);

	return (*this);
}

//**** PUBLIC METHODS **********************************************************

void	Shader::init(
					Engine &engine, size_t uboStructSize, FaceCulling faceCulling,
					std::string vertexPath, std::string fragmentPath)
{
	VkDevice	device = engine.context.getDevice();
	VkPhysicalDevice	physicalDevice = engine.context.getPhysicalDevice();

	this->uboStructSize = uboStructSize;

	this->createDescriptorSetLayout(device, 0);
	this->createGraphicsPipeline(device, engine.window, vertexPath, fragmentPath,
									faceCulling);
	this->createUniformBuffers(device, physicalDevice);
	this->createDescriptorPool(device, 0);
	this->createDescriptorSets(device, {});
}


void	Shader::init(
					Engine &engine, size_t uboStructSize, FaceCulling faceCulling,
					std::string vertexPath, std::string fragmentPath,
					const std::vector<std::string> &imageIds)
{
	VkDevice	device = engine.context.getDevice();
	VkPhysicalDevice	physicalDevice = engine.context.getPhysicalDevice();

	this->uboStructSize = uboStructSize;

	std::vector<const Image *> images = getImages(engine.textureManager, imageIds);

	this->createDescriptorSetLayout(device, images.size());
	this->createGraphicsPipeline(device, engine.window, vertexPath, fragmentPath,
									faceCulling);
	this->createUniformBuffers(device, physicalDevice);
	this->createDescriptorPool(device, images.size());
	this->createDescriptorSets(device, images);
}


void	Shader::destroy(Engine &engine)
{
	VkDevice	device = engine.context.getDevice();

	// Free uniforms buffers
	size_t nbUBO = this->uniformBuffers.size();
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT && i < nbUBO; i++)
	{
		if (this->uniformBuffers[i])
			vkDestroyBuffer(device, this->uniformBuffers[i], nullptr);
		if (this->uniformBuffersMemory[i])
			vkFreeMemory(device, this->uniformBuffersMemory[i], nullptr);
	}

	// Free descriptor pool
	if (this->descriptorPool != NULL)
		vkDestroyDescriptorPool(device, this->descriptorPool, nullptr);

	// Free descriptor layout
	if (this->descriptorSetLayout != NULL)
		vkDestroyDescriptorSetLayout(device, this->descriptorSetLayout, nullptr);

	// Free pipeline
	if (this->graphicsPipeline != NULL)
		vkDestroyPipeline(device, this->graphicsPipeline, nullptr);
	if (this->pipelineLayout != NULL)
		vkDestroyPipelineLayout(device, this->pipelineLayout, nullptr);
}


void	Shader::updateUBO(Window &window, void *ubo)
{
	memcpy(this->uniformBuffersMapped[window.getCurrentFrame()], ubo, this->uboStructSize);
}

//**** STATIC METHODS **********************************************************
//**** PRIVATE METHODS *********************************************************

void	Shader::createDescriptorSetLayout(VkDevice device, size_t nbImages)
{
	// Bind uniforms to shaders
	VkDescriptorSetLayoutBinding uboLayoutBinding{};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.pImmutableSamplers = nullptr; // Optional
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	std::vector<VkDescriptorSetLayoutBinding> bindings(1 + nbImages);
	bindings[0] = uboLayoutBinding;

	// Bind sampler to shaders
	std::vector<VkDescriptorSetLayoutBinding> samplerLayoutBindings(nbImages);
	for (size_t i = 0; i < nbImages; i++)
	{
		samplerLayoutBindings[i].binding = 1 + i;
		samplerLayoutBindings[i].descriptorCount = 1;
		samplerLayoutBindings[i].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		samplerLayoutBindings[i].pImmutableSamplers = nullptr;
		samplerLayoutBindings[i].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		bindings[1 + i] = samplerLayoutBindings[i];
	}

	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings = bindings.data();

	if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &this->descriptorSetLayout) != VK_SUCCESS)
		throw std::runtime_error("Create descriptor set layout failed");
}


void	Shader::createGraphicsPipeline(
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
	auto bindingDescription = Vertex::getBindingDescription();
	auto attributeDescriptions = Vertex::getAttributeDescriptions();

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


void	Shader::createUniformBuffers(VkDevice device, VkPhysicalDevice physicalDevice)
{
	this->uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
	this->uniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
	this->uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		createVulkanBuffer(device, physicalDevice,
							this->uboStructSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
							VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
							this->uniformBuffers[i], this->uniformBuffersMemory[i]);
		vkMapMemory(device, this->uniformBuffersMemory[i], 0, this->uboStructSize, 0, &this->uniformBuffersMapped[i]);
	}
}


void	Shader::createDescriptorPool(VkDevice device, size_t nbImages)
{
	uint32_t maxFramesInFlight = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

	std::vector<VkDescriptorPoolSize> poolSizes(1 + nbImages);
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = maxFramesInFlight;
	for (size_t i = 0; i < nbImages; i++)
	{
		poolSizes[1 + i].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		poolSizes[1 + i].descriptorCount = maxFramesInFlight;
	}

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

	if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &this->descriptorPool) != VK_SUCCESS)
		throw std::runtime_error("Create descriptor pool failed");
}


void	Shader::createDescriptorSets(
					VkDevice device,
					const std::vector<const Image *> &images)
{
	std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, this->descriptorSetLayout);
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = this->descriptorPool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
	allocInfo.pSetLayouts = layouts.data();

	this->descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
	if (vkAllocateDescriptorSets(device, &allocInfo, this->descriptorSets.data()) != VK_SUCCESS)
		throw std::runtime_error("Allocate descriptor sets failed");

	// Get images
	uint32_t nbImages = images.size();

	// Create images struct
	std::vector<VkDescriptorImageInfo>	imagesInfo(nbImages);
	for (uint32_t i = 0; i < nbImages; i++)
	{
		imagesInfo[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imagesInfo[i].imageView = images[i]->view;
		imagesInfo[i].sampler = images[i]->sampler;
	}

	// Init sets for each frame
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = this->uniformBuffers[i];
		bufferInfo.offset = 0;
		bufferInfo.range = this->uboStructSize;

		std::vector<VkWriteDescriptorSet> descriptorWrites(1 + nbImages);

		descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[0].dstSet = this->descriptorSets[i];
		descriptorWrites[0].dstBinding = 0;
		descriptorWrites[0].dstArrayElement = 0;
		descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrites[0].descriptorCount = 1;
		descriptorWrites[0].pBufferInfo = &bufferInfo;

		for (uint32_t j = 0; j < nbImages; j++)
		{
			uint32_t	id = j + 1;

			descriptorWrites[id].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[id].dstSet = this->descriptorSets[i];
			descriptorWrites[id].dstBinding = id;
			descriptorWrites[id].dstArrayElement = 0;
			descriptorWrites[id].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrites[id].descriptorCount = 1;
			descriptorWrites[id].pImageInfo = &imagesInfo[j];
		}

		vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}
}

//**** FUNCTIONS ***************************************************************
//**** STATIC FUNCTIONS ********************************************************

static std::vector<char>	readFile(const std::string &filename)
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


static VkShaderModule	createShaderModule(VkDevice device, const std::vector<char> &code)
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


static std::vector<const Image *>	getImages(const TextureManager &textureManager, const std::vector<std::string> &imageIds)
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
