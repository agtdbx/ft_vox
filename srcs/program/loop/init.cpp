#include <program/loop/loop.hpp>


static void	loadTextures(Engine &engine);
static void loadShaders(Engine &engine, Shader &shader);


bool init(
		Engine &engine,
		Map &map,
		Shader &shader,
		Camera &camera)
{
	camera.setPosition(gm::Vec3f(7.65f, 15.89f, 24.78f));
	camera.setRotation(-24.08f, -90.03f, 0.0f);

	try
	{
		// Init engine
		initEngine(engine);

		// Load texture
		loadTextures(engine);

		// Vulkan attributs creation
		engine.textureManager.createAllImages(engine);

		map.init(engine.commandPool, camera);
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
	std::vector<std::string>	names = {
		"diamond", "dirt", "grass", "ice", "iron",
		"lava", "sand", "snow", "stone", "water"
	};

	engine.textureManager.addTexture("test", "data/textures/test.png");

	for (std::string &name : names)
	{
		engine.textureManager.addTexture(name + "-up", "data/textures/" + name +"-up.png");
		engine.textureManager.addTexture(name + "-side", "data/textures/" + name +"-side.png");
		engine.textureManager.addTexture(name + "-down", "data/textures/" + name +"-down.png");
	}
}


static void loadShaders(Engine &engine, Shader &shader)
{
	shader.init<VertexPos>(engine, sizeof(UBO3DChunk), FCUL_COUNTER,
							"shadersbin/mesh_vert.spv", "shadersbin/meshUp_frag.spv",
							{"test"});
}
