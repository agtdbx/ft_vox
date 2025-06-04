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
		{{-0.5, 0.5, 0.5}, {0.0, 0.0, 1.0}, {0.0, 0.0}}, // LUF
		{{-0.5,-0.5, 0.5}, {0.0, 0.0, 1.0}, {0.0, 1.0}}, // LDF
		{{ 0.5,-0.5, 0.5}, {0.0, 0.0, 1.0}, {1.0, 1.0}}, // RDF
		{{ 0.5, 0.5, 0.5}, {0.0, 0.0, 1.0}, {1.0, 0.0}}, // RUF
		{{-0.5, 0.5,-0.5}, {0.0, 0.0, 1.0}, {0.0, 0.0}}, // LUB
		{{-0.5,-0.5,-0.5}, {0.0, 0.0, 1.0}, {0.0, 1.0}}, // LDB
		{{ 0.5,-0.5,-0.5}, {0.0, 0.0, 1.0}, {1.0, 1.0}}, // RDB
		{{ 0.5, 0.5,-0.5}, {0.0, 0.0, 1.0}, {1.0, 0.0}}, // RUB
	};
	indices = {
		0, 1, 2, // Front face 1
		0, 2, 3, // Front face 2
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
