#ifndef MESH_HPP
# define MESH_HPP

# include <engine/mesh/Vertex.hpp>
# include <engine/vulkan/VulkanCommandPool.hpp>

# include <vector>

/**
 * @brief Class for 3D mesh. Made to work with Vulkan.
 */
class Mesh
{
public:
//**** PUBLIC ATTRIBUTS ********************************************************
//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------
	/**
	 * @brief Default contructor of Mesh class.
	 *
	 * @return The default Mesh.
	 */
	Mesh(void);
	/**
	 * @brief Copy constructor of Mesh class.
	 *
	 * @param obj The Mesh to copy.
	 *
	 * @return The Mesh copied from parameter.
	 */
	Mesh(const Mesh &obj);
	/**
	 * @brief Constructor of Mesh class.
	 *
	 * @param vertices The vector of vertex.
	 * @param indices The vector of index.
	 *
	 * @return The Mesh created from parameters.
	 *
	 * @warning You need to call createBuffers after it if you want to use it for drawing.
	 */
	Mesh(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices);

//---- Destructor --------------------------------------------------------------
	/**
	 * @brief Destructor of Mesh class.
	 */
	~Mesh();

//**** ACCESSORS ***************************************************************
//---- Getters -----------------------------------------------------------------
	/**
	 * @brief Getter of position.
	 *
	 * @return The position as Vec3f.
	 */
	const gm::Vec3f	&getPosition(void) const;
	/**
	 * @brief Getter of model.
	 *
	 * @return The model matrix as Mat4f.
	 */
	const gm::Mat4f	&getModel(void) const;
	/**
	 * @brief Getter of vertices.
	 *
	 * @return A vector of Vertex.
	 */
	const std::vector<Vertex>	&getVertices(void) const;
	/**
	 * @brief Getter of indices.
	 *
	 * @return A vector of index as int32.
	 */
	const std::vector<uint32_t>	&getIndices(void) const;
	/**
	 * @brief Get the number of vertex.
	 *
	 * @return Number of vertex as uint32.
	 */
	uint32_t	getNbVertex(void) const;
	/**
	 * @brief Get the number of index.
	 *
	 * @return Number of index as uint32.
	 */
	uint32_t	getNbIndex(void) const;
	/**
	 * @brief Getter of vertex buffer.
	 *
	 * @return Vertex buffer.
	 */
	VkBuffer	getVertexBuffer(void) const;
	/**
	 * @brief Getter of index buffer.
	 *
	 * @return Index buffer.
	 */
	VkBuffer	getIndexBuffer(void) const;

//---- Setters -----------------------------------------------------------------
	/**
	 * @brief Set mesh position.
	 *
	 * @param position New position.
	 */
	void	setPosition(const gm::Vec3f &position);
	/**
	 * @brief Set mesh rotation.
	 *
	 * @param axis New rotation axis.
	 * @param radians New rotation radians.
	 */
	void	setRotation(const gm::Vec3f &axis, const float radians);
	/**
	 * @brief Set mesh scale.
	 *
	 * @param scale New scale.
	 *
	 * @warning If scale is 0, you will loose real vertices position !
	 */
	void	setScale(const float scale);

//---- Operators ---------------------------------------------------------------
	/**
	 * @brief Copy operator of Mesh class.
	 *
	 * @param obj The Mesh to copy.
	 *
	 * @return The Mesh copied from parameter.
	 */
	Mesh	&operator=(const Mesh &obj);

//**** PUBLIC METHODS **********************************************************
//---- Mesh operation ----------------------------------------------------------
	/**
	 * @brief Load a 3d mesh from variables.
	 *
	 * @param vertices The vector of vertex.
	 * @param indices The vector of index.
	 *
	 * @warning You need to call createBuffers after it if you want to use it for drawing.
	 */
	void	loadMesh(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices);
	/**
	 * @brief Create buffers.
	 *
 	 * @param commandPool The command pool for creating buffers. It will be save for next calls.
	 */
	void	createBuffers(VulkanCommandPool &commandPool);
	/**
	 * @brief Clear allocated memory.
	 */
	void	destroy(void);
	/**
	 * @brief Clear only allocated memory for buffers.
	 */
	void	destroyBuffers(void);

//---- Geometry operation ------------------------------------------------------
	/**
	 * @brief Apply a translation on the mesh.
	 *
	 * @param movement Movement to apply.
	 */
	void	translate(const gm::Vec3f &movement);
	/**
	 * @brief Apply a rotation on the mesh.
	 *
	 * @param axis The axis of the rotation. Must be normalized.
	 * @param radians The angle for rotation, in radians.
	 */
	void	rotate(const gm::Vec3f &axis, const float radians);
	/**
	 * @brief Apply a scaling on the mesh.
	 *
	 * @param scale The factor for the scale.
	 *
	 * @warning If scale is 0, you will loose real vertices position !
	 */
	void	scale(const float scale);

//**** STATIC METHODS **********************************************************

private:
//**** PRIVATE ATTRIBUTS *******************************************************
	gm::Vec3f				position;
	gm::Mat4f				model;
	float					scalingFactor;
	std::vector<Vertex>		vertices;
	std::vector<uint32_t>	indices;
	uint32_t				nbVertex;
	uint32_t				nbIndex;
	VkBuffer				vertexBuffer, indexBuffer;
	VkDeviceMemory			vertexBufferMemory, indexBufferMemory;
//---- Copy --------------------------------------------------------------------
	VulkanCommandPool		*commandPool;

//**** PRIVATE METHODS *********************************************************
	/**
	 * @brief Create vertex buffer and vertex buffer memory.
	 */
	void	createVertexBuffer(void);
	/**
	 * @brief Create index buffer and index buffer memory.
	 */
	void	createIndexBuffer(void);
	/**
	 * @brief Update vertex buffer.
	 */
	void	updateVertexBuffer(void);
	/**
	 * @brief Update index buffer.
	 */
	void	updateIndexBuffer(void);
};

//**** FUNCTIONS ***************************************************************

#endif
