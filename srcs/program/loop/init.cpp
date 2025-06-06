#include <program/loop/loop.hpp>


static void	loadTextures(Engine &engine);
static void loadShaders(Engine &engine, Shader &chunkShader);


bool init(
		Engine &engine,
		Map &map,
		Shader &chunkShader,
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
		loadShaders(engine, chunkShader);
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
		"grass", "dirt", "stone", "water", "snow",
		"ice", "sand", "lava", "iron", "diamond",
	};

	engine.textureManager.addTexture("test", "data/textures/test.png");

	for (std::string &name : names)
	{
		engine.textureManager.addTexture(name + "-up", "data/textures/" + name +"-up.png");
		engine.textureManager.addTexture(name + "-side", "data/textures/" + name +"-side.png");
		engine.textureManager.addTexture(name + "-down", "data/textures/" + name +"-down.png");
	}
}


static void loadShaders(Engine &engine, Shader &chunkShader)
{
	std::vector<UBOType>	uboTypes = {{sizeof(UBO3DChunkPos), UBO_VERTEX}, {sizeof(UBO3DChunkCubes), UBO_FRAGMENT}};
	std::vector<std::string>	texturesUp = {"grass-up", "dirt-up", "stone-up", "water-up", "snow-up",
												"ice-up", "sand-up", "lava-up", "iron-up", "diamond-up"};
	std::vector<std::string>	texturesSide = {"grass-side", "dirt-side", "stone-side", "water-side", "snow-side",
												"ice-side", "sand-side", "lava-side", "iron-side", "diamond-side"};
	std::vector<std::string>	texturesDown = {"grass-down", "dirt-down", "stone-down", "water-down", "snow-down",
												"ice-down", "sand-down", "lava-down", "iron-down", "diamond-down"};

	chunkShader.init<VertexPos>(
					engine, FCUL_COUNTER,
					"shadersbin/mesh_vert.spv", "shadersbin/mesh_frag.spv",
					uboTypes, texturesUp);
}
