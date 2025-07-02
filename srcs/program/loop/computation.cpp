#include <program/loop/loop.hpp>


static void perfLog(
				double delta,
				Window &window);
static void cameraMovements(
				Engine &engine,
				Camera &camera,
				double delta);


void	computation(
			Engine &engine,
			Camera &camera,
			Objects &objects,
			Shaders &shaders,
			double delta)
{
	perfLog(delta, engine.window);

	cameraMovements(engine , camera, delta);

	if (engine.inputManager.tab.isPressed())
		shaders.chunkShader.shaderFdfEnable = !shaders.chunkShader.shaderFdfEnable;

	if (engine.inputManager.c.isPressed())
		shaders.chunkShader.shaderBorderEnable = !shaders.chunkShader.shaderBorderEnable;

	objects.map.update(engine, camera);
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
