#include <program/loop/loop.hpp>


static void	loadTextures(Engine &engine);
static void loadShaders(
				Engine &engine,
				Shaders &shaders);


bool init(
		Engine &engine,
		Camera &camera,
		Objects &objects,
		Shaders &shaders)
{
	camera.setPosition(gm::Vec3f(0.0f, 100.0f, 0.0f));
	camera.setRotation(-30.0f, -90.0f, 0.0f);

	// TODO : Remove
	camera.setPosition(gm::Vec3f(10.0f, 203.0f, 15.0f));
	camera.setRotation(-28.67f, -73.83f, 0.0f);

	try
	{
		// Init engine
		initEngine(engine);

		// Load texture
		loadTextures(engine);

		// Vulkan attributs creation
		loadShaders(engine, shaders);
		objects.map.init(engine, camera, shaders.chunkShader);
		objects.skybox.init(engine, camera, shaders.skyboxShader);
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
	// Load sky box
	engine.textureManager.addTexture("skybox", "data/skybox/skybox.png");
	engine.textureManager.createImage(
							engine,
							{false, true, false, false},
							"skybox", "skybox");

	// Load cube texture and put them in an array.
	for (const std::string &name : CUBE_TEXTURES)
	{
		engine.textureManager.addTexture(name, "data/cubes/" + name +".png");
	}
	engine.textureManager.createImageArray(
							engine,
							{true, true, false, false},
							"cubes", CUBE_TEXTURES);

}


static void loadShaders(
				Engine &engine,
				Shaders &shaders)
{
	std::vector<BufferInfo>	bufferInfosChunk = {{sizeof(UBO3DChunkPos), BUFFER_UBO, STAGE_VERTEX}};
	std::vector<ImageInfo>	imageInfosCubes = {
		{CUBE_TEXTURES.size(), STAGE_COMPUTE_FRAGMENT},
	};
	std::vector<ImageInfo>	imageInfosSkybox = {
		{1, STAGE_COMPUTE_FRAGMENT},
	};

	shaders.chunkShader.shaderFdfEnable = false;
	shaders.chunkShader.shaderBorderEnable = false;

	shaders.chunkShader.shader.init<VertexVoxel>(engine,
						DEPTH_READ_WRITE, FCUL_COUNTER, DRAW_POLYGON, ALPHA_OFF,
						"shadersbin/chunk_vert.spv", "shadersbin/chunk_frag.spv",
						bufferInfosChunk, imageInfosCubes);
	shaders.chunkShader.shaderWater.init<VertexVoxel>(engine,
						DEPTH_READ, FCUL_COUNTER, DRAW_POLYGON, ALPHA_ON,
						"shadersbin/chunk_vert.spv", "shadersbin/chunk_frag.spv",
						bufferInfosChunk, imageInfosCubes);
	shaders.chunkShader.shaderFdf.init<VertexVoxel>(engine,
						DEPTH_READ_WRITE, FCUL_NONE, DRAW_LINE, ALPHA_OFF,
						"shadersbin/chunk_vert.spv", "shadersbin/chunkFdf_frag.spv",
						bufferInfosChunk);
	shaders.chunkShader.shaderBorder.init<VertexPos>(engine,
						DEPTH_READ_WRITE, FCUL_NONE, DRAW_LINE, ALPHA_OFF,
						"shadersbin/chunkBorder_vert.spv", "shadersbin/chunkBorder_frag.spv",
						bufferInfosChunk);

	shaders.skyboxShader.init<VertexPosTex>(engine,
						DEPTH_DISABLE, FCUL_COUNTER, DRAW_POLYGON, ALPHA_OFF,
						"shadersbin/skybox_vert.spv", "shadersbin/skybox_frag.spv",
						bufferInfosChunk, imageInfosSkybox);
}
