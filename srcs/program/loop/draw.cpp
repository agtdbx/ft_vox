#include <program/loop/loop.hpp>

void	draw(
			Engine &engine,
			Map &map,
			Shader *chunkShaders,
			Camera &camera)
{
	// Start drawing
	engine.window.startDraw();

	map.draw(engine, camera, chunkShaders);

	// End drawing
	engine.window.endDraw(engine.context);
}
