#include <engine/mesh/VertexVoxel.hpp>

//**** STATIC FUNCTIONS DEFINE *************************************************
//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------

VertexVoxel::VertexVoxel(void)
{
	this->pos = gm::Vec3f();
	this->nrm = gm::Vec3f();
	this->cubeType = 0;
}


VertexVoxel::VertexVoxel(const VertexVoxel &obj)
{
	this->pos = obj.pos;
	this->nrm = obj.nrm;
	this->cubeType = obj.cubeType;
}


VertexVoxel::VertexVoxel(
				const gm::Vec3f &pos,
				const gm::Vec3f &nrm,
				const uint8_t cubeType)
{
	this->pos = pos;
	this->nrm = nrm;
	this->cubeType = cubeType;
}

//---- Destructor --------------------------------------------------------------

VertexVoxel::~VertexVoxel()
{

}

//**** ACCESSORS ***************************************************************
//---- Getters -----------------------------------------------------------------
//---- Setters -----------------------------------------------------------------
//---- Operators ---------------------------------------------------------------

VertexVoxel	&VertexVoxel::operator=(const VertexVoxel &obj)
{
	if (this == &obj)
		return (*this);

	this->pos = obj.pos;
	this->nrm = obj.nrm;
	this->cubeType = obj.cubeType;

	return (*this);
}

//**** PUBLIC METHODS **********************************************************

std::size_t	VertexVoxel::getHash(void)
{
	std::size_t	hash = 0;

	hash = std::hash<float>{}(this->pos.x) + 0x9e3779b9 + (hash<<6) + (hash>>2);
	hash = std::hash<float>{}(this->pos.y) + 0x9e3779b9 + (hash<<6) + (hash>>2);
	hash = std::hash<float>{}(this->pos.z) + 0x9e3779b9 + (hash<<6) + (hash>>2);
	hash = std::hash<float>{}(this->nrm.x) + 0x9e3779b9 + (hash<<6) + (hash>>2);
	hash = std::hash<float>{}(this->nrm.y) + 0x9e3779b9 + (hash<<6) + (hash>>2);
	hash = std::hash<float>{}(this->nrm.z) + 0x9e3779b9 + (hash<<6) + (hash>>2);
	hash = std::hash<uint8_t>{}(this->cubeType) + 0x9e3779b9 + (hash<<6) + (hash>>2);

	return (hash);
}

//**** STATIC METHODS **********************************************************

VkVertexInputBindingDescription	VertexVoxel::getBindingDescription(void)
{
	VkVertexInputBindingDescription bindingDescription{};

	bindingDescription.binding = 0;
	bindingDescription.stride = sizeof(VertexVoxel);
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	return (bindingDescription);
}


std::array<VkVertexInputAttributeDescription, 3>	VertexVoxel::getAttributeDescriptions(void)
{
	std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

	// Bind info for location 0
	attributeDescriptions[0].binding = 0;
	attributeDescriptions[0].location = 0;
	attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[0].offset = offsetof(VertexVoxel, pos);

	// Bind info for location 1
	attributeDescriptions[1].binding = 0;
	attributeDescriptions[1].location = 1;
	attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[1].offset = offsetof(VertexVoxel, nrm);

	// Bind info for location 2
	attributeDescriptions[2].binding = 0;
	attributeDescriptions[2].location = 2;
	attributeDescriptions[2].format = VK_FORMAT_R8_UINT;
	attributeDescriptions[2].offset = offsetof(VertexVoxel, cubeType);

	return (attributeDescriptions);
}

//**** PRIVATE METHODS *********************************************************
//**** FUNCTIONS ***************************************************************
//**** STATIC FUNCTIONS ********************************************************
