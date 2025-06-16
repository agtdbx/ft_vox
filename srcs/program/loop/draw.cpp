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

	// End drawing
	engine.window.endDraw(engine.context);
}
