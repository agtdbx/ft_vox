#include <program/loop/loop.hpp>

void	draw(
			Engine &engine,
			Map &map,
			Shader &chunkShader,
			Shader &chunkFdfShader,
			bool enableFdfShader,
			Camera &camera)
{
	// Start drawing
	engine.window.startDraw();

	if (enableFdfShader)
		map.draw(engine, camera, chunkFdfShader);
	else
		map.draw(engine, camera, chunkShader);

	// End drawing
	engine.window.endDraw(engine.context);
}
