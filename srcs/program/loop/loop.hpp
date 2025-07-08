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
# include <engine/ui/Text.hpp>
# include <program/shaderStruct.hpp>
# include <program/map/Map.hpp>
# include <program/skybox/Skybox.hpp>


struct Objects
{
	Map		map;
	Skybox	skybox;
	Text	fpsText;
	bool	displayFps;

	/**
	 * @brief Destroy all object in struct.
	 *
	 * @param engine Engine to init.
	 */
	void	destroy(Engine &engine)
	{
		map.destroy(engine);
		skybox.destroy(engine);
		fpsText.destroy(engine);
	}
};

struct Shaders
{
	ChunkShader	chunkShader;
	Shader		skyboxShader;
	Shader		textShader;

	/**
	 * @brief Destroy all shader in struct.
	 *
	 * @param engine Engine to init.
	 */
	void	destroy(Engine &engine)
	{
		chunkShader.destroy(engine);
		skyboxShader.destroy(engine);
		textShader.destroy(engine);
	}
};

/**
 * @brief Init function of program.
 *
 * @param engine Engine to init.
 * @param camera Camera to init.
 * @param objects Struct that contain all objects.
 * @param shaders Struct that contain all shaders.
 *
 * @return True if the init succeed, false else.
 */
bool init(
		Engine &engine,
		Camera &camera,
		Objects &objects,
		Shaders &shaders);
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
 * @param camera Camera to update.
 * @param objects Struct that contain all objects.
 * @param shaders Struct that contain all shaders.
 * @param delta Delta time, previous main loop execution time in second.
 */
void	computation(
			Engine &engine,
			Camera &camera,
			Objects &objects,
			Shaders &shaders,
			double delta);
/**
 * @brief Function to call drawing.
 *
 * @param engine Engine struct.
 * @param camera Camera used for draw.
 * @param objects Struct that contain all objects.
 * @param shaders Struct that contain all shaders.
 */
void	draw(
			Engine &engine,
			Camera &camera,
			Objects &objects,
			Shaders &shaders);


#endif
