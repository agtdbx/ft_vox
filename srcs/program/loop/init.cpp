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
	camera.setPosition(gm::Vec3f(-3.0f, 235.0f, 212.0f));
	camera.setRotation(-33.73f, -91.42f, 0.0f);

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
	std::vector<BufferInfo>	uboTypesChunk = {{sizeof(UBO3DChunkPos), BUFFER_UBO, STAGE_VERTEX},
											{sizeof(UBO3DChunkCubes), BUFFER_SSBO, STAGE_FRAGMENT}};
	std::vector<BufferInfo>	uboTypesChunkFdf = {{sizeof(UBO3DChunkPos), BUFFER_UBO, STAGE_VERTEX}};

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
