#ifndef ENGINE_HPP
# define ENGINE_HPP

# include <define.hpp>

# include <engine/window/Window.hpp>
# include <engine/inputs/InputManager.hpp>
# include <engine/vulkan/VulkanContext.hpp>
# include <engine/textures/TextureManager.hpp>
# include <engine/vulkan/VulkanCommandPool.hpp>

struct Engine
{
	VulkanContext		context;
	VulkanCommandPool	commandPool;
	Window				window;
	GLFWwindow			*glfwWindow;
	TextureManager		textureManager;
	InputManager		inputManager;
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

#endif
