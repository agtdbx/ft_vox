#include <program/loop/loop.hpp>


static void perfLog(
				double delta,
				Camera &camera,
				Window &window);
static void cameraMovements(
				Engine &engine,
				Camera &camera,
				double delta);
static void updateTexts(
				double delta,
				Objects &objects,
				Camera &camera);
static void	destroyBlock(
				Engine &engine,
				Camera &camera,
				Objects &objects);
static void	placeBlock(
				Engine &engine,
				Camera &camera,
				Objects &objects);
static void	updateChunkMesh(
				Engine &engine,
				Map &map,
				int chunkX,
				int chunkY);


void	computation(
			Engine &engine,
			Camera &camera,
			Objects &objects,
			Shaders &shaders,
			double delta)
{
	// perfLog(delta, camera, engine.window);

	cameraMovements(engine , camera, delta);

	updateTexts(delta, objects, camera);

	if (engine.inputManager.f.isPressed())
		objects.displayUi = !objects.displayUi;

	if (engine.inputManager.tab.isPressed())
		shaders.chunkShader.shaderFdfEnable = !shaders.chunkShader.shaderFdfEnable;

	if (engine.inputManager.c.isPressed())
		shaders.chunkShader.shaderBorderEnable = !shaders.chunkShader.shaderBorderEnable;

	if (engine.inputManager.mouse.left.isPressed())
		destroyBlock(engine, camera, objects);
	else if (engine.inputManager.mouse.right.isPressed())
		placeBlock(engine, camera, objects);

	objects.map.update(engine, camera);
}


static void perfLog(
				double delta,
				Camera &camera,
				Window &window)
{
	static double	printFpsTime = 0.0;
	static double	minDelta = 1000.0;
	static double	maxDelta = 0.0;
	static int		nbCall = 0;

	if (minDelta > delta)
		minDelta = delta;

	if (maxDelta < delta)
		maxDelta = delta;

	printFpsTime += delta;
	nbCall++;

	if (printFpsTime > PRINT_FPS_TIME)
	{
		double avgDelta = printFpsTime / (double)nbCall;
		double avgFps = 1.0 / avgDelta;
		double minFps = 1.0 / maxDelta;
		double maxFps = 1.0 / minDelta;

		// Title
		char	stringTitle[50] = {0};
		snprintf(stringTitle,  49, "fps : %7.2f | min %7.2f | max %8.2f",
				avgFps, minFps, maxFps);
		window.setTitle(std::string(stringTitle));

		printFpsTime = 0.0;
		minDelta = 1000.0;
		maxDelta = 0.0;
		nbCall = 0;
	}
}


static void cameraMovements(
				Engine &engine,
				Camera &camera,
				double delta)
{
	// Speed
	float speed = SPEED * delta;
	if (engine.inputManager.lcontrol.isDown())
		speed *= SPRINT;
	float rot = ROTATE * delta;

	// Move
	if (engine.inputManager.w.isDown())
		camera.moveFront(speed);
	else if (engine.inputManager.s.isDown())
		camera.moveFront(-speed);

	if (engine.inputManager.a.isDown())
		camera.moveRight(-speed);
	else if (engine.inputManager.d.isDown())
		camera.moveRight(speed);

	if (engine.inputManager.space.isDown())
		camera.moveUp(speed);
	else if (engine.inputManager.lshift.isDown())
		camera.moveUp(-speed);

	// Rotate with mouse
	gm::Vec2i	windowCenter = engine.window.getSize() / 2;
	gm::Vec2d	mousePos;
	int			mx, my;

	mousePos = engine.inputManager.mouse.getPos();
	engine.inputManager.mouse.goTo(engine.glfwWindow, windowCenter.x, windowCenter.y);

	mx = (int)mousePos.x;
	if (mx != windowCenter.x)
		camera.rotateY((mx - windowCenter.x) * SENSITIVITY * delta);

	my = (int)mousePos.y;
	if (my != windowCenter.y)
		camera.rotateX((windowCenter.y - my) * SENSITIVITY * delta);

	// Rotate with keys
	if (engine.inputManager.down.isDown())
		camera.rotateX(-rot);
	if (engine.inputManager.up.isDown())
		camera.rotateX(rot);

	if (engine.inputManager.left.isDown())
		camera.rotateY(-rot);
	if (engine.inputManager.right.isDown())
		camera.rotateY(rot);

	// Status
	if (engine.inputManager.p.isPressed())
		camera.printStatus();
}


static void updateTexts(
				double delta,
				Objects &objects,
				Camera &camera)
{
	static double	printFpsTime = 0.0;
	static int		nbCall = 0;

	printFpsTime += delta;
	nbCall++;

	if (printFpsTime > UPDATE_TEXT_TIME)
	{
		double avgDelta = printFpsTime / (double)nbCall;
		double avgFps = 1.0 / avgDelta;

		// Fps text
		char	stringFps[50] = {0};
		snprintf(stringFps, 49, "fps : %.2f", avgFps);
		objects.textFps.setText(std::string(stringFps));

		// Position text
		gm::Vec3f	pos = camera.getPosition();
		char	stringPosition[50] = {0};
		snprintf(stringPosition, 49, "pos (%.2f, %.2f, %.2f)",
				pos.x, pos.y, pos.z);
		objects.textPosition.setText(std::string(stringPosition));

		printFpsTime = 0.0;
		nbCall = 0;
	}
}


