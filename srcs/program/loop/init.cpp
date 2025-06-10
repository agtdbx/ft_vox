#include <program/loop/loop.hpp>


static void	loadTextures(Engine &engine);
static void loadShaders(
				Engine &engine,
				ChunkShader &chunkShader);


bool init(
		Engine &engine,
		Map &map,
		ChunkShader &chunkShader,
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

		loadShaders(engine, chunkShader);
		map.init(engine, camera, chunkShader);
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


static void loadShaders(
				Engine &engine,
				ChunkShader &chunkShader)
{
	std::vector<UBOType>	uboTypesChunk = {{sizeof(UBO3DChunkPos), UBO_VERTEX},
											{sizeof(UBO3DChunkCubes), UBO_FRAGMENT}};
	std::vector<UBOType>	uboTypesChunkFdf = {{sizeof(UBO3DChunkPos), UBO_VERTEX}};

	chunkShader.shaderFdfEnable = false;
	chunkShader.shaderBorderEnable = false;

	chunkShader.shader.init<VertexPosNrm>(
						engine, FCUL_COUNTER, DRAW_POLYGON,
						"shadersbin/chunk_vert.spv", "shadersbin/chunk_frag.spv",
						uboTypesChunk, CUBE_TEXTURES.size());
	chunkShader.shaderFdf.init<VertexPosNrm>(
						engine, FCUL_NONE, DRAW_LINE,
						"shadersbin/chunk_vert.spv", "shadersbin/chunkFdf_frag.spv",
						uboTypesChunkFdf);
	chunkShader.shaderBorder.init<VertexPos>(
						engine, FCUL_NONE, DRAW_LINE,
						"shadersbin/chunkBorder_vert.spv", "shadersbin/chunkBorder_frag.spv",
						uboTypesChunkFdf);
}
