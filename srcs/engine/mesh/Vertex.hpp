#ifndef VERTEX_HPP
# define VERTEX_HPP

# include <define.hpp>

# include <array>
# include <gmath.hpp>

/**
 * @brief Class for 3D vertex with a position, a normal and a texture coordinates. Made to work with Vulkan.
 */
class Vertex
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
	 * @brief Texture coordinates of vertex.
	 */
	gm::Vec2f	tex;

//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------
	/**
	 * @brief Default contructor of Vertex class.
	 *
	 * @return The default Vertex.
	 */
	Vertex(void);
	/**
	 * @brief Copy constructor of Vertex class.
	 *
	 * @param obj The Vertex to copy.
	 *
	 * @return The Vertex copied from parameter.
	 */
	Vertex(const Vertex &obj);
	/**
	 * @brief Constructor of Vertex class.
	 *
	 * @param pos The position of Vertex.
	 * @param nrm The normal of Vertex.
	 * @param tex The texture coordonate of Vertex.
	 *
	 * @return The Vertex create from parameter.
	 */
	Vertex(const gm::Vec3f &pos, const gm::Vec3f &nrm, const gm::Vec2f &tex);

//---- Destructor --------------------------------------------------------------
	/**
	 * @brief Destructor of Vertex class.
	 */
	~Vertex();

//**** ACCESSORS ***************************************************************
//---- Getters -----------------------------------------------------------------
//---- Setters -----------------------------------------------------------------
//---- Operators ---------------------------------------------------------------
	/**
	 * @brief Copy operator of Vertex class.
	 *
	 * @param obj The Vertex to copy.
	 *
	 * @return The Vertex copied from parameter.
	 */
	Vertex	&operator=(const Vertex &obj);

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
	 * @return The VkVertexInputBindingDescription for Vertex class.
	 */
	static VkVertexInputBindingDescription	getBindingDescription(void);
	/**
	 * @brief Get attribute description for Vulkan.
	 *
	 * @return The VkVertexInputAttributeDescription for Vertex class.
	 */
	static std::array<VkVertexInputAttributeDescription, 3>	getAttributeDescriptions(void);

private:
//**** PRIVATE ATTRIBUTS *******************************************************
//**** PRIVATE METHODS *********************************************************
};

//**** FUNCTIONS ***************************************************************

#endif
