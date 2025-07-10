#include <program/loop/loop.hpp>


static void	loadTextures(Engine &engine);
static void loadShaders(
				Engine &engine,
				Shaders &shaders);
static void	initUi(
				Engine &engine,
				Objects &objects,
				Shaders &shaders);

bool init(
		Engine &engine,
		Camera &camera,
		Objects &objects,
		Shaders &shaders)
{
	try
	{
		// Init engine
		initEngine(engine);

		// Load texture
		loadTextures(engine);

		// Vulkan attributs creation
		loadShaders(engine, shaders);

		initUi(engine, objects, shaders);

		objects.map.init(engine, camera, shaders.chunkShader);
		objects.skybox.init(engine, camera, shaders.skyboxShader);
	}
	catch(const std::exception& e)
	{
		std::cerr << "Error : " << e.what() << std::endl;
		return (false);
	}

	camera.setPosition(gm::Vec3f(0.0f, 150.0f, 0.0f));
	camera.setRotation(-30.0f, -90.0f, 0.0f);
	engine.inputManager.mouse.setMouseMode(engine.glfwWindow, GLFW_CURSOR_DISABLED);

	return (true);
}


static void	loadTextures(Engine &engine)
{
	// Load font texture
	engine.textureManager.addTexture("font", "data/font/ascii.png");
	engine.textureManager.createImage(
							engine,
							{false, true, false, false},
							"font", "font");

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
	std::vector<BufferInfo>	bufferTextInfosChunk = {
								{sizeof(UBOText), BUFFER_UBO, STAGE_VERTEX},
								{sizeof(UBOTextColor), BUFFER_UBO, STAGE_FRAGMENT}
	};
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

	shaders.textShader.init<VertexPosTex>(engine,
						DEPTH_DISABLE, FCUL_NONE, DRAW_POLYGON, ALPHA_ON,
						"shadersbin/text_vert.spv", "shadersbin/text_frag.spv",
						bufferTextInfosChunk, imageInfosSkybox);
}


static void	initUi(
				Engine &engine,
				Objects &objects,
				Shaders &shaders)
{
	objects.textFps.init(engine, shaders.textShader);
	objects.textFps.setPos({-0.99f, -0.99f});
	objects.textFps.setDrawPos(TEXT_TOP_LEFT);
	objects.textFps.setFontSize(1.0f);
	objects.textFps.setTextColor({1, 1, 1, 1});
	objects.textFps.setBackgroundColor({0, 0, 0, 0.3});

	objects.textCrossAir.init(engine, shaders.textShader);
	objects.textCrossAir.setText("+");
	objects.textCrossAir.setPos({0.0f, 0.0f});
	objects.textCrossAir.setDrawPos(TEXT_MID_CENTER);
	objects.textCrossAir.setFontSize(1.0f);
	objects.textCrossAir.setTextColor({1, 1, 1, 1});
	objects.textCrossAir.setBackgroundColor({0, 0, 0, 0});

	objects.textPosition.init(engine, shaders.textShader);
	objects.textPosition.setPos({0.99f, -0.99f});
	objects.textPosition.setDrawPos(TEXT_TOP_RIGHT);
	objects.textPosition.setFontSize(1.0f);
	objects.textPosition.setTextColor({1, 1, 1, 1});
	objects.textPosition.setBackgroundColor({0, 0, 0, 0.3});

	objects.displayUi = true;
}
