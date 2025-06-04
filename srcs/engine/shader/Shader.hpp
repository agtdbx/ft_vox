#ifndef SHADER_HPP
# define SHADER_HPP

# include <define.hpp>
# include <engine/engine.hpp>
# include <engine/window/Window.hpp>
# include <engine/textures/TextureManager.hpp>

# include <string>
# include <gmath.hpp>

enum FaceCulling
{
	FCUL_NONE,
	FCUL_CLOCK,
	FCUL_COUNTER,
};

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
	/**
	 * @brief Getter of descriptor sets.
	 *
	 * @return The descriptor sets.
	 */
	std::vector<VkDescriptorSet>	&getDescriptorSets(void);

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
	 * @param uboStructSize The size of the struct used for UBO.
	 * @param faceCulling How do face culling. Clock wise, counter or disable it.
	 * @param vertexPath Path to compile vertex shader file.
	 * @param fragmentPath Path to compile fragment shader file.
	 */
	void	init(
				Engine &engine, size_t uboStructSize, FaceCulling faceCulling,
				std::string vertexPath, std::string fragmentPath);
	/**
	 * @brief Init shader from parameters.
	 *
	 * @param engine The engine struct.
	 * @param uboStructSize The size of the struct used for UBO.
	 * @param faceCulling How do face culling. Clock wise, counter or disable it.
	 * @param vertexPath Path to compile vertex shader file.
	 * @param fragmentPath Path to compile fragment shader file.
	 * @param imageIds Vector of image id to used in shader.
	 */
	void	init(
				Engine &engine, size_t uboStructSize, FaceCulling faceCulling,
				std::string vertexPath, std::string fragmentPath,
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
	 */
	void	updateUBO(Window &window, void *ubo);

//**** STATIC METHODS **********************************************************

private:
//**** PRIVATE ATTRIBUTS *******************************************************
	size_t							uboStructSize;
	VkDescriptorSetLayout			descriptorSetLayout;
	VkPipelineLayout				pipelineLayout;
	VkPipeline						graphicsPipeline;
	std::vector<VkBuffer>			uniformBuffers;
	std::vector<VkDeviceMemory>		uniformBuffersMemory;
	std::vector<void*>				uniformBuffersMapped;
	VkDescriptorPool				descriptorPool;
	std::vector<VkDescriptorSet>	descriptorSets;

//**** PRIVATE METHODS *********************************************************
	/**
	 * @brief Create descriptor set layout.
	 *
	 * @param device The device of VulkanContext class.
	 * @param nbImages The number of image used in shader.
	 */
	void	createDescriptorSetLayout(VkDevice device, size_t nbImages);
	/**
	 * @brief Create graphic pipeline.
	 *
	 * @param device The device of VulkanContext class.
	 * @param window The Window class.
	 * @param vertexPath Path to compile vertex shader file.
	 * @param fragmentPath Path to compile fragment shader file.
	 */
	void	createGraphicsPipeline(
				VkDevice device, Window &window,
				std::string vertexPath, std::string fragmentPath,
				FaceCulling faceCulling);
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
				const std::vector<const Image *> &images);
};

//**** FUNCTIONS ***************************************************************

#endif
