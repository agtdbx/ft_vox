#include <program/overlay/OverlayLiquids.hpp>

//**** STATIC FUNCTIONS DEFINE *************************************************
//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------

OverlayLiquids::OverlayLiquids(void)
{

}


OverlayLiquids::OverlayLiquids(const OverlayLiquids &obj)
{

}

//---- Destructor --------------------------------------------------------------

OverlayLiquids::~OverlayLiquids()
{

}

//**** ACCESSORS ***************************************************************
//---- Getters -----------------------------------------------------------------
//---- Setters -----------------------------------------------------------------
//---- Operators ---------------------------------------------------------------

OverlayLiquids	&OverlayLiquids::operator=(const OverlayLiquids &obj)
{
	if (this == &obj)
		return (*this);

	return (*this);
}

//**** PUBLIC METHODS **********************************************************
void OverlayLiquids::init(Engine &engine, Shader &overlayWaterShader, Shader &overlayLavaShader)
{
	overlayWaterShader.initShaderParam(engine, this->shaderParamWater);
	overlayLavaShader.initShaderParam(engine, this->shaderParamLava);

	std::vector<VertexPos> vertices;
	std::vector<uint32_t> indices;

	vertices = {
		{{-1,-1,0}}, //  0
		{{-1, 1,0}}, //  1
		{{ 1,-1,0}}, //  2
		{{ 1, 1,0}}, //  3
	};

	indices = {
		// Front face
		0, 1, 2,  1, 2, 3,
	};

	this->mesh = Mesh<VertexPos>(vertices, indices);
	this->mesh.createBuffers(engine.commandPool);
}

void OverlayLiquids::drawWater(Engine &engine, Shader &overlayWaterShader)
{
	// Draw mesh
	engine.window.drawMesh(this->mesh, overlayWaterShader, this->shaderParamWater);
}

void OverlayLiquids::drawLava(Engine &engine, Shader &overlayLavaShader)
{
	// Draw mesh
	engine.window.drawMesh(this->mesh, overlayLavaShader, this->shaderParamLava);
}

void OverlayLiquids::destroy(Engine &engine)
{
	this->mesh.destroy();
	this->shaderParamWater.destroy(engine);
	this->shaderParamLava.destroy(engine);
}

//**** STATIC METHODS **********************************************************
//**** PRIVATE METHODS *********************************************************
//**** FUNCTIONS ***************************************************************
//**** STATIC FUNCTIONS ********************************************************
