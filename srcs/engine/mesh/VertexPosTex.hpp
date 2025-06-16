#ifndef VERTEX_POS_TEX_HPP
# define VERTEX_POS_TEX_HPP

# include <define.hpp>

# include <array>
# include <gmath.hpp>

/**
 * @brief Class for 3D vertex with a position and a normal. Made to work with Vulkan.
 */
class VertexPosTex
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
	gm::Vec2f	tex;

//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------
	/**
	 * @brief Default contructor of VertexPosTex class.
	 *
	 * @return The default VertexPosTex.
	 */
	VertexPosTex(void);
	/**
	 * @brief Copy constructor of VertexPosTex class.
	 *
	 * @param obj The VertexPosTex to copy.
	 *
	 * @return The VertexPosTex copied from parameter.
	 */
	VertexPosTex(const VertexPosTex &obj);
	/**
	 * @brief Constructor of VertexPosTex class.
	 *
	 * @param pos The position of VertexPosTex.
	 * @param tex The normal of VertexPosTex.
	 *
	 * @return The VertexPosTex create from parameter.
	 */
	VertexPosTex(const gm::Vec3f &pos, const gm::Vec2f &tex);

//---- Destructor --------------------------------------------------------------
	/**
	 * @brief Destructor of VertexPosTex class.
	 */
	~VertexPosTex();

//**** ACCESSORS ***************************************************************
//---- Getters -----------------------------------------------------------------
//---- Setters -----------------------------------------------------------------
//---- Operators ---------------------------------------------------------------
	/**
	 * @brief Copy operator of VertexPosTex class.
	 *
	 * @param obj The VertexPosTex to copy.
	 *
	 * @return The VertexPosTex copied from parameter.
	 */
	VertexPosTex	&operator=(const VertexPosTex &obj);

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
	 * @return The VkVertexInputBindingDescription for VertexPosTex class.
	 */
	static VkVertexInputBindingDescription	getBindingDescription(void);
	/**
	 * @brief Get attribute description for Vulkan.
	 *
	 * @return The VkVertexInputAttributeDescription for VertexPosTex class.
	 */
	static std::array<VkVertexInputAttributeDescription, 2>	getAttributeDescriptions(void);

private:
//**** PRIVATE ATTRIBUTS *******************************************************
//**** PRIVATE METHODS *********************************************************
};

//**** FUNCTIONS ***************************************************************

#endif
