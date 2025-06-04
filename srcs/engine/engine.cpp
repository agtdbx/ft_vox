# include <engine/engine.hpp>

void	initEngine(Engine &engine)
{
	engine.context.init(engine.commandPool, engine.window);
	engine.glfwWindow = engine.window.getWindow();

	engine.inputManager = InputManager(engine.glfwWindow);
}


void	destroyEngine(Engine &engine)
{
	engine.textureManager.destroyImages(engine.context.getDevice());
	engine.commandPool.destroy(engine.context.getDevice());
	engine.window.destroy(engine.context.getInstance());
	engine.context.destroy();
	glfwDestroyWindow(engine.glfwWindow);
}
