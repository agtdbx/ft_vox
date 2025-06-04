#include <program/loop/loop.hpp>

void	draw(
			Engine &engine,
			Mesh &mesh,
			UBO3DMesh &ubo,
			Shader &shader,
			Camera &camera)
{
	// Start drawing
	engine.window.startDraw();

	shader.updateUBO(engine.window, &ubo);
	engine.window.drawMesh(mesh, shader);

	// End drawing
	engine.window.endDraw(engine.context);
}
