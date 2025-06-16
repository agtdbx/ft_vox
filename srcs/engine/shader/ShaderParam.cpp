#include <engine/shader/ShaderParam.hpp>

#include <stdexcept>
#include <algorithm>
#include <cstring>

//**** STATIC DEFINE FUNCTIONS *************************************************

static inline std::vector<const Image *>	getImages(const TextureManager &textureManager, const std::vector<std::string> &imageIds);

//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------

ShaderParam::ShaderParam(void)
{
	this->descriptorPool = NULL;
}


ShaderParam::ShaderParam(const ShaderParam &obj)
{
	this->descriptorPool = NULL;
}

//---- Destructor --------------------------------------------------------------

ShaderParam::~ShaderParam()
{
}

//**** ACCESSORS ***************************************************************
//---- Getters -----------------------------------------------------------------

std::vector<VkDescriptorSet>	&ShaderParam::getDescriptorSets(void)
{
	return (this->descriptorSets);
}

//---- Setters -----------------------------------------------------------------
//---- Operators ---------------------------------------------------------------

ShaderParam	&ShaderParam::operator=(const ShaderParam &obj)
{
	if (this == &obj)
		return (*this);

	return (*this);
}

//**** PUBLIC METHODS **********************************************************

void	ShaderParam::init(
			Engine &engine,
			VkDescriptorSetLayout descriptorSetLayout,
			const std::vector<BufferInfo> &bufferInfos,
			const std::vector<ImageInfo> &imageInfos,
			const std::vector<std::string> &imageIds)
{
	VkDevice	device = engine.context.getDevice();
	VkPhysicalDevice	physicalDevice = engine.context.getPhysicalDevice();

	this->bufferInfos = bufferInfos;
	this->imageInfos = imageInfos;

	if (this->imageInfos.size() != imageIds.size())
		throw std::runtime_error("Invalid number of image according to shader parameters");

	std::vector<const Image *> images = getImages(engine.textureManager, imageIds);

	this->createUniformBuffers(device, physicalDevice);
	this->createDescriptorPool(device, images.size());
	this->createDescriptorSets(device, descriptorSetLayout, images);
}


void	ShaderParam::destroy(Engine &engine)
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
}


void	ShaderParam::updateBuffer(Window &window, void *buffer, int bufferId)
{
	int	id = window.getCurrentFrame() * this->bufferInfos.size() + bufferId;
	memcpy(this->uniformBuffersMapped[id], buffer, this->bufferInfos[bufferId].size);
}

//**** STATIC METHODS **********************************************************
//**** PRIVATE METHODS *********************************************************

void	ShaderParam::createUniformBuffers(VkDevice device, VkPhysicalDevice physicalDevice)
{
	int	nbUbo = this->bufferInfos.size();
	int	nbBuffers = MAX_FRAMES_IN_FLIGHT * nbUbo;

	this->uniformBuffers.resize(nbBuffers);
	this->uniformBuffersMemory.resize(nbBuffers);
	this->uniformBuffersMapped.resize(nbBuffers);

	int	bufferId;
	int	bufferOffset = 0;
	VkBufferUsageFlags	bufferUsage;
	for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		for (int j = 0; j < nbUbo; j++)
		{
			if (this->bufferInfos[j].type == BUFFER_UBO)
				bufferUsage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
			else // SSBO
				bufferUsage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
			bufferId = bufferOffset + j;
			createVulkanBuffer(device, physicalDevice,
								this->bufferInfos[j].size, bufferUsage,
								VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
								this->uniformBuffers[bufferId], this->uniformBuffersMemory[bufferId]);
			vkMapMemory(device, this->uniformBuffersMemory[bufferId], 0, this->bufferInfos[j].size, 0, &this->uniformBuffersMapped[bufferId]);
		}
		bufferOffset += nbUbo;
	}
}


void	ShaderParam::createDescriptorPool(VkDevice device, size_t nbImages)
{
	uint32_t maxFramesInFlight = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
	size_t nbUbo = this->bufferInfos.size();

	std::vector<VkDescriptorPoolSize> poolSizes(nbUbo + nbImages);
	for (size_t i = 0; i < nbUbo; i++)
	{
		if (this->bufferInfos[i].type == BUFFER_UBO)
			poolSizes[i].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		else // SSBO
			poolSizes[i].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
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


void	ShaderParam::createDescriptorSets(
					VkDevice device,
					VkDescriptorSetLayout descriptorSetLayout,
					const std::vector<const Image *> &images)
{
	std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, descriptorSetLayout);
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = this->descriptorPool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
	allocInfo.pSetLayouts = layouts.data();

	this->descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
	if (vkAllocateDescriptorSets(device, &allocInfo, this->descriptorSets.data()) != VK_SUCCESS)
		throw std::runtime_error("Allocate descriptor sets failed");

	// Get images
	uint32_t nbUbo = this->bufferInfos.size();
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
			buffersInfo[j].range = this->bufferInfos[j].size;
		}

		std::vector<VkWriteDescriptorSet> descriptorWrites(nbUbo + nbImages);
		for (uint32_t j = 0; j < nbUbo; j++)
		{
			descriptorWrites[j].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[j].dstSet = this->descriptorSets[i];
			descriptorWrites[j].dstBinding = j;
			descriptorWrites[j].dstArrayElement = 0;
			if (this->bufferInfos[j].type == BUFFER_UBO)
				descriptorWrites[j].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			else // SSBO
				descriptorWrites[j].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
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
