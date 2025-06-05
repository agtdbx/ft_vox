#include <engine/mesh/VertexPos.hpp>

//**** STATIC FUNCTIONS DEFINE *************************************************
//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------

VertexPos::VertexPos(void)
{
	this->pos = gm::Vec3f();
}


VertexPos::VertexPos(const VertexPos &obj)
{
	this->pos = obj.pos;
}


VertexPos::VertexPos(const gm::Vec3f &pos)
{
	this->pos = pos;
}

//---- Destructor --------------------------------------------------------------

VertexPos::~VertexPos()
{

}

//**** ACCESSORS ***************************************************************
//---- Getters -----------------------------------------------------------------
//---- Setters -----------------------------------------------------------------
//---- Operators ---------------------------------------------------------------

VertexPos	&VertexPos::operator=(const VertexPos &obj)
{
	if (this == &obj)
		return (*this);

	this->pos = obj.pos;

	return (*this);
}

//**** PUBLIC METHODS **********************************************************

std::size_t	VertexPos::getHash(void)
{
	std::size_t	hash = 0;

	hash = std::hash<float>{}(this->pos.x) + 0x9e3779b9 + (hash<<6) + (hash>>2);
	hash = std::hash<float>{}(this->pos.y) + 0x9e3779b9 + (hash<<6) + (hash>>2);
	hash = std::hash<float>{}(this->pos.z) + 0x9e3779b9 + (hash<<6) + (hash>>2);

	return (hash);
}

//**** STATIC METHODS **********************************************************

VkVertexInputBindingDescription	VertexPos::getBindingDescription(void)
{
	VkVertexInputBindingDescription bindingDescription{};

	bindingDescription.binding = 0;
	bindingDescription.stride = sizeof(VertexPos);
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	return (bindingDescription);
}


std::array<VkVertexInputAttributeDescription, 1>	VertexPos::getAttributeDescriptions(void)
{
	std::array<VkVertexInputAttributeDescription, 1> attributeDescriptions{};

	// Bind info for location 0
	attributeDescriptions[0].binding = 0;
	attributeDescriptions[0].location = 0;
	attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[0].offset = offsetof(VertexPos, pos);

	return (attributeDescriptions);
}

//**** PRIVATE METHODS *********************************************************
//**** FUNCTIONS ***************************************************************
//**** STATIC FUNCTIONS ********************************************************
