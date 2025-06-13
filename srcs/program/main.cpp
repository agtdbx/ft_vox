/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gugus <gugus@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 15:33:15 by aderouba          #+#    #+#             */
/*   Updated: 2025/06/13 19:22:26 by gugus            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <program/loop/loop.hpp>

#include <iostream>

int	main(void)
{
	if (!glfwInit())
	{
		std::cerr << "Error : GLFW init failed" << std::endl;
		return (1);
	}

	Engine		engine;
	Camera		camera;
	ChunkShader	chunkShader;
	Map			map;

	if (!init(engine, map, chunkShader, camera))
	{
		// Wait all vulkan tasks
		vkDeviceWaitIdle(engine.context.getDevice());

		// Destroy vulkans attributs
		map.destroy(engine);
		chunkShader.destroy(engine);

		// Terminate engine and glfw
		destroyEngine(engine);
		glfwTerminate();

		return (1);
	}

	// Main loop
	double	delta;
	double	currentTime = 0;
	double	lastTime = 0;
	while (!glfwWindowShouldClose(engine.glfwWindow))
	{
		currentTime = glfwGetTime();
		delta = currentTime - lastTime;
		lastTime = currentTime;

		events(engine);

		// Close window on escape
		if (engine.inputManager.escape.isPressed())
			break;

		// Compute part
		computation(engine, map, camera, chunkShader, delta);

		// Drawing part
		draw(engine, map, chunkShader, camera);
	}

	// Wait all vulkan tasks
	vkDeviceWaitIdle(engine.context.getDevice());

	// Destroy vulkans attributs
	map.destroy(engine);
	chunkShader.destroy(engine);

	// Terminate engine and glfw
	destroyEngine(engine);
	glfwTerminate();

	return (0);
}
