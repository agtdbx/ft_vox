#ifndef LOOP_HPP
# define LOOP_HPP

# include <define.hpp>
# include <program/shaderStruct.hpp>
# include <engine/engine.hpp>
# include <engine/mesh/Mesh.hpp>
# include <engine/camera/Camera.hpp>
# include <engine/window/Window.hpp>
# include <engine/shader/Shader.hpp>
# include <engine/inputs/InputManager.hpp>
# include <engine/vulkan/VulkanContext.hpp>
# include <engine/textures/TextureManager.hpp>

/**
 * @brief Init function of program.
 *
 * @param engine Engine to init.
 * @param mesh Mesh to init.
 * @param shader Shader to init.
 * @param camera Camera to init.
 *
 * @return True if the init succeed, false else.
 */
bool init(
			Engine &engine,
			Mesh &mesh,
			Shader &shader,
			Camera &camera);
/**
 * @brief Update envents of program.
 *
 * @param engine Engine struct.
 */
void	events(
			Engine &engine);
/**
 * @brief Make computation of program.
 *
 * @param engine Engine struct.
 * @param mesh Mesh to update.
 * @param ubo UBO to update.
 * @param camera Camera to update.
 * @param delta Delta time, previous main loop execution time in second.
 */
void	computation(
			Engine &engine,
			Mesh &mesh,
			UBO3DMesh &ubo,
			Camera &camera,
			double delta);
/**
 * @brief Function to call drawing.
 *
 * @param engine Engine struct.
 * @param mesh Mesh to draw.
 * @param mesh Mesh to draw.
 * @param ubo UBO to use in shader.
 * @param shader Shader used for draw.
 * @param camera Camera used for draw.
 */
void	draw(
			Engine &engine,
			Mesh &mesh,
			UBO3DMesh &ubo,
			Shader &shader,
			Camera &camera);


#endif