static void	destroyBlock(
				Engine &engine,
				Camera &camera,
				Objects &objects)
{
	gm::Vec3f	cameraPos = camera.getPosition();
	gm::Vec3f	cameraDir = camera.getFront();

	for (int i = 0; i < MAX_BLOCK_REACH; i++)
	{
		gm::Vec3f	pos = cameraPos + cameraDir * (float)i;
		gm::Vec3f	posId = pos / (float)CHUNK_SIZE;
		gm::Vec2i	chunkId((int)posId.x, (int)posId.z);
		if (posId.x < 0.0f)
			chunkId.x--;
		if (posId.z < 0.0f)
			chunkId.y--;
		Chunk	*currentChunk = objects.map.getChunk(chunkId.x, chunkId.y);

		if (!currentChunk || !currentChunk->isMeshCreated())
			continue;

		int	cubeX = pos.x - (chunkId.x * CHUNK_SIZE);
		int	cubeY = pos.y;
		int	cubeZ = pos.z - (chunkId.y * CHUNK_SIZE);
		Cube	cube = currentChunk->getCube(cubeX, cubeY, cubeZ);

		if (cube == CUBE_AIR || cube == CUBE_WATER)
			continue;

		currentChunk->setCube(cubeX, cubeY, cubeZ, CUBE_AIR);
		currentChunk->updateMesh(engine, objects.map);

		if (cubeX == 0)
			updateChunkMesh(engine, objects.map, chunkId.x - 1, chunkId.y);
		else if (cubeX == CHUNK_MAX)
			updateChunkMesh(engine, objects.map, chunkId.x + 1, chunkId.y);
		if (cubeZ == 0)
			updateChunkMesh(engine, objects.map, chunkId.x, chunkId.y - 1);
		else if (cubeZ == CHUNK_MAX)
			updateChunkMesh(engine, objects.map, chunkId.x, chunkId.y + 1);

		return ;
	}
}


static void	placeBlock(
				Engine &engine,
				Camera &camera,
				Objects &objects)
{
	gm::Vec3f	cameraPos = camera.getPosition();
	gm::Vec3f	cameraDir = camera.getFront();

	int	posSolidBlock = -1;

	for (int i = 0; i < MAX_BLOCK_REACH; i++)
	{
		gm::Vec3f	pos = cameraPos + cameraDir * (float)i;
		gm::Vec3f	posId = pos / (float)CHUNK_SIZE;
		gm::Vec2i	chunkId((int)posId.x, (int)posId.z);
		if (posId.x < 0.0f)
			chunkId.x--;
		if (posId.z < 0.0f)
			chunkId.y--;
		Chunk		*currentChunk = objects.map.getChunk(chunkId.x, chunkId.y);

		if (!currentChunk || !currentChunk->isMeshCreated())
			continue;

		int	cubeX = pos.x - (chunkId.x * CHUNK_SIZE);
		int	cubeY = pos.y;
		int	cubeZ = pos.z - (chunkId.y * CHUNK_SIZE);
		Cube	cube = currentChunk->getCube(cubeX, cubeY, cubeZ);

		if (cube == CUBE_AIR || cube == CUBE_WATER)
			continue;

		posSolidBlock = i;
		break;
	}

	if (posSolidBlock <= 0)
		return ;

	gm::Vec3f	pos = cameraPos + cameraDir * (float)(posSolidBlock - 1);
	gm::Vec3f	posId = pos / (float)CHUNK_SIZE;
	gm::Vec2i	chunkId((int)posId.x, (int)posId.z);
	if (posId.x < 0.0f)
		chunkId.x--;
	if (posId.z < 0.0f)
		chunkId.y--;
	Chunk	*currentChunk = objects.map.getChunk(chunkId.x, chunkId.y);

	if (!currentChunk || !currentChunk->isMeshCreated())
		return ;

	int	cubeX = pos.x - (chunkId.x * CHUNK_SIZE);
	int	cubeY = pos.y;
	int	cubeZ = pos.z - (chunkId.y * CHUNK_SIZE);
	currentChunk->setCube(cubeX, cubeY, cubeZ, CUBE_STONE);
	currentChunk->updateMesh(engine, objects.map);
	if (cubeX == 0)
		updateChunkMesh(engine, objects.map, chunkId.x - 1, chunkId.y);
	else if (cubeX == CHUNK_MAX)
		updateChunkMesh(engine, objects.map, chunkId.x + 1, chunkId.y);
	if (cubeZ == 0)
		updateChunkMesh(engine, objects.map, chunkId.x, chunkId.y - 1);
	else if (cubeZ == CHUNK_MAX)
		updateChunkMesh(engine, objects.map, chunkId.x, chunkId.y + 1);
}


static void	updateChunkMesh(
				Engine &engine,
				Map &map,
				int chunkX,
				int chunkY)
{
	Chunk	*tmpChunk = map.getChunk(chunkX, chunkY);
	if (tmpChunk && tmpChunk->isMeshCreated())
	{
		engine.queueMutex.lock();
		try
		{
			tmpChunk->updateMesh(engine, map);
		}
		catch(const std::exception& e)
		{
			printf("Update chunk %i %i error :\n", chunkX, chunkY);
			std::cerr << e.what() << std::endl;
		}
		engine.queueMutex.unlock();
	}
}
