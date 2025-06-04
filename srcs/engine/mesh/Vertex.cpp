#include <engine/mesh/Vertex.hpp>

//**** STATIC FUNCTIONS DEFINE *************************************************
//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------

Vertex::Vertex(void)
{
	this->pos = gm::Vec3f();
	this->nrm = gm::Vec3f();
	this->tex = gm::Vec2f();
}


Vertex::Vertex(const Vertex &obj)
{
	this->pos = obj.pos;
	this->nrm = obj.nrm;
	this->tex = obj.tex;
}


Vertex::Vertex(const gm::Vec3f &pos, const gm::Vec3f &nrm, const gm::Vec2f &tex)
{
	this->pos = pos;
	this->nrm = nrm;
	this->tex = tex;
}

//---- Destructor --------------------------------------------------------------

Vertex::~Vertex()
{

}

//**** ACCESSORS ***************************************************************
//---- Getters -----------------------------------------------------------------
//---- Setters -----------------------------------------------------------------
//---- Operators ---------------------------------------------------------------

Vertex	&Vertex::operator=(const Vertex &obj)
{
	if (this == &obj)
		return (*this);

	this->pos = obj.pos;
	this->nrm = obj.nrm;
	this->tex = obj.tex;

	return (*this);
}

//**** PUBLIC METHODS **********************************************************

std::size_t	Vertex::getHash(void)
{
	std::size_t	hash = 0;

	hash = std::hash<float>{}(this->pos.x) + 0x9e3779b9 + (hash<<6) + (hash>>2);
	hash = std::hash<float>{}(this->pos.y) + 0x9e3779b9 + (hash<<6) + (hash>>2);
	hash = std::hash<float>{}(this->pos.z) + 0x9e3779b9 + (hash<<6) + (hash>>2);
	hash = std::hash<float>{}(this->nrm.x) + 0x9e3779b9 + (hash<<6) + (hash>>2);
	hash = std::hash<float>{}(this->nrm.y) + 0x9e3779b9 + (hash<<6) + (hash>>2);
	hash = std::hash<float>{}(this->nrm.z) + 0x9e3779b9 + (hash<<6) + (hash>>2);
	hash = std::hash<float>{}(this->tex.x) + 0x9e3779b9 + (hash<<6) + (hash>>2);
	hash = std::hash<float>{}(this->tex.y) + 0x9e3779b9 + (hash<<6) + (hash>>2);

	return (hash);
}

//**** STATIC METHODS **********************************************************

VkVertexInputBindingDescription	Vertex::getBindingDescription(void)
{
	VkVertexInputBindingDescription bindingDescription{};

	bindingDescription.binding = 0;
	bindingDescription.stride = sizeof(Vertex);
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	return (bindingDescription);
}


std::array<VkVertexInputAttributeDescription, 3>	Vertex::getAttributeDescriptions(void)
{
	std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

	// Bind info for location 0
	attributeDescriptions[0].binding = 0;
	attributeDescriptions[0].location = 0;
	attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[0].offset = offsetof(Vertex, pos);

	// Bind info for location 1
	attributeDescriptions[1].binding = 0;
	attributeDescriptions[1].location = 1;
	attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[1].offset = offsetof(Vertex, nrm);

	// Bind info for location 2
	attributeDescriptions[2].binding = 0;
	attributeDescriptions[2].location = 2;
	attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
	attributeDescriptions[2].offset = offsetof(Vertex, tex);

	return (attributeDescriptions);
}

//**** PRIVATE METHODS *********************************************************
//**** FUNCTIONS ***************************************************************
//**** STATIC FUNCTIONS ********************************************************
