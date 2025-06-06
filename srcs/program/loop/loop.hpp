#ifndef LOOP_HPP
# define LOOP_HPP

# include <define.hpp>
# include <engine/engine.hpp>
# include <engine/camera/Camera.hpp>
# include <engine/window/Window.hpp>
# include <engine/shader/Shader.hpp>
# include <engine/inputs/InputManager.hpp>
# include <engine/vulkan/VulkanContext.hpp>
# include <engine/textures/TextureManager.hpp>
# include <program/shaderStruct.hpp>
# include <program/map/Map.hpp>

/**
 * @brief Init function of program.
 *
 * @param engine Engine to init.
 * @param map Map to init.
 * @param chunkShader Shader for chunk to init.
 * @param chunkFdfShader Fdf shader for chunk to init.
 * @param camera Camera to init.
 *
 * @return True if the init succeed, false else.
 */
bool init(
			Engine &engine,
			Map &map,
			Shader &chunkShader,
			Shader &chunkFdfShader,
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
 * @param map Map to update.
 * @param camera Camera to update.
 * @param enableFdfShader Bool for know if use fdf shader.
 * @param delta Delta time, previous main loop execution time in second.
 */
void	computation(
			Engine &engine,
			Map &map,
			Camera &camera,
			bool &enableFdfShader,
			double delta);
/**
 * @brief Function to call drawing.
 *
 * @param engine Engine struct.
 * @param map Map to draw.
 * @param chunkShader Shader used for draw chunks.
 * @param chunkFdfShader Fdf shader for chunk to init.
 * @param enableFdfShader Bool for know if use fdf shader.
 * @param camera Camera used for draw.
 */
void	draw(
			Engine &engine,
			Map &map,
			Shader &chunkShader,
			Shader &chunkFdfShader,
			bool enableFdfShader,
			Camera &camera);


#endif
