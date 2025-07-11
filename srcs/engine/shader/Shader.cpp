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
					ShaderParam &shaderParam)
{
	shaderParam.init(engine, this->descriptorSetLayout,
						this->bufferInfos, this->imageInfos, {});
}


void	Shader::initShaderParam(
					Engine &engine,
					ShaderParam &shaderParam,
					const std::vector<std::string> &imageIds)
{
	shaderParam.init(engine, this->descriptorSetLayout,
						this->bufferInfos, this->imageInfos, imageIds);
}


void	Shader::destroy(Engine &engine)
{
	VkDevice	device = engine.context.getDevice();

	// Free descriptor layout
	if (this->descriptorSetLayout != NULL)
	{
		vkDestroyDescriptorSetLayout(device, this->descriptorSetLayout, nullptr);
		descriptorSetLayout = NULL;
	}

	// Free pipeline
	if (this->graphicsPipeline != NULL)
	{
		vkDestroyPipeline(device, this->graphicsPipeline, nullptr);
		this->graphicsPipeline = NULL;
	}
	if (this->pipelineLayout != NULL)
	{
		vkDestroyPipelineLayout(device, this->pipelineLayout, nullptr);
		this->pipelineLayout = NULL;
	}
}

//**** STATIC METHODS **********************************************************
//**** PRIVATE METHODS *********************************************************

void	Shader::createDescriptorSetLayout(VkDevice device)
{
	size_t	nbBuffers = this->bufferInfos.size();
	size_t	nbImages = this->imageInfos.size();

	std::vector<VkDescriptorSetLayoutBinding> bindings(nbBuffers + nbImages);

	// Bind uniforms to shaders
	std::vector<VkDescriptorSetLayoutBinding> bufferLayoutBindings(nbBuffers);
	for (size_t i = 0; i < nbBuffers; i++)
	{
		bufferLayoutBindings[i].binding = i;
		bufferLayoutBindings[i].descriptorCount = 1;
		if (this->bufferInfos[i].type == BUFFER_UBO)
			bufferLayoutBindings[i].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		else // SSBO
			bufferLayoutBindings[i].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		bufferLayoutBindings[i].pImmutableSamplers = nullptr; // Optional
		bufferLayoutBindings[i].stageFlags = 0;
		if (this->bufferInfos[i].stage & STAGE_COMPUTE)
			bufferLayoutBindings[i].stageFlags = bufferLayoutBindings[i].stageFlags | VK_SHADER_STAGE_COMPUTE_BIT;
		if (this->bufferInfos[i].stage & STAGE_VERTEX)
			bufferLayoutBindings[i].stageFlags = bufferLayoutBindings[i].stageFlags | VK_SHADER_STAGE_VERTEX_BIT;
		if (this->bufferInfos[i].stage & STAGE_FRAGMENT)
			bufferLayoutBindings[i].stageFlags = bufferLayoutBindings[i].stageFlags | VK_SHADER_STAGE_FRAGMENT_BIT;

		bindings[i] = bufferLayoutBindings[i];
	}

	// Bind sampler to shaders
	std::vector<VkDescriptorSetLayoutBinding> samplerLayoutBindings(nbImages);
	for (size_t i = 0; i < nbImages; i++)
	{
		samplerLayoutBindings[i].binding = nbBuffers + i;
		samplerLayoutBindings[i].descriptorCount = 1;
		samplerLayoutBindings[i].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		samplerLayoutBindings[i].pImmutableSamplers = nullptr;
		samplerLayoutBindings[i].stageFlags = 0;
		if (this->imageInfos[i].stage & STAGE_COMPUTE)
			samplerLayoutBindings[i].stageFlags = samplerLayoutBindings[i].stageFlags | VK_SHADER_STAGE_COMPUTE_BIT;
		if (this->imageInfos[i].stage & STAGE_VERTEX)
			samplerLayoutBindings[i].stageFlags = samplerLayoutBindings[i].stageFlags | VK_SHADER_STAGE_VERTEX_BIT;
		if (this->imageInfos[i].stage & STAGE_FRAGMENT)
			samplerLayoutBindings[i].stageFlags = samplerLayoutBindings[i].stageFlags | VK_SHADER_STAGE_FRAGMENT_BIT;

		bindings[nbBuffers + i] = samplerLayoutBindings[i];
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

