#ifndef VERTEX_VOXEL_HPP
# define VERTEX_VOXEL_HPP

# include <define.hpp>

# include <array>
# include <gmath.hpp>

/**
 * @brief Class for 3D vertex with a position, a normal, a cube type. Made to work with Vulkan.
 */
class VertexVoxel
{
public:
//**** PUBLIC ATTRIBUTS ********************************************************
	/**
	 * @brief Position of vertex.
	 */
	gm::Vec3f	pos;
	/**
	 * @brief Normal of vertex.
	 */
	gm::Vec3f	nrm;
	/**
	 * @brief Cube type of vertex.
	 */
	uint8_t		cubeType;

//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------
	/**
	 * @brief Default contructor of VertexVoxel class.
	 *
	 * @return The default VertexVoxel.
	 */
	VertexVoxel(void);
	/**
	 * @brief Copy constructor of VertexVoxel class.
	 *
	 * @param obj The VertexVoxel to copy.
	 *
	 * @return The VertexVoxel copied from parameter.
	 */
	VertexVoxel(const VertexVoxel &obj);
	/**
	 * @brief Constructor of VertexVoxel class.
	 *
	 * @param pos The position of VertexVoxel.
	 * @param nrm The normal of VertexVoxel.
	 * @param tex The texture coordonate of VertexVoxel.
	 * @param cubeType The Cube type of VertexVoxel.
	 *
	 * @return The VertexVoxel create from parameter.
	 */
	VertexVoxel(
		const gm::Vec3f &pos,
		const gm::Vec3f &nrm,
		const uint8_t cubeType);

//---- Destructor --------------------------------------------------------------
	/**
	 * @brief Destructor of VertexVoxel class.
	 */
	~VertexVoxel();

//**** ACCESSORS ***************************************************************
//---- Getters -----------------------------------------------------------------
//---- Setters -----------------------------------------------------------------
//---- Operators ---------------------------------------------------------------
	/**
	 * @brief Copy operator of VertexVoxel class.
	 *
	 * @param obj The VertexVoxel to copy.
	 *
	 * @return The VertexVoxel copied from parameter.
	 */
	VertexVoxel	&operator=(const VertexVoxel &obj);

//**** PUBLIC METHODS **********************************************************
	/**
	 * @brief Get hash of vertex.
	 *
	 * @return Hash of vertex as a size_t.
	 */
	std::size_t	getHash(void);

//**** STATIC METHODS **********************************************************
	/**
	 * @brief Get binding description for Vulkan.
	 *
	 * @return The VkVertexInputBindingDescription for VertexVoxel class.
	 */
	static VkVertexInputBindingDescription	getBindingDescription(void);
	/**
	 * @brief Get attribute description for Vulkan.
	 *
	 * @return The VkVertexInputAttributeDescription for VertexVoxel class.
	 */
	static std::array<VkVertexInputAttributeDescription, 3>	getAttributeDescriptions(void);

private:
//**** PRIVATE ATTRIBUTS *******************************************************
//**** PRIVATE METHODS *********************************************************
};

//**** FUNCTIONS ***************************************************************

#endif
