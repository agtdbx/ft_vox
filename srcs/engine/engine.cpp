# include <engine/engine.hpp>

void	initEngine(Engine &engine)
{
	engine.context.init(engine.commandPool, engine.window);
	engine.glfwWindow = engine.window.getWindow();

	engine.inputManager = InputManager(engine.glfwWindow);

	engine.commandPoolThreads = new VulkanCommandPool[MAP_NB_THREAD];

	for (int i = 0; i < MAP_NB_THREAD; i++)
		engine.commandPoolThreads[i].create(engine.context.getDevice(),
										engine.context.getPhysicalDevice(),
										engine.window.getSurface(),
										engine.context.getTransferQueue());
}


void	destroyEngine(Engine &engine)
{
	engine.textureManager.destroyImages(engine.context.getDevice());
	engine.commandPool.destroy(engine.context.getDevice());

	for (int i = 0; i < MAP_NB_THREAD; i++)
		engine.commandPoolThreads[i].destroy(engine.context.getDevice());

	delete [] engine.commandPoolThreads;

	engine.window.destroy(engine.context.getInstance());
	engine.context.destroy();
	glfwDestroyWindow(engine.glfwWindow);
}
