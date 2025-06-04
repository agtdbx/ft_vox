#include <engine/mesh/Mesh.hpp>

#include <engine/vulkan/VulkanUtils.hpp>

#include <cstring>

//**** STATIC FUNCTIONS DEFINE *************************************************
//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------

Mesh::Mesh(void)
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


Mesh::Mesh(const Mesh &obj)
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


Mesh::Mesh(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices)
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

Mesh::~Mesh()
{
}

//**** ACCESSORS ***************************************************************
//---- Getters -----------------------------------------------------------------

const gm::Vec3f	&Mesh::getPosition(void) const
{
	return (this->position);
}


const gm::Mat4f	&Mesh::getModel(void) const
{
	return (this->model);
}


const std::vector<Vertex>	&Mesh::getVertices(void) const
{
	return (this->vertices);
}


const std::vector<uint32_t>	&Mesh::getIndices(void) const
{
	return (this->indices);
}


uint32_t	Mesh::getNbVertex(void) const
{
	return (this->nbVertex);
}


uint32_t	Mesh::getNbIndex(void) const
{
	return (this->nbIndex);
}

VkBuffer	Mesh::getVertexBuffer(void) const
{
	return (this->vertexBuffer);
}

VkBuffer	Mesh::getIndexBuffer(void) const
{
	return (this->indexBuffer);
}

//---- Setters -----------------------------------------------------------------

void	Mesh::setPosition(const gm::Vec3f &position)
{
	this->position = position;
}


void	Mesh::setRotation(const gm::Vec3f &axis, const float radians)
{
	this->model = gm::rotate(gm::Mat4f(this->scalingFactor), axis, radians);
}


void	Mesh::setScale(const float scale)
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

Mesh	&Mesh::operator=(const Mesh &obj)
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

void	Mesh::loadMesh(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices)
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


void	Mesh::createBuffers(VulkanCommandPool &commandPool)
{
	this->destroyBuffers();

	this->commandPool = &commandPool;

	this->createVertexBuffer();
	this->createIndexBuffer();
}


void	Mesh::destroy(void)
{
	this->vertices.clear();
	this->indices.clear();
	this->nbVertex = 0;
	this->nbIndex = 0;

	this->destroyBuffers();
}


void	Mesh::destroyBuffers(void)
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

void	Mesh::translate(const gm::Vec3f &movement)
{
	this->position += movement;
}


void	Mesh::rotate(const gm::Vec3f &axis, const float radians)
{
	this->model = gm::rotate(this->model, axis, radians);
}


void	Mesh::scale(const float scale)
{
	this->scalingFactor *= scale;
	this->model.at(0, 0) *= scale;
	this->model.at(1, 1) *= scale;
	this->model.at(2, 2) *= scale;
}

//**** STATIC METHODS **********************************************************
//**** PRIVATE METHODS *********************************************************

void	Mesh::createVertexBuffer(void)
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
						bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
						VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
						stagingBuffer, stagingBufferMemory);

	// Map data to vertex buffer
	void	*data;
	vkMapMemory(copyDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, this->vertices.data(), (size_t) bufferSize);
	vkUnmapMemory(copyDevice, stagingBufferMemory);

	// Create final buffer
	createVulkanBuffer(copyDevice, copyPhysicalDevice,
						bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
						VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
						this->vertexBuffer, this->vertexBufferMemory);

	// Copy data form temp to final buffer
	copyBuffer(*commandPool, stagingBuffer, this->vertexBuffer, bufferSize);

	// Release temp buffer
	vkDestroyBuffer(copyDevice, stagingBuffer, nullptr);
	vkFreeMemory(copyDevice, stagingBufferMemory, nullptr);
}


void	Mesh::createIndexBuffer(void)
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

	// Create final buffer
	createVulkanBuffer(copyDevice, copyPhysicalDevice,
						bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
						VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, this->indexBuffer, this->indexBufferMemory);

	// Copy data form temp to final buffer
	copyBuffer(*commandPool, stagingBuffer, this->indexBuffer, bufferSize);

	// Release temp buffer
	vkDestroyBuffer(copyDevice, stagingBuffer, nullptr);
	vkFreeMemory(copyDevice, stagingBufferMemory, nullptr);
}


void	Mesh::updateVertexBuffer(void)
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
						bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
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


void	Mesh::updateIndexBuffer(void)
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

//**** FUNCTIONS ***************************************************************
//**** STATIC FUNCTIONS ********************************************************
