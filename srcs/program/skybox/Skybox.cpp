# include <program/skybox/Skybox.hpp>

Skybox::Skybox()
{

}

Skybox::~Skybox()
{

}

void Skybox::init(Engine &engine, Shader &skyboxShader, Camera &camera)
{
    std::vector<VertexPosTex> vertices;
    std::vector<uint32_t> indices;

    skyboxShader.initShaderParam(engine, this->shaderParam, {});

    vertices = {
        {{1000 , 1000 , 1000},{0, 0}},
        {{-1000, 1000 , 1000},{0, 0}},
        {{1000 , -1000, 1000},{0, 0}},
        {{1000 , 1000 , -1000},{0, 0}},
        {{-1000, -1000, 1000},{0, 0}},
        {{1000 , -1000, -1000},{0, 0}},
        {{-1000, 1000 , -1000},{0, 0}},
        {{-1000, -1000, -1000},{0, 0}},
    };

    indices = {
        //back face
        6,7,5, 6,5,3,
        //front face
        0,2,4, 0,4,1,
        //left face
        1,4,7, 1,7,6,
        //right face
        3,5,2, 3,2,0,
        //up face
        1,6,3, 1,3,0,
        //down face
        7,4,2, 7,2,5,
    };

    this->mesh = Mesh<VertexPosTex>(vertices, indices);
    this->mesh.createBuffers(engine.commandPool);
    this->uboPos.proj = camera.getProjection();
	this->uboPos.proj.at(1, 1) *= -1;
    this->uboPos.model = this->mesh.getModel();
	this->uboPos.pos = gm::Vec4f(this->mesh.getPosition());
}

void Skybox::destroy(Engine &engine)
{
    this->mesh.destroy();
    this->shaderParam.destroy(engine);
}

void Skybox::draw(Engine &engine, Camera &camera, Shader &skyboxShader)
{
	this->uboPos.view = camera.getView();

	// Draw mesh
    this->shaderParam.updateBuffer(engine.window, &this->uboPos, 0);
    engine.window.drawMesh(this->mesh, skyboxShader, this->shaderParam);
}
