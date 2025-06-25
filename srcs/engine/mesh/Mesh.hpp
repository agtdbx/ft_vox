#ifndef MESH_HPP
# define MESH_HPP

# include <engine/mesh/Vertex.hpp>
# include <engine/mesh/StagingBuffer.hpp>
# include <engine/vulkan/VulkanCommandPool.hpp>
# include <program/perfLogger/PerfLogger.hpp> // TODO : Remove

# include <vector>
# include <cstring>

/**
 * @brief Class for 3D mesh. Made to work with Vulkan.
 */
template<typename VertexType>
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
	Mesh(void)
	{
		this->position = gm::Vec3f(0.0f);
		this->model = gm::Mat4f(1.0f);
		this->scalingFactor = 1.0f;
		this->nbVertex = 0;
		this->nbIndex = 0;
		this->vertexBuffer = NULL;
		this->vertexBufferMemory = NULL;
		this->indexBuffer = NULL;
		this->indexBufferMemory = NULL;

		this->commandPool = NULL;
	}
	/**
	 * @brief Copy constructor of Mesh class.
	 *
	 * @param obj The Mesh to copy.
	 *
	 * @return The Mesh copied from parameter.
	 */
	Mesh(const Mesh &obj)
	{
		this->position = obj.position;
		this->model = obj.model;
		this->scalingFactor = obj.scalingFactor;
		this->vertices = obj.vertices;
		this->indices = obj.indices;
		this->nbVertex = obj.nbVertex;
		this->nbIndex = obj.nbIndex;

		this->commandPool = obj.commandPool;

		this->createVertexBuffer();
		this->createIndexBuffer();
	}
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
	Mesh(const std::vector<VertexType> &vertices, const std::vector<uint32_t> &indices)
	{
		this->position = gm::Vec3f(0.0f);
		this->model = gm::Mat4f(1.0f);
		this->scalingFactor = 1.0f;
		this->vertices = vertices;
		this->indices = indices;
		this->nbVertex = static_cast<uint32_t>(this->vertices.size());
		this->nbIndex = static_cast<uint32_t>(this->indices.size());

		this->commandPool = NULL;
	}


//---- Destructor --------------------------------------------------------------
	/**
	 * @brief Destructor of Mesh class.
	 */
	~Mesh()
	{
	}

//**** ACCESSORS ***************************************************************
//---- Getters -----------------------------------------------------------------
	/**
	 * @brief Getter of position.
	 *
	 * @return The position as Vec3f.
	 */
	const gm::Vec3f	&getPosition(void) const
	{
		return (this->position);
	}
	/**
	 * @brief Getter of model.
	 *
	 * @return The model matrix as Mat4f.
	 */
	const gm::Mat4f	&getModel(void) const
	{
		return (this->model);
	}
	/**
	 * @brief Getter of vertices.
	 *
	 * @return A reference to vector of VertexType.
	 */
	std::vector<VertexType>	&getVertices(void)
	{
		return (this->vertices);
	}
	/**
	 * @brief Getter of vertices.
	 *
	 * @return A const reference to vector of VertexType.
	 */
	const std::vector<VertexType>	&getVertices(void) const
	{
		return (this->vertices);
	}
	/**
	 * @brief Getter of indices.
	 *
	 * @return A reference to vector of index as int32.
	 */
	std::vector<uint32_t>	&getIndices(void)
	{
		return (this->indices);
	}
	/**
	 * @brief Getter of indices.
	 *
	 * @return A const reference to vector of index as int32.
	 */
	const std::vector<uint32_t>	&getIndices(void) const
	{
		return (this->indices);
	}
	/**
	 * @brief Get the number of vertex.
	 *
	 * @return Number of vertex as uint32.
	 */
	uint32_t	getNbVertex(void) const
	{
		return (this->nbVertex);
	}
	/**
	 * @brief Get the number of index.
	 *
	 * @return Number of index as uint32.
	 */
	uint32_t	getNbIndex(void) const
	{
		return (this->nbIndex);
	}
	/**
	 * @brief Getter of vertex buffer.
	 *
	 * @return Vertex buffer.
	 */
	VkBuffer	getVertexBuffer(void) const
	{
		return (this->vertexBuffer);
	}
	/**
	 * @brief Getter of index buffer.
	 *
	 * @return Index buffer.
	 */
	VkBuffer	getIndexBuffer(void) const
	{
		return (this->indexBuffer);
	}

