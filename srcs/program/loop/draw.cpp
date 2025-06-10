#include <program/loop/loop.hpp>

void	draw(
			Engine &engine,
			Map &map,
			ChunkShader &chunkShader,
			Camera &camera)
{
	// Start drawing
	engine.window.startDraw();

	map.draw(engine, camera, chunkShader);

	// End drawing
	engine.window.endDraw(engine.context);
}
