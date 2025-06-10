#ifndef VERTEX_POS_NRM_HPP
# define VERTEX_POS_NRM_HPP

# include <define.hpp>

# include <array>
# include <gmath.hpp>

/**
 * @brief Class for 3D vertex with a position and a normal. Made to work with Vulkan.
 */
class VertexPosNrm
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

//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------
	/**
	 * @brief Default contructor of VertexPosNrm class.
	 *
	 * @return The default VertexPosNrm.
	 */
	VertexPosNrm(void);
	/**
	 * @brief Copy constructor of VertexPosNrm class.
	 *
	 * @param obj The VertexPosNrm to copy.
	 *
	 * @return The VertexPosNrm copied from parameter.
	 */
	VertexPosNrm(const VertexPosNrm &obj);
	/**
	 * @brief Constructor of VertexPosNrm class.
	 *
	 * @param pos The position of VertexPosNrm.
	 * @param nrm The normal of VertexPosNrm.
	 *
	 * @return The VertexPosNrm create from parameter.
	 */
	VertexPosNrm(const gm::Vec3f &pos, const gm::Vec3f &nrm);

//---- Destructor --------------------------------------------------------------
	/**
	 * @brief Destructor of VertexPosNrm class.
	 */
	~VertexPosNrm();

//**** ACCESSORS ***************************************************************
//---- Getters -----------------------------------------------------------------
//---- Setters -----------------------------------------------------------------
//---- Operators ---------------------------------------------------------------
	/**
	 * @brief Copy operator of VertexPosNrm class.
	 *
	 * @param obj The VertexPosNrm to copy.
	 *
	 * @return The VertexPosNrm copied from parameter.
	 */
	VertexPosNrm	&operator=(const VertexPosNrm &obj);

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
	 * @return The VkVertexInputBindingDescription for VertexPosNrm class.
	 */
	static VkVertexInputBindingDescription	getBindingDescription(void);
	/**
	 * @brief Get attribute description for Vulkan.
	 *
	 * @return The VkVertexInputAttributeDescription for VertexPosNrm class.
	 */
	static std::array<VkVertexInputAttributeDescription, 2>	getAttributeDescriptions(void);

private:
//**** PRIVATE ATTRIBUTS *******************************************************
//**** PRIVATE METHODS *********************************************************
};

//**** FUNCTIONS ***************************************************************

#endif
