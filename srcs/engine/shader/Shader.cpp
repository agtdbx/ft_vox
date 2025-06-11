#include <engine/shader/Shader.hpp>

#include <stdexcept>
#include <algorithm>
#include <cstring>

//**** STATIC DEFINE FUNCTIONS *************************************************
//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------

Shader::Shader(void)
{
	this->descriptorSetLayout = NULL;
	this->pipelineLayout = NULL;
	this->graphicsPipeline = NULL;
}


Shader::Shader(const Shader &obj)
{
	this->descriptorSetLayout = NULL;
	this->pipelineLayout = NULL;
	this->graphicsPipeline = NULL;
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

//---- Setters -----------------------------------------------------------------
//---- Operators ---------------------------------------------------------------

Shader	&Shader::operator=(const Shader &obj)
{
	if (this == &obj)
		return (*this);

	return (*this);
}

//**** PUBLIC METHODS **********************************************************

void	Shader::initShaderParam(
					Engine &engine,
					ShaderParam &shaderParam,
					const std::vector<std::string> &imageIds)
{
	shaderParam.init(engine, this->descriptorSetLayout, this->bufferInfos, imageIds);
}


void	Shader::destroy(Engine &engine)
{
	VkDevice	device = engine.context.getDevice();

	// Free descriptor layout
	if (this->descriptorSetLayout != NULL)
		vkDestroyDescriptorSetLayout(device, this->descriptorSetLayout, nullptr);

	// Free pipeline
	if (this->graphicsPipeline != NULL)
		vkDestroyPipeline(device, this->graphicsPipeline, nullptr);
	if (this->pipelineLayout != NULL)
		vkDestroyPipelineLayout(device, this->pipelineLayout, nullptr);
}

//**** STATIC METHODS **********************************************************
//**** PRIVATE METHODS *********************************************************

void	Shader::createDescriptorSetLayout(VkDevice device, size_t nbImages)
{
	int	nbUbo = this->bufferInfos.size();

	std::vector<VkDescriptorSetLayoutBinding> bindings(nbUbo + nbImages);

	// Bind uniforms to shaders
	std::vector<VkDescriptorSetLayoutBinding> uboLayoutBindings(nbUbo);
	for (int i = 0; i < nbUbo; i++)
	{
		uboLayoutBindings[i].binding = i;
		uboLayoutBindings[i].descriptorCount = 1;
		if (this->bufferInfos[i].type == BUFFER_UBO)
			uboLayoutBindings[i].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		else // SSBO
			uboLayoutBindings[i].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		uboLayoutBindings[i].pImmutableSamplers = nullptr; // Optional
		uboLayoutBindings[i].stageFlags = 0;
		if (this->bufferInfos[i].stage & STAGE_COMPUTE)
			uboLayoutBindings[i].stageFlags = uboLayoutBindings[i].stageFlags | VK_SHADER_STAGE_COMPUTE_BIT;
		if (this->bufferInfos[i].stage & STAGE_VERTEX)
			uboLayoutBindings[i].stageFlags = uboLayoutBindings[i].stageFlags | VK_SHADER_STAGE_VERTEX_BIT;
		if (this->bufferInfos[i].stage & STAGE_FRAGMENT)
			uboLayoutBindings[i].stageFlags = uboLayoutBindings[i].stageFlags | VK_SHADER_STAGE_FRAGMENT_BIT;

		bindings[i] = uboLayoutBindings[i];
	}

	// Bind sampler to shaders
	std::vector<VkDescriptorSetLayoutBinding> samplerLayoutBindings(nbImages);
	for (size_t i = 0; i < nbImages; i++)
	{
		samplerLayoutBindings[i].binding = nbUbo + i;
		samplerLayoutBindings[i].descriptorCount = 1;
		samplerLayoutBindings[i].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		samplerLayoutBindings[i].pImmutableSamplers = nullptr;
		samplerLayoutBindings[i].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		bindings[nbUbo + i] = samplerLayoutBindings[i];
	}

	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings = bindings.data();

	if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &this->descriptorSetLayout) != VK_SUCCESS)
		throw std::runtime_error("Create descriptor set layout failed");
}

//**** FUNCTIONS ***************************************************************
//**** STATIC FUNCTIONS ********************************************************

