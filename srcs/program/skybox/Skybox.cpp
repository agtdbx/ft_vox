# include <program/skybox/Skybox.hpp>

//**** STATIC FUNCTIONS DEFINE *************************************************
//**** INITIALISION ************************************************************
//---- Constructors ------------------------------------------------------------

Skybox::Skybox()
{
}

//---- Destructor --------------------------------------------------------------

Skybox::~Skybox()
{
}

//**** ACCESSORS ***************************************************************
//---- Getters -----------------------------------------------------------------
//---- Setters -----------------------------------------------------------------
//---- Operators ---------------------------------------------------------------
//**** PUBLIC METHODS **********************************************************

void Skybox::init(Engine &engine, Camera &camera, Shader &skyboxShader)
{
	std::vector<VertexPosTex> vertices;
	std::vector<uint32_t> indices;

	skyboxShader.initShaderParam(engine, this->shaderParam, {"skybox"});

	float	x0 =    0.0f;
	float	x1 =  901.0f;
	float	x2 = 1799.0f;
	float	x3 = 2700.0f;
	float	x4 = 3600.0f;
	float	y0 =    0.0f;
	float	y1 =  901.0f;
	float	y2 = 1799.0f;
	float	y3 = 2700.0f;

	vertices = {
		{{-SKYBOX_DIST, SKYBOX_DIST,-SKYBOX_DIST}, {x1, y1}}, //  0
		{{-SKYBOX_DIST,-SKYBOX_DIST,-SKYBOX_DIST}, {x1, y2}}, //  1
		{{ SKYBOX_DIST,-SKYBOX_DIST,-SKYBOX_DIST}, {x2, y2}}, //  2
		{{ SKYBOX_DIST, SKYBOX_DIST,-SKYBOX_DIST}, {x2, y1}}, //  3
		{{ SKYBOX_DIST,-SKYBOX_DIST, SKYBOX_DIST}, {x3, y2}}, //  4
		{{ SKYBOX_DIST, SKYBOX_DIST, SKYBOX_DIST}, {x3, y1}}, //  5
		{{-SKYBOX_DIST,-SKYBOX_DIST, SKYBOX_DIST}, {x4, y2}}, //  6
		{{-SKYBOX_DIST, SKYBOX_DIST, SKYBOX_DIST}, {x4, y1}}, //  7
		{{-SKYBOX_DIST, SKYBOX_DIST, SKYBOX_DIST}, {x0, y1}}, //  8
		{{-SKYBOX_DIST,-SKYBOX_DIST, SKYBOX_DIST}, {x0, y2}}, //  9
		{{-SKYBOX_DIST, SKYBOX_DIST, SKYBOX_DIST}, {x1, y0}}, // 10
		{{ SKYBOX_DIST, SKYBOX_DIST, SKYBOX_DIST}, {x2, y0}}, // 11
		{{-SKYBOX_DIST,-SKYBOX_DIST, SKYBOX_DIST}, {x1, y3}}, // 12
		{{ SKYBOX_DIST,-SKYBOX_DIST, SKYBOX_DIST}, {x2, y3}}, // 13
	};

	indices = {
		// Front face
		0, 1, 2,  0, 2, 3,
		// Right face
		3, 2, 4,  3, 4, 5,
		// Back face
		5, 4, 6,  5, 6, 7,
		// Left face
		8, 9, 1,  8, 1, 0,
		// Up face
		10, 0, 3, 10, 3,11,
		// Down face
		1,12,13,  1,13,2,
	};

	this->mesh = Mesh<VertexPosTex>(vertices, indices);
	this->mesh.createBuffers(engine.commandPool);
	this->uboPos.proj = camera.getProjection();
	this->uboPos.proj.at(1, 1) *= -1;
	this->uboPos.model = this->mesh.getModel();
	this->uboPos.pos = gm::Vec4f(this->mesh.getPosition());
}


void Skybox::draw(Engine &engine, Camera &camera, Shader &skyboxShader)
{
	this->uboPos.view = camera.getView();
	this->uboPos.pos = camera.getPosition();

	// Draw mesh
	this->shaderParam.updateBuffer(engine.window, &this->uboPos, 0);
	engine.window.drawMesh(this->mesh, skyboxShader, this->shaderParam);
}


void Skybox::destroy(Engine &engine)
{
	this->mesh.destroy();
	this->shaderParam.destroy(engine);
}

//**** STATIC METHODS **********************************************************
//**** PRIVATE METHODS *********************************************************
//**** FUNCTIONS ***************************************************************
//**** STATIC FUNCTIONS ********************************************************
