#ifndef ENGINE_HPP
# define ENGINE_HPP

# include <define.hpp>

# include <engine/window/Window.hpp>
# include <engine/inputs/InputManager.hpp>
# include <engine/vulkan/VulkanContext.hpp>
# include <engine/textures/TextureManager.hpp>
# include <engine/vulkan/VulkanCommandPool.hpp>

# include <mutex>

struct Engine
{
	VulkanContext		context;
	VulkanCommandPool	commandPool;
	VulkanCommandPool	*commandPoolThreads;
	Window				window;
	GLFWwindow			*glfwWindow;
	TextureManager		textureManager;
	InputManager		inputManager;
	std::mutex			queueMutex;
};

/**
 * @brief Init engine.
 *
 * @param engine Reference of engine struct.
 */
void	initEngine(Engine &engine);
/**
 * @brief Destyo engine for avoid leak.
 *
 * @param engine Reference of engine struct.
 */
void	destroyEngine(Engine &engine);

// # include <engine/window/WindowDrawMesh.hpp>

#endif
