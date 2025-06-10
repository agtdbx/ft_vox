#ifndef SHADER_PARAM_HPP
# define SHADER_PARAM_HPP

# include <define.hpp>
# include <engine/engine.hpp>
# include <engine/window/Window.hpp>
# include <engine/textures/TextureManager.hpp>

# include <string>
# include <fstream>
# include <gmath.hpp>

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

/**
 * @brief Class for shader with vulkan.
 */
class ShaderParam
{
public:
//**** PUBLIC ATTRIBUTS ********************************************************
//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------
	/**
	 * @brief Default contructor of ShaderParam class.
	 *
	 * @return The default ShaderParam.
	 */
	ShaderParam(void);
	/**
	 * @brief Copy constructor of ShaderParam class.
	 *
	 * @param obj The ShaderParam to copy.
	 *
	 * @return The ShaderParam copied from parameter.
	 */
	ShaderParam(const ShaderParam &obj);

//---- Destructor --------------------------------------------------------------
	/**
	 * @brief Destructor of ShaderParam class.
	 */
	~ShaderParam();

//**** ACCESSORS ***************************************************************
//---- Getters -----------------------------------------------------------------
	/**
	 * @brief Getter of descriptor sets.
	 *
	 * @return The descriptor sets.
	 */
	std::vector<VkDescriptorSet>	&getDescriptorSets(void);

//---- Setters -----------------------------------------------------------------
//---- Operators ---------------------------------------------------------------
	/**
	 * @brief Copy operator of ShaderParam class.
	 *
	 * @param obj The ShaderParam to copy.
	 *
	 * @return The ShaderParam copied from parameter.
	 */
	ShaderParam	&operator=(const ShaderParam &obj);

//**** PUBLIC METHODS **********************************************************
	/**
	 * @brief Init shader from parameters.
	 *
	 * @param engine The engine struct.
	 * @param uboTypes Vector of ubo types.
	 * @param imageIds Vector of image id to used in shader.
	 */
	void	init(
				Engine &engine,
				VkDescriptorSetLayout descriptorSetLayout,
				const std::vector<UBOType> &uboTypes,
				const std::vector<std::string> &imageIds);
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
	std::vector<VkBuffer>			uniformBuffers;
	std::vector<VkDeviceMemory>		uniformBuffersMemory;
	std::vector<void*>				uniformBuffersMapped;
	VkDescriptorPool				descriptorPool;
	std::vector<VkDescriptorSet>	descriptorSets;

//**** PRIVATE METHODS *********************************************************
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
				VkDescriptorSetLayout descriptorSetLayout,
				const std::vector<const Image *> &images);
};

//**** FUNCTIONS ***************************************************************
//**** STATIC FUNCTIONS ********************************************************

#endif
