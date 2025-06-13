#include <engine/textures/TextureManager.hpp>

#include <engine/engine.hpp>
#include <engine/vulkan/VulkanUtils.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <cstring>
#include <stdexcept>

//**** STATIC FUNCTIONS DEFINE *************************************************
//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------

TextureManager::TextureManager(void)
{

}

//---- Destructor --------------------------------------------------------------

TextureManager::~TextureManager()
{
	std::unordered_map<std::string, Texture>::iterator it = this->textures.begin();

	while (it != this->textures.end())
	{
		stbi_image_free(it->second.pixels);
		it++;
	}

	this->textures.clear();
}

//**** ACCESSORS ***************************************************************
//---- Getters -----------------------------------------------------------------

const Texture	*TextureManager::getTexture(std::string id) const
{
	std::unordered_map<std::string, Texture>::const_iterator it = this->textures.find(id);

	if (it == this->textures.end())
		return (NULL);

	return (&it->second);
}


const Image	*TextureManager::getImage(std::string id) const
{
	std::unordered_map<std::string, Image>::const_iterator it = this->images.find(id);

	if (it == this->images.end())
		return (NULL);

	return (&it->second);
}

//---- Setters -----------------------------------------------------------------
//---- Operators ---------------------------------------------------------------
//**** PUBLIC METHODS **********************************************************

void	TextureManager::addTexture(std::string id, std::string texturePath)
{
	std::unordered_map<std::string, Texture>::const_iterator it = this->textures.find(id);

	if (it != this->textures.end())
		throw std::runtime_error("Texture error : id '" + id + "' is already used");

	Texture	texture;

	texture.pixels = stbi_load(texturePath.c_str(), &texture.width, &texture.height,
								&texture.channels, STBI_rgb_alpha);

	if (!texture.pixels)
		throw std::runtime_error("Texture error : can open file '" + texturePath + "'");

	texture.imageSize = texture.width * texture.height * 4;

	this->textures[id] = texture;
}


void	TextureManager::createImage(std::string imageId, std::string textureId, Engine &engine)
{
	std::unordered_map<std::string, Image>::iterator itImg = this->images.find(imageId);

	if (itImg != this->images.end())
		throw std::runtime_error("Image error : id '" + imageId + "' is already used");

	std::unordered_map<std::string, Texture>::iterator itTex = this->textures.find(textureId);

	if (itTex == this->textures.end())
		throw std::runtime_error("Image error : texture id '" + textureId + "' not found");

	VkDevice device = engine.context.getDevice();
	VkPhysicalDevice physicalDevice = engine.context.getPhysicalDevice();
	Image	image;

	this->createTextureImage(device, physicalDevice, engine.commandPool,
								itTex->second, image.image, image.memory);
	this->createTextureImageView(device, image.image, image.view);
	this->createTextureSampler(device, physicalDevice, image.sampler);

	this->images[imageId] = image;
}


void	TextureManager::createAllImages(Engine &engine)
{
	std::unordered_map<std::string, Texture>::iterator itTex = this->textures.begin();

	while (itTex != this->textures.end())
	{
		const std::string &imageId = itTex->first;
		std::unordered_map<std::string, Image>::iterator itImg = this->images.find(imageId);

		if (itImg != this->images.end())
			throw std::runtime_error("Image error : id '" + imageId + "' is already used");

		VkDevice device = engine.context.getDevice();
		VkPhysicalDevice physicalDevice = engine.context.getPhysicalDevice();
		Image	image;

		this->createTextureImage(device, physicalDevice, engine.commandPool,
									itTex->second, image.image, image.memory);
		this->createTextureImageView(device, image.image, image.view);
		this->createTextureSampler(device, physicalDevice, image.sampler);

		this->images[imageId] = image;

		itTex++;
	}
}


