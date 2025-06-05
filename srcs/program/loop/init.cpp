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
	engine.textureManager.addTexture("dirt", "data/textures/dirt.png");
}


static void loadShaders(Engine &engine, Shader &shader)
{
	shader.init(engine, sizeof(UBO3DChunk), FCUL_COUNTER,
					"shadersbin/mesh_vert.spv", "shadersbin/mesh_frag.spv",
					{"dirt"});
}
