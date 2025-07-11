#ifndef TEXTURE_MANAGER_HPP
# define TEXTURE_MANAGER_HPP

# include <define.hpp>
# include <engine/vulkan/VulkanCommandPool.hpp>

# include <string>
# include <unordered_map>


/**
 * @brief Struct for texture loaded from file.
 */
struct Texture
{
	unsigned char	*pixels;
	int				width;
	int				height;
	int				channels;
	VkDeviceSize	imageSize;
};

/**
 * @brief Struct for image create from texture and usable by vulkan.
 */
struct Image
{
	VkImage			image;
	VkDeviceMemory	memory;
	VkImageView		view;
	VkSampler		sampler;
	uint32_t		nbLayer;
};


/**
 * @brief Struct for sampler info.
 */
struct SamplerInfo
{
	// false for interpolate pixel, true for pixelart
	bool	pixelize;
	// false to clamp to edge, true for repeat
	bool	repeat;
	// false for texCoord in range [0.0,1.0[, true for range [0, size[
	bool	intCoordonates;
	// false for quality, true for perf
	bool	perfOverQuality;
};


struct Engine;

/**
 * @brief TextureManager class.
 */
class TextureManager
{
public:
//**** PUBLIC ATTRIBUTS ********************************************************
//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------
	/**
	 * @brief Default contructor of TextureManager class.
	 *
	 * @return The default TextureManager.
	 */
	TextureManager(void);

//---- Destructor --------------------------------------------------------------
	/**
	 * @brief Destructor of TextureManager class.
	 */
	~TextureManager();

//**** ACCESSORS ***************************************************************
//---- Getters -----------------------------------------------------------------
	/**
	 * @brief Get a texture be id.
	 *
	 * @param id The id of the texture.
	 *
	 * @return Pointer of texture with param id, or NULL if no texture match the id.
	 */
	const Texture	*getTexture(std::string id) const;
	/**
	 * @brief Get an image be id.
	 *
	 * @param id The id of the image.
	 *
	 * @return Pointer of image with param id, or NULL if no image match the id.
	 */
	const Image	*getImage(std::string id) const;

//---- Setters -----------------------------------------------------------------
//---- Operators ---------------------------------------------------------------
//**** PUBLIC METHODS **********************************************************
	/**
	 * @brief Load and add a texture from a path to the manager.
	 *
	 * @param id The id of the texture used for get it later. It must be unique.
	 * @param texturePath The path of the texture file.
	 *
	 * @exception Throw a runtime_error if the id is already used of if the file can't be open.
	 */
	void	addTexture(std::string id, std::string texturePath);
	/**
	 * @brief Create an image usable for vulkan.
	 *
	 * @param engine The engine struct.
	 * @param samplerInfo Info for the sampler.
	 * @param imageId Id of the created image.
	 * @param textureId Id of the texture used to create the image.
	 *
	 * @exception Throw an runtime_error if imageId is already used or if textureId isn't exist.
	 */
	void	createImage(
				Engine &engine,
				SamplerInfo samplerInfo,
				std::string imageId,
				std::string textureId);
	/**
	 * @brief Create an image array usable for vulkan.
	 *
	 * @param engine The engine struct.
	 * @param samplerInfo Info for the sampler.
	 * @param imageId Id of the created image.
	 * @param textureIds Vector or texture's id used to create the image array.
	 *
	 * @exception Throw an runtime_error if imageId is already used or if textureId isn't exist.
	 */
	void	createImageArray(
				Engine &engine,
				SamplerInfo samplerInfo,
				std::string imageId,
				const std::vector<std::string> &textureIds);
	/**
	 * @brief Create all image usable for vulkan from all textures.
	 *
	 * @param engine The engine struct.
	 * @param samplerInfo Info for the sampler.
	 *
	 * @exception Throw an runtime_error if imageId is already used or if textureId isn't exist.
	 */
	void	createAllImages(
				Engine &engine,
				SamplerInfo samplerInfo);
	/**
	 * @brief Free all images created.
	 *
	 * @param device The device of VulkanContext class.
	 */
	void	destroyImages(VkDevice device);

//**** STATIC METHODS **********************************************************

private:
//**** PRIVATE ATTRIBUTS *******************************************************
	std::unordered_map<std::string, Texture>	textures;
	std::unordered_map<std::string, Image>		images;

//**** PRIVATE METHODS *********************************************************
	/**
	 * @brief Create a texture image from a texture.
	 *
	 * @param device The device of VulkanContext class.
	 * @param physicalDevice The physical device of VulkanContext class.
	 * @param commandPool The command pool for run the creation of image.
	 * @param texture The texture used for creation of image.
	 * @param image The image to create.
	 * @param memory The image memory for allocation.
	 */
	void	createTextureImage(
				VkDevice device, VkPhysicalDevice physicalDevice,
				const VulkanCommandPool &commandPool,
				Texture &texture, VkImage &image, VkDeviceMemory &memory);
	/**
	 * @brief Create a texture image from a texture.
	 *
	 * @param device The device of VulkanContext class.
	 * @param physicalDevice The physical device of VulkanContext class.
	 * @param commandPool The command pool for run the creation of image.
	 * @param textures The textures used for creation of image array.
	 * @param image The image to create.
	 * @param memory The image memory for allocation.
	 */
	void	createTextureImageArray(
				VkDevice device, VkPhysicalDevice physicalDevice,
				const VulkanCommandPool &commandPool,
				std::vector<Texture*> &textures, VkImage &image,
				VkDeviceMemory &memory);
	/**
	 * @brief Create an image view from an image.
	 *
	 * @param device The device of VulkanContext class.
	 * @param image The image used for create view.
	 * @param view The view to create.
	 */
	void	createTextureImageView(
				VkDevice device,
				VkImage &image,
				VkImageView &view);
	/**
	 * @brief Create an image array view from an image array.
	 *
	 * @param device The device of VulkanContext class.
	 * @param image The image used for create view.
	 * @param view The view to create.
	 */
	void	createTextureImageArrayView(
				VkDevice device,
				VkImage &image,
				VkImageView &view,
				uint32_t nbLayer);
	/**
	 * @brief Create a sampler for an image.
	 *
	 * @param device The device of VulkanContext class.
	 * @param physicalDevice The physical device of VulkanContext class.
	 * @param sampler The sampler to create.
	 * @param samplerInfoParam Info for the sampler.
	 *
	 * @exception Throw a runtime_error if the creation failed.
	 */
	void	createTextureSampler(
				VkDevice device, VkPhysicalDevice physicalDevice,
				VkSampler &sampler, SamplerInfo &samplerInfoParam);
};

//**** FUNCTIONS ***************************************************************

#endif
