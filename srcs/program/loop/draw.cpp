#include <program/loop/loop.hpp>

void	draw(
			Engine &engine,
			Map &map,
			ChunkShader &chunkShader,
			Camera &camera,
			Skybox &skybox,
			Shader &skyboxShader)
{
	// Start drawing
	engine.window.startDraw();

	map.draw(engine, camera, chunkShader);
	//skybox.draw(engine, camera, skyboxShader);

	// End drawing
	engine.window.endDraw(engine.context);
}
