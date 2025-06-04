#include <program/loop/loop.hpp>


static void	loadTextures(Engine &engine);
static void loadMeshes(Engine &engine, Mesh &mesh);
static void loadShaders(Engine &engine, Shader &shader);


bool init(
		Engine &engine,
		Mesh &mesh,
		Shader &shader,
		Camera &camera)
{
	camera.setPosition(gm::Vec3f(-0.23f, 1.75f, 3.20f));
	camera.setRotation(-26.19f, -90.03f, 0.0f);

	try
	{
		// Init engine
		initEngine(engine);

		// Load texture
		loadTextures(engine);

		// Vulkan attributs creation
		engine.textureManager.createAllImages(engine);

		loadMeshes(engine, mesh);
		loadShaders(engine, shader);
	}
	catch(const std::exception& e)
	{
		std::cerr << "Error : " << e.what() << std::endl;
		return (false);
	}
	return (true);
}


static void	loadTextures(Engine &engine)
{
	engine.textureManager.addTexture("spaceDuck", "data/textures/spaceDuck.png");
}


static void loadMeshes(Engine &engine, Mesh &mesh)
{
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

	vertices = {
		// Front face vertex
		{{-0.5, 0.5, 0.5}, { 0.0, 0.0, 1.0}, {0.0, 0.0}}, // LU
		{{-0.5,-0.5, 0.5}, { 0.0, 0.0, 1.0}, {0.0, 1.0}}, // LD
		{{ 0.5,-0.5, 0.5}, { 0.0, 0.0, 1.0}, {1.0, 1.0}}, // RD
		{{ 0.5, 0.5, 0.5}, { 0.0, 0.0, 1.0}, {1.0, 0.0}}, // RU
		// Right face vertex
		{{ 0.5, 0.5, 0.5}, { 1.0, 0.0, 0.0}, {0.0, 0.0}}, // LU
		{{ 0.5,-0.5, 0.5}, { 1.0, 0.0, 0.0}, {0.0, 1.0}}, // LD
		{{ 0.5,-0.5,-0.5}, { 1.0, 0.0, 0.0}, {1.0, 1.0}}, // RD
		{{ 0.5, 0.5,-0.5}, { 1.0, 0.0, 0.0}, {1.0, 0.0}}, // RU
		// Left face vertex
		{{-0.5, 0.5,-0.5}, {-1.0, 0.0, 0.0}, {0.0, 0.0}}, // LU
		{{-0.5,-0.5,-0.5}, {-1.0, 0.0, 0.0}, {0.0, 1.0}}, // LD
		{{-0.5,-0.5, 0.5}, {-1.0, 0.0, 0.0}, {1.0, 1.0}}, // RD
		{{-0.5, 0.5, 0.5}, {-1.0, 0.0, 0.0}, {1.0, 0.0}}, // RU
		// Back face vertex
		{{ 0.5, 0.5,-0.5}, { 0.0, 0.0,-1.0}, {0.0, 0.0}}, // LU
		{{ 0.5,-0.5,-0.5}, { 0.0, 0.0,-1.0}, {0.0, 1.0}}, // LD
		{{-0.5,-0.5,-0.5}, { 0.0, 0.0,-1.0}, {1.0, 1.0}}, // RD
		{{-0.5, 0.5,-0.5}, { 0.0, 0.0,-1.0}, {1.0, 0.0}}, // RU
		// Up face vertex
		{{-0.5, 0.5,-0.5}, { 0.0, 0.0,-1.0}, {0.0, 0.0}}, // LU
		{{-0.5, 0.5, 0.5}, { 0.0, 0.0,-1.0}, {0.0, 1.0}}, // LD
		{{ 0.5, 0.5, 0.5}, { 0.0, 0.0,-1.0}, {1.0, 1.0}}, // RD
		{{ 0.5, 0.5,-0.5}, { 0.0, 0.0,-1.0}, {1.0, 0.0}}, // RU
		// Down face vertex
		{{-0.5,-0.5, 0.5}, { 0.0, 0.0,-1.0}, {0.0, 0.0}}, // LU
		{{-0.5,-0.5,-0.5}, { 0.0, 0.0,-1.0}, {0.0, 1.0}}, // LD
		{{ 0.5,-0.5,-0.5}, { 0.0, 0.0,-1.0}, {1.0, 1.0}}, // RD
		{{ 0.5,-0.5, 0.5}, { 0.0, 0.0,-1.0}, {1.0, 0.0}}, // RU

	};
	indices = {
		// Front face
		 0,  1,  2,
		 0,  2,  3,
		// Right face
		 4,  5,  6,
		 4,  6,  7,
		// Left face
		 8,  9, 10,
		 8, 10, 11,
		// Back face
		12, 13, 14,
		12, 14, 15,
		// Up face
		16, 17, 18,
		16, 18, 19,
		// Down face
		20, 21, 22,
		20, 22, 23,
	};

	mesh = Mesh(vertices, indices);
	mesh.createBuffers(engine.commandPool);
}


static void loadShaders(Engine &engine, Shader &shader)
{
	shader.init(engine, sizeof(UBO3DMesh), FCUL_COUNTER,
					"shadersbin/mesh_vert.spv", "shadersbin/mesh_frag.spv",
					{"spaceDuck"});
}
