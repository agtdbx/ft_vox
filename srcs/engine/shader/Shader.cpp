#include <engine/shader/Shader.hpp>

#include <stdexcept>
#include <algorithm>
#include <cstring>

//**** STATIC DEFINE FUNCTIONS *************************************************
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