//---- Setters -----------------------------------------------------------------
	/**
	 * @brief Set mesh position.
	 *
	 * @param position New position.
	 */
	void	setPosition(const gm::Vec3f &position)
	{
		this->position = position;
	}
	/**
	 * @brief Set mesh rotation.
	 *
	 * @param axis New rotation axis.
	 * @param radians New rotation radians.
	 */
	void	setRotation(const gm::Vec3f &axis, const float radians)
	{
		this->model = gm::rotate(gm::Mat4f(this->scalingFactor), axis, radians);
	}

	/**
	 * @brief Set mesh scale.
	 *
	 * @param scale New scale.
	 *
	 * @warning If scale is 0, you will loose real vertices position !
	 */
	void	setScale(const float scale)
	{
		if (this->scalingFactor == 0.0f)
			return ;

		const float	factor = scale / this->scalingFactor;

		this->scalingFactor = scale;
		this->model.at(0, 0) *= factor;
		this->model.at(1, 1) *= factor;
		this->model.at(2, 2) *= factor;
	}

//---- Operators ---------------------------------------------------------------
	/**
	 * @brief Copy operator of Mesh class.
	 *
	 * @param obj The Mesh to copy.
	 *
	 * @return The Mesh copied from parameter.
	 */
	Mesh	&operator=(const Mesh &obj)
	{
		if (this == &obj)
			return (*this);

		this->destroy();

		this->position = obj.position;
		this->model = obj.model;
		this->scalingFactor = obj.scalingFactor;
		this->vertices = obj.vertices;
		this->indices = obj.indices;
		this->nbVertex = obj.nbVertex;
		this->nbIndex = obj.nbIndex;

		return (*this);
	}

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
	void	loadMesh(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices)
	{
		this->destroy();

		this->position = gm::Vec3f(0.0f);
		this->model = gm::Mat4f(1.0f);
		this->scalingFactor = 1.0f;
		this->vertices = vertices;
		this->indices = indices;
		this->nbVertex = static_cast<uint32_t>(this->vertices.size());
		this->nbIndex = static_cast<uint32_t>(this->indices.size());
	}
	/**
	 * @brief Update some mesh info when vertices or indices vector has been modifed outside the class.
	 */
	void	updateMeshInfo(void)
	{
		this->position = gm::Vec3f(0.0f);
		this->model = gm::Mat4f(1.0f);
		this->scalingFactor = 1.0f;
		this->nbVertex = static_cast<uint32_t>(this->vertices.size());
		this->nbIndex = static_cast<uint32_t>(this->indices.size());
	}
	/**
	 * @brief Create buffers.
	 *
 	 * @param commandPool The command pool for creating buffers. It will be save for next calls.
	 */
	void	createBuffers(VulkanCommandPool &commandPool)
	{
		if (this->nbVertex == 0)
			return ;

		this->destroyBuffers();

		this->commandPool = &commandPool;

		VkDevice			copyDevice = this->commandPool->getCopyDevice();
		VkPhysicalDevice	copyPhysicalDevice = this->commandPool->getCopyPhysicalDevice();

		VkDeviceSize	bufferSizeVertex = sizeof(this->vertices[0]) * this->nbVertex;
		VkDeviceSize	bufferSizeIndex = sizeof(this->indices[0]) * this->nbIndex;
		VkDeviceSize	bufferSize = gm::max(bufferSizeVertex, bufferSizeIndex);

		// Create temp buffers
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		createVulkanBuffer(copyDevice, copyPhysicalDevice,
							bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
							VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
							stagingBuffer, stagingBufferMemory);

		PerfLogger perfLogger;
		this->createVertexBuffer(stagingBuffer, stagingBufferMemory, perfLogger);
		this->createIndexBuffer(stagingBuffer, stagingBufferMemory, perfLogger);

		// Release temp buffer
		vkDestroyBuffer(copyDevice, stagingBuffer, nullptr);
		vkFreeMemory(copyDevice, stagingBufferMemory, nullptr);
	}
	/**
	 * @brief Create buffers.
	 *
 	 * @param commandPool The command pool for creating buffers. It will be save for next calls.
 	 * @param stagingBuffer The tempory buffer use to avoid create new tempory buffer for each mesh.
	 */
	void	createBuffers(VulkanCommandPool &commandPool, StagingBuffer &stagingBuffer, PerfLogger &perfLogger)
	{
		if (this->nbVertex == 0)
			return ;

		this->destroyBuffers();

		this->commandPool = &commandPool;

		VkDeviceSize	bufferSizeVertex = sizeof(this->vertices[0]) * this->nbVertex;
		VkDeviceSize	bufferSizeIndex = sizeof(this->indices[0]) * this->nbIndex;
		VkDeviceSize	bufferSize = gm::max(bufferSizeVertex, bufferSizeIndex);

		perflogStart(perfLogger.createUpdateStaging);
		stagingBuffer.update(commandPool, bufferSize);
		perflogEnd(perfLogger.createUpdateStaging);

		this->createVertexBuffer(stagingBuffer.buffer, stagingBuffer.memory, perfLogger);
		this->createIndexBuffer(stagingBuffer.buffer, stagingBuffer.memory, perfLogger);
	}
	/**
	 * @brief Clear allocated memory.
	 */
	void	destroy(void)
	{
		this->vertices.clear();
		this->indices.clear();
		this->nbVertex = 0;
		this->nbIndex = 0;

		this->destroyBuffers();
	}

	/**
	 * @brief Clear only allocated memory for buffers.
	 */
	void	destroyBuffers(void)
	{
		if (this->commandPool == NULL)
			return ;

		VkDevice	copyDevice = this->commandPool->getCopyDevice();

		// Free vertex buffer and memory
		if (this->vertexBuffer != NULL)
		{
			vkDestroyBuffer(copyDevice, this->vertexBuffer, nullptr);
			this->vertexBuffer = NULL;
		}
		if (this->vertexBufferMemory != NULL)
		{
			vkFreeMemory(copyDevice, this->vertexBufferMemory, nullptr);
			this->vertexBufferMemory = NULL;
		}

		// Free index buffer and memory
		if (this->indexBuffer != NULL)
		{
			vkDestroyBuffer(copyDevice, this->indexBuffer, nullptr);
			this->indexBuffer = NULL;
		}
		if (this->indexBufferMemory != NULL)
		{
			vkFreeMemory(copyDevice, this->indexBufferMemory, nullptr);
			this->indexBufferMemory = NULL;
		}

		this->commandPool = NULL;
	}

