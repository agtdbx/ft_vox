/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gugus <gugus@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 15:33:15 by aderouba          #+#    #+#             */
/*   Updated: 2025/06/18 15:30:34 by gugus            ###   ########.fr       */
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
	Objects		objects;
	Shaders		shaders;

	if (!init(engine, camera, objects, shaders))
	{
		// Wait all vulkan tasks
		vkDeviceWaitIdle(engine.context.getDevice());

		// Destroy vulkans attributs
		objects.destroy(engine);
		shaders.destroy(engine);

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
		computation(engine, camera, objects, shaders, delta);

		// Drawing part
		draw(engine, camera, objects, shaders);
	}

	// Wait all vulkan tasks
	vkDeviceWaitIdle(engine.context.getDevice());

	// Destroy vulkans attributs
	objects.destroy(engine);
	shaders.destroy(engine);

	// Terminate engine and glfw
	destroyEngine(engine);
	glfwTerminate();

	return (0);
}
