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
	this->descriptorPool = NULL;
}


Shader::Shader(const Shader &obj)
{
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

void	Shader::destroy(Engine &engine)
{
	VkDevice	device = engine.context.getDevice();

	// Free uniforms buffers
	size_t nbBuffers = this->uniformBuffers.size();
	for (size_t i = 0; i < nbBuffers; i++)
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


void	Shader::updateUBO(Window &window, void *ubo, int uboId)
{
	int	bufferId = window.getCurrentFrame() * this->uboTypes.size() + uboId;
	memcpy(this->uniformBuffersMapped[bufferId], ubo, this->uboTypes[uboId].size);
}

//**** STATIC METHODS **********************************************************
//**** PRIVATE METHODS *********************************************************

void	Shader::createDescriptorSetLayout(VkDevice device, size_t nbImages)
{
	int	nbUbo = this->uboTypes.size();

	std::vector<VkDescriptorSetLayoutBinding> bindings(nbUbo + nbImages);

	// Bind uniforms to shaders
	std::vector<VkDescriptorSetLayoutBinding> uboLayoutBindings(nbUbo);
	for (int i = 0; i < nbUbo; i++)
	{
		uboLayoutBindings[i].binding = i;
		uboLayoutBindings[i].descriptorCount = 1;
		uboLayoutBindings[i].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBindings[i].pImmutableSamplers = nullptr; // Optional
		if (this->uboTypes[i].location == UBO_VERTEX)
			uboLayoutBindings[i].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		else
			uboLayoutBindings[i].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

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


void	Shader::createUniformBuffers(VkDevice device, VkPhysicalDevice physicalDevice)
{
	int	nbUbo = this->uboTypes.size();
	int	nbBuffers = MAX_FRAMES_IN_FLIGHT * nbUbo;

	this->uniformBuffers.resize(nbBuffers);
	this->uniformBuffersMemory.resize(nbBuffers);
	this->uniformBuffersMapped.resize(nbBuffers);

	int	bufferId;
	int	bufferOffset = 0;
	for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		for (int j = 0; j < nbUbo; j++)
		{
			bufferId = bufferOffset + j;
			createVulkanBuffer(device, physicalDevice,
								this->uboTypes[j].size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
								VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
								this->uniformBuffers[bufferId], this->uniformBuffersMemory[bufferId]);
			vkMapMemory(device, this->uniformBuffersMemory[bufferId], 0, this->uboTypes[j].size, 0, &this->uniformBuffersMapped[bufferId]);
		}
		bufferOffset += nbUbo;
	}
}


void	Shader::createDescriptorPool(VkDevice device, size_t nbImages)
{
	uint32_t maxFramesInFlight = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
	size_t nbUbo = this->uboTypes.size();

	std::vector<VkDescriptorPoolSize> poolSizes(nbUbo + nbImages);
	for (size_t i = 0; i < nbUbo; i++)
	{
		poolSizes[i].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSizes[i].descriptorCount = maxFramesInFlight;
	}
	for (size_t i = 0; i < nbImages; i++)
	{
		poolSizes[nbUbo + i].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		poolSizes[nbUbo + i].descriptorCount = maxFramesInFlight;
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
	uint32_t nbUbo = this->uboTypes.size();
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
		// Create ubo struct
		std::vector<VkDescriptorBufferInfo>	buffersInfo(nbUbo);
		for (uint32_t j = 0; j < nbUbo; j++)
		{
			buffersInfo[j].buffer = this->uniformBuffers[i * nbUbo + j];
			buffersInfo[j].offset = 0;
			buffersInfo[j].range = this->uboTypes[j].size;
		}

		std::vector<VkWriteDescriptorSet> descriptorWrites(nbUbo + nbImages);
		for (uint32_t j = 0; j < nbUbo; j++)
		{
			descriptorWrites[j].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[j].dstSet = this->descriptorSets[i];
			descriptorWrites[j].dstBinding = j;
			descriptorWrites[j].dstArrayElement = 0;
			descriptorWrites[j].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[j].descriptorCount = 1;
			descriptorWrites[j].pBufferInfo = &buffersInfo[j];
		}

		for (uint32_t j = 0; j < nbImages; j++)
		{
			uint32_t	id = nbUbo + j;

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

