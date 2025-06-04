#include <program/loop/loop.hpp>

void	events(Engine &engine)
{
	// Update events
	glfwPollEvents();

	engine.inputManager.update(engine.glfwWindow);
}
