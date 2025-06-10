#include <engine/mesh/VertexPosNrm.hpp>

//**** STATIC FUNCTIONS DEFINE *************************************************
//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------

VertexPosNrm::VertexPosNrm(void)
{
	this->pos = gm::Vec3f();
	this->nrm = gm::Vec3f();
}


VertexPosNrm::VertexPosNrm(const VertexPosNrm &obj)
{
	this->pos = obj.pos;
	this->nrm = obj.nrm;
}


VertexPosNrm::VertexPosNrm(const gm::Vec3f &pos, const gm::Vec3f &nrm)
{
	this->pos = pos;
	this->nrm = nrm;
}

//---- Destructor --------------------------------------------------------------

VertexPosNrm::~VertexPosNrm()
{

}

//**** ACCESSORS ***************************************************************
//---- Getters -----------------------------------------------------------------
//---- Setters -----------------------------------------------------------------
//---- Operators ---------------------------------------------------------------

VertexPosNrm	&VertexPosNrm::operator=(const VertexPosNrm &obj)
{
	if (this == &obj)
		return (*this);

	this->pos = obj.pos;
	this->nrm = obj.nrm;

	return (*this);
}

//**** PUBLIC METHODS **********************************************************

std::size_t	VertexPosNrm::getHash(void)
{
	std::size_t	hash = 0;

	hash = std::hash<float>{}(this->pos.x) + 0x9e3779b9 + (hash<<6) + (hash>>2);
	hash = std::hash<float>{}(this->pos.y) + 0x9e3779b9 + (hash<<6) + (hash>>2);
	hash = std::hash<float>{}(this->pos.z) + 0x9e3779b9 + (hash<<6) + (hash>>2);
	hash = std::hash<float>{}(this->nrm.x) + 0x9e3779b9 + (hash<<6) + (hash>>2);
	hash = std::hash<float>{}(this->nrm.y) + 0x9e3779b9 + (hash<<6) + (hash>>2);
	hash = std::hash<float>{}(this->nrm.z) + 0x9e3779b9 + (hash<<6) + (hash>>2);

	return (hash);
}

//**** STATIC METHODS **********************************************************

VkVertexInputBindingDescription	VertexPosNrm::getBindingDescription(void)
{
	VkVertexInputBindingDescription bindingDescription{};

	bindingDescription.binding = 0;
	bindingDescription.stride = sizeof(VertexPosNrm);
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	return (bindingDescription);
}


std::array<VkVertexInputAttributeDescription, 2>	VertexPosNrm::getAttributeDescriptions(void)
{
	std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

	// Bind info for location 0
	attributeDescriptions[0].binding = 0;
	attributeDescriptions[0].location = 0;
	attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[0].offset = offsetof(VertexPosNrm, pos);

	// Bind info for location 1
	attributeDescriptions[1].binding = 0;
	attributeDescriptions[1].location = 1;
	attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[1].offset = offsetof(VertexPosNrm, nrm);

	return (attributeDescriptions);
}

//**** PRIVATE METHODS *********************************************************
//**** FUNCTIONS ***************************************************************
//**** STATIC FUNCTIONS ********************************************************