void	TextureManager::destroyImages(VkDevice device)
{
	std::unordered_map<std::string, Image>::iterator it = this->images.begin();

	while (it != this->images.end())
	{
		vkDestroySampler(device, it->second.sampler, nullptr);
		vkDestroyImageView(device, it->second.view, nullptr);
		vkDestroyImage(device, it->second.image, nullptr);
		vkFreeMemory(device, it->second.memory, nullptr);
		it++;
	}

	this->images.clear();
}

//**** STATIC METHODS **********************************************************
//**** PRIVATE METHODS *********************************************************

void	TextureManager::createTextureImage(
							VkDevice device, VkPhysicalDevice physicalDevice,
							const VulkanCommandPool &commandPool,
							Texture &texture, VkImage &image, VkDeviceMemory &memory)
{
	// Create temporary texture buffer
	VkBuffer		stagingBuffer;
	VkDeviceMemory	stagingBufferMemory;

	createVulkanBuffer(device, physicalDevice,
						texture.imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
						VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
						stagingBuffer, stagingBufferMemory);

	// Copy image into temporary texture buffer
	void	*data;
	vkMapMemory(device, stagingBufferMemory, 0, texture.imageSize, 0, &data);
	memcpy(data, texture.pixels, static_cast<size_t>(texture.imageSize));
	vkUnmapMemory(device, stagingBufferMemory);

	// Create texture
	createVulkanImage(
		device, physicalDevice,
		texture.width, texture.height, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, image, memory);

	// Put image into optimize format for copy data in it
	transitionImageLayout(
		commandPool, image, VK_FORMAT_R8G8B8A8_SRGB,
		VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	// Copy temporary buffet into image
	copyBufferToImage(commandPool, stagingBuffer, image, static_cast<uint32_t>(texture.width), static_cast<uint32_t>(texture.height));
	// Put image into optimize format for shader
	transitionImageLayout(
		commandPool, image, VK_FORMAT_R8G8B8A8_SRGB,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	vkDestroyBuffer(device, stagingBuffer, nullptr);
	vkFreeMemory(device, stagingBufferMemory, nullptr);
}


void	TextureManager::createTextureImageView(VkDevice device, VkImage &image, VkImageView &view)
{
	view = createVulkanImageView(
			device, image,
			VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);
}


void	TextureManager::createTextureSampler(
							VkDevice device, VkPhysicalDevice physicalDevice,
							VkSampler &sampler)
{
	VkPhysicalDeviceProperties properties{};
	vkGetPhysicalDeviceProperties(physicalDevice, &properties);

	// Define how texture will be displayed (interpolation or not, repeat or not...)
	VkSamplerCreateInfo samplerInfo{};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	// samplerInfo.magFilter = VK_FILTER_LINEAR;
	// samplerInfo.minFilter = VK_FILTER_LINEAR;
	samplerInfo.magFilter = VK_FILTER_NEAREST;
	samplerInfo.minFilter = VK_FILTER_NEAREST;
	// VK_SAMPLER_ADDRESS_MODE_REPEAT
	// VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT
	// VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
	// VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE
	// VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

	// If anisotropy is high, perf < quality
	samplerInfo.anisotropyEnable = VK_TRUE;
	samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
	// If anisotropy is lower, perf > quality
	// samplerInfo.anisotropyEnable = VK_FALSE;
	// samplerInfo.maxAnisotropy = 1.0f;

	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	// samplerInfo.unnormalizedCoordinates = VK_FALSE; // False : pixel in range [0, 1[ | True : pixel in range [0, width[
	samplerInfo.unnormalizedCoordinates = VK_TRUE; // False : pixel in range [0, 1[ | True : pixel in range [0, width[
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.mipLodBias = 0.0f;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = 0.0f;

	// Create sampler
	if (vkCreateSampler(device, &samplerInfo, nullptr, &sampler) != VK_SUCCESS)
		throw std::runtime_error("Create texture sampler failed");
}

//**** FUNCTIONS ***************************************************************
//**** STATIC FUNCTIONS ********************************************************
