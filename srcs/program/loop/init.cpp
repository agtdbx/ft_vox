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
	camera.setPosition(gm::Vec3f(0.0f, 200.0f, 0.0f));
	camera.setRotation(-30.0f, -90.0f, 0.0f);

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
	for (const std::string &name : CUBE_TEXTURES)
	{
		engine.textureManager.addTexture(name, "data/textures/" + name +".png");
	}

	engine.textureManager.createImageArray(engine, "cubes", CUBE_TEXTURES);
}


static void loadShaders(
				Engine &engine,
				ChunkShader &chunkShader)
{
	std::vector<BufferInfo>	bufferInfosChunk = {{sizeof(UBO3DChunkPos), BUFFER_UBO, STAGE_VERTEX},
											{sizeof(UBO3DChunkCubes), BUFFER_SSBO, STAGE_FRAGMENT}};
	std::vector<BufferInfo>	bufferInfosChunkFdf = {{sizeof(UBO3DChunkPos), BUFFER_UBO, STAGE_VERTEX}};
	std::vector<ImageInfo>	imageInfos = {
		{CUBE_TEXTURES.size(), STAGE_COMPUTE_FRAGMENT},
	};

	chunkShader.shaderFdfEnable = false;
	chunkShader.shaderBorderEnable = false;

	chunkShader.shader.init<VertexPosNrm>(
						engine, FCUL_COUNTER, DRAW_POLYGON,
						"shadersbin/chunk_vert.spv", "shadersbin/chunk_frag.spv",
						bufferInfosChunk, imageInfos);
	chunkShader.shaderFdf.init<VertexPosNrm>(
						engine, FCUL_NONE, DRAW_LINE,
						"shadersbin/chunk_vert.spv", "shadersbin/chunkFdf_frag.spv",
						bufferInfosChunkFdf);
	chunkShader.shaderBorder.init<VertexPos>(
						engine, FCUL_NONE, DRAW_LINE,
						"shadersbin/chunkBorder_vert.spv", "shadersbin/chunkBorder_frag.spv",
						bufferInfosChunkFdf);
}
