#include <program/loop/loop.hpp>

void	draw(
			Engine &engine,
			Camera &camera,
			Objects &objects,
			Shaders &shaders)
{
	// Start drawing
	engine.window.startDraw();

	try
	{
		objects.skybox.draw(engine, camera, shaders.skyboxShader);
		objects.map.draw(engine, camera, shaders.chunkShader);
		if (objects.displayUi)
		{
			objects.textFps.draw(engine, shaders.textShader);
			objects.textCrossAir.draw(engine, shaders.textShader);
			objects.textPosition.draw(engine, shaders.textShader);
		}
	}
	catch(const std::exception& e)
	{
	}

	// End drawing
	engine.window.endDraw();
}
