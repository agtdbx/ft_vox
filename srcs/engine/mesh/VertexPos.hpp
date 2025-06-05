#ifndef VERTEX_POS_HPP
# define VERTEX_POS_HPP

# include <define.hpp>

# include <array>
# include <gmath.hpp>

/**
 * @brief Class for 3D vertex with a position. Made to work with Vulkan.
 */
class VertexPos
{
public:
//**** PUBLIC ATTRIBUTS ********************************************************
	/**
	 * @brief Position of vertex.
	 */
	gm::Vec3f	pos;

//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------
	/**
	 * @brief Default contructor of VertexPos class.
	 *
	 * @return The default VertexPos.
	 */
	VertexPos(void);
	/**
	 * @brief Copy constructor of VertexPos class.
	 *
	 * @param obj The VertexPos to copy.
	 *
	 * @return The VertexPos copied from parameter.
	 */
	VertexPos(const VertexPos &obj);
	/**
	 * @brief Constructor of VertexPos class.
	 *
	 * @param pos The position of VertexPos.
	 *
	 * @return The VertexPos create from parameter.
	 */
	VertexPos(const gm::Vec3f &pos);

//---- Destructor --------------------------------------------------------------
	/**
	 * @brief Destructor of VertexPos class.
	 */
	~VertexPos();

//**** ACCESSORS ***************************************************************
//---- Getters -----------------------------------------------------------------
//---- Setters -----------------------------------------------------------------
//---- Operators ---------------------------------------------------------------
	/**
	 * @brief Copy operator of VertexPos class.
	 *
	 * @param obj The VertexPos to copy.
	 *
	 * @return The VertexPos copied from parameter.
	 */
	VertexPos	&operator=(const VertexPos &obj);

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
	 * @return The VkVertexInputBindingDescription for VertexPos class.
	 */
	static VkVertexInputBindingDescription	getBindingDescription(void);
	/**
	 * @brief Get attribute description for Vulkan.
	 *
	 * @return The VkVertexInputAttributeDescription for VertexPos class.
	 */
	static std::array<VkVertexInputAttributeDescription, 1>	getAttributeDescriptions(void);

private:
//**** PRIVATE ATTRIBUTS *******************************************************
//**** PRIVATE METHODS *********************************************************
};

//**** FUNCTIONS ***************************************************************

#endif
