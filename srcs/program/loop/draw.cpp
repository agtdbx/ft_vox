#include <program/loop/loop.hpp>

void	draw(
			Engine &engine,
			Map &map,
			Shader &shader,
			Camera &camera)
{
	// Start drawing
	engine.window.startDraw();

	map.draw(engine, camera, shader);

	// End drawing
	engine.window.endDraw(engine.context);
}
