#include <program/loop/loop.hpp>

void	draw(
			Engine &engine,
			Camera &camera,
			Objects &objects,
			Shaders &shaders)
{
	// Start drawing
	engine.window.startDraw();

	objects.skybox.draw(engine, camera, shaders.skyboxShader);
	objects.map.draw(engine, camera, shaders.chunkShader);

	camera.draw(engine.window, shaders.frustumShader);

	// End drawing
	engine.window.endDraw();
}
