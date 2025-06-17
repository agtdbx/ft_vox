#include <program/loop/loop.hpp>


static void perfLog(
				double delta,
				Window &window);
static void cameraMovements(
				InputManager &inputManager,
				Camera &camera,
				double delta);


void	computation(
			Engine &engine,
			Camera &camera,
			Objects &objects,
			Shaders &shaders,
			double delta)
{
	InputManager &inputManager = engine.inputManager;
	ChunkShader	&chunkShader = shaders.chunkShader;

	perfLog(delta, engine.window);

	cameraMovements(inputManager, camera, delta);

	if (inputManager.tab.isPressed())
		chunkShader.shaderFdfEnable = !chunkShader.shaderFdfEnable;

	if (inputManager.c.isPressed())
		chunkShader.shaderBorderEnable = !chunkShader.shaderBorderEnable;

	objects.map.update(camera);
}


static void perfLog(
				double delta,
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
		double avgFps = 1 / avgDelta;
		double minFps = 1 / maxDelta;
		double maxFps = 1 / minDelta;

		char	string[50] = {0};

		sprintf(string, "fps : %7.2f | min %7.2f | max %8.2f",
				avgFps, minFps, maxFps);

		window.setTitle(std::string(string));

		printFpsTime = 0.0;
		minDelta = 1000.0;
		maxDelta = 0.0;
		nbCall = 0;
	}
}


static void cameraMovements(
				InputManager &inputManager,
				Camera &camera,
				double delta)
{
	// Speed
	float speed = SPEED * delta;
	if (inputManager.lcontrol.isDown())
		speed *= SPRINT;
	float rot = ROTATE * delta;

	// Move
	if (inputManager.w.isDown())
		camera.moveFront(speed);
	else if (inputManager.s.isDown())
		camera.moveFront(-speed);

	if (inputManager.a.isDown())
		camera.moveRight(speed);
	else if (inputManager.d.isDown())
		camera.moveRight(-speed);

	if (inputManager.space.isDown())
		camera.moveUp(speed);
	else if (inputManager.lshift.isDown())
		camera.moveUp(-speed);

	// Rotate
	if (inputManager.down.isDown())
		camera.rotateX(-rot);
	if (inputManager.up.isDown())
		camera.rotateX(rot);

	if (inputManager.left.isDown())
		camera.rotateY(-rot);
	if (inputManager.right.isDown())
		camera.rotateY(rot);

	// Status
	if (inputManager.p.isPressed())
		camera.printStatus();
}