//---- Geometry operation ------------------------------------------------------
	/**
	 * @brief Apply a translation on the mesh.
	 *
	 * @param movement Movement to apply.
	 */
	void	translate(const gm::Vec3f &movement)
	{
		this->position += movement; // TODO : CHANGE IT FOR MATRIX TRANSLATE
	}
	/**
	 * @brief Apply a rotation on the mesh.
	 *
	 * @param axis The axis of the rotation. Must be normalized.
	 * @param radians The angle for rotation, in radians.
	 */
	void	rotate(const gm::Vec3f &axis, const float radians)
	{
		this->model = gm::rotate(this->model, axis, radians);
	}
	/**
	 * @brief Apply a scaling on the mesh.
	 *
	 * @param scale The factor for the scale.
	 *
	 * @warning If scale is 0, you will loose real vertices position !
	 */
	void	scale(const float scale)
	{
		this->scalingFactor *= scale;
		this->model.at(0, 0) *= scale;
		this->model.at(1, 1) *= scale;
		this->model.at(2, 2) *= scale;
	}


//**** STATIC METHODS **********************************************************

private:
//**** PRIVATE ATTRIBUTS *******************************************************
	gm::Vec3f				position;
	gm::Mat4f				model;
	float					scalingFactor;
	std::vector<VertexType>	vertices;
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
	void	createVertexBuffer(
				VkBuffer stagingBuffer,
				VkDeviceMemory stagingBufferMemory,
				PerfLogger &perfLogger)
	{
		if (this->commandPool == NULL || this->nbVertex == 0)
			return ;

		VkDevice			copyDevice = this->commandPool->getCopyDevice();
		VkPhysicalDevice	copyPhysicalDevice = this->commandPool->getCopyPhysicalDevice();

		VkDeviceSize	bufferSize = sizeof(this->vertices[0]) * this->nbVertex;

		// Map data to vertex buffer
		perflogStart(perfLogger.mapVertexBuffer);
		void	*data;
		vkMapMemory(copyDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, this->vertices.data(), (size_t) bufferSize);
		vkUnmapMemory(copyDevice, stagingBufferMemory);
		perflogEnd(perfLogger.mapVertexBuffer);

		// Create final buffer
		perflogStart(perfLogger.createVertexBuffer);
		createVulkanBuffer(copyDevice, copyPhysicalDevice,
							bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
							VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
							this->vertexBuffer, this->vertexBufferMemory);
		perflogEnd(perfLogger.createVertexBuffer);

		// Copy data form temp to final buffer
		perflogStart(perfLogger.copyVertexBuffer);
		copyBuffer(*commandPool, stagingBuffer, this->vertexBuffer, bufferSize);
		perflogEnd(perfLogger.copyVertexBuffer);
	}
	/**
	 * @brief Create index buffer and index buffer memory.
	 */
	void	createIndexBuffer(
				VkBuffer stagingBuffer,
				VkDeviceMemory stagingBufferMemory,
				PerfLogger &perfLogger)
	{
		if (this->commandPool == NULL || this->nbIndex == 0)
			return ;

		VkDevice			copyDevice = this->commandPool->getCopyDevice();
		VkPhysicalDevice	copyPhysicalDevice = this->commandPool->getCopyPhysicalDevice();

		VkDeviceSize bufferSize = sizeof(this->indices[0]) * this->nbIndex;

		// Map data to vertex buffer
		perflogStart(perfLogger.mapIndexBuffer);
		void* data;
		vkMapMemory(copyDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, this->indices.data(), (size_t) bufferSize);
		vkUnmapMemory(copyDevice, stagingBufferMemory);
		perflogEnd(perfLogger.mapIndexBuffer);

		// Create final buffer
		perflogStart(perfLogger.createIndexBuffer);
		createVulkanBuffer(copyDevice, copyPhysicalDevice,
							bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
							VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, this->indexBuffer, this->indexBufferMemory);
		perflogEnd(perfLogger.createIndexBuffer);

		// Copy data form temp to final buffer
		perflogStart(perfLogger.copyIndexBuffer);
		copyBuffer(*commandPool, stagingBuffer, this->indexBuffer, bufferSize);
		perflogEnd(perfLogger.copyIndexBuffer);
	}
	/**
	 * @brief Update vertex buffer.
	 */
	void	updateVertexBuffer(void)
	{
		if (this->commandPool == NULL)
			return ;

		VkDevice			copyDevice = this->commandPool->getCopyDevice();
		VkPhysicalDevice	copyPhysicalDevice = this->commandPool->getCopyPhysicalDevice();

		VkDeviceSize	bufferSize = sizeof(this->vertices[0]) * this->nbVertex;

		// Create temp buffers
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		createVulkanBuffer(copyDevice, copyPhysicalDevice,
							bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
							VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
							stagingBuffer, stagingBufferMemory);

		// Map data to vertex buffer
		void	*data;
		vkMapMemory(copyDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, this->vertices.data(), (size_t) bufferSize);
		vkUnmapMemory(copyDevice, stagingBufferMemory);

		// Copy data form temp to final buffer
		copyBuffer(*commandPool, stagingBuffer, this->vertexBuffer, bufferSize);

		// Release temp buffer
		vkDestroyBuffer(copyDevice, stagingBuffer, nullptr);
		vkFreeMemory(copyDevice, stagingBufferMemory, nullptr);
	}
	/**
	 * @brief Update index buffer.
	 */
	void	updateIndexBuffer(void)
	{
		if (this->commandPool == NULL)
			return ;

		VkDevice			copyDevice = this->commandPool->getCopyDevice();
		VkPhysicalDevice	copyPhysicalDevice = this->commandPool->getCopyPhysicalDevice();

		VkDeviceSize bufferSize = sizeof(this->indices[0]) * this->nbIndex;

		// Create temp buffers
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		createVulkanBuffer(copyDevice, copyPhysicalDevice,
							bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
							VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
							stagingBuffer, stagingBufferMemory);

		// Map data to vertex buffer
		void* data;
		vkMapMemory(copyDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, this->indices.data(), (size_t) bufferSize);
		vkUnmapMemory(copyDevice, stagingBufferMemory);

		// Copy data form temp to final buffer
		copyBuffer(*commandPool, stagingBuffer, this->indexBuffer, bufferSize);

		// Release temp buffer
		vkDestroyBuffer(copyDevice, stagingBuffer, nullptr);
		vkFreeMemory(copyDevice, stagingBufferMemory, nullptr);
	}
};

//**** FUNCTIONS ***************************************************************

#endif
