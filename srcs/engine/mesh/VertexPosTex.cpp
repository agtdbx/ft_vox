#include <engine/mesh/VertexPosTex.hpp>

//**** STATIC FUNCTIONS DEFINE *************************************************
//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------

VertexPosTex::VertexPosTex(void)
{
	this->pos = gm::Vec3f();
	this->tex = gm::Vec2f();
}


VertexPosTex::VertexPosTex(const VertexPosTex &obj)
{
	this->pos = obj.pos;
	this->tex = obj.tex;
}


VertexPosTex::VertexPosTex(const gm::Vec3f &pos, const gm::Vec2f &tex)
{
	this->pos = pos;
	this->tex = tex;
}

//---- Destructor --------------------------------------------------------------

VertexPosTex::~VertexPosTex()
{

}

//**** ACCESSORS ***************************************************************
//---- Getters -----------------------------------------------------------------
//---- Setters -----------------------------------------------------------------
//---- Operators ---------------------------------------------------------------

VertexPosTex	&VertexPosTex::operator=(const VertexPosTex &obj)
{
	if (this == &obj)
		return (*this);

	this->pos = obj.pos;
	this->tex = obj.tex;

	return (*this);
}

//**** PUBLIC METHODS **********************************************************

std::size_t	VertexPosTex::getHash(void)
{
	std::size_t	hash = 0;

	hash = std::hash<float>{}(this->pos.x) + 0x9e3779b9 + (hash<<6) + (hash>>2);
	hash = std::hash<float>{}(this->pos.y) + 0x9e3779b9 + (hash<<6) + (hash>>2);
	hash = std::hash<float>{}(this->pos.z) + 0x9e3779b9 + (hash<<6) + (hash>>2);
	hash = std::hash<float>{}(this->tex.x) + 0x9e3779b9 + (hash<<6) + (hash>>2);
	hash = std::hash<float>{}(this->tex.y) + 0x9e3779b9 + (hash<<6) + (hash>>2);

	return (hash);
}

//**** STATIC METHODS **********************************************************

VkVertexInputBindingDescription	VertexPosTex::getBindingDescription(void)
{
	VkVertexInputBindingDescription bindingDescription{};

	bindingDescription.binding = 0;
	bindingDescription.stride = sizeof(VertexPosTex);
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	return (bindingDescription);
}


std::array<VkVertexInputAttributeDescription, 2>	VertexPosTex::getAttributeDescriptions(void)
{
	std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

	// Bind info for location 0
	attributeDescriptions[0].binding = 0;
	attributeDescriptions[0].location = 0;
	attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[0].offset = offsetof(VertexPosTex, pos);

	// Bind info for location 1
	attributeDescriptions[1].binding = 0;
	attributeDescriptions[1].location = 1;
	attributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
	attributeDescriptions[1].offset = offsetof(VertexPosTex, tex);

	return (attributeDescriptions);
}

//**** PRIVATE METHODS *********************************************************
//**** FUNCTIONS ***************************************************************
//**** STATIC FUNCTIONS ********************************************************
