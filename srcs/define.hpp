/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   define.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gugus <gugus@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/06 12:55:17 by aderouba          #+#    #+#             */
/*   Updated: 2025/06/10 16:25:19 by gugus            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DEFINE_HPP
# define DEFINE_HPP

// Standar libs
# include <string>
# include <vector>

// GLFW and Vulkan
# define VK_USE_PLATFORM_XCB_KHR
# define GLFW_INCLUDE_VULKAN
# include <GLFW/glfw3.h>
# define GLFW_EXPOSE_NATIVE_XCB
# include <GLFW/glfw3native.h>

// Global defines
# define PRINT_FPS_TIME 1.0

// Window defines
# define WIN_TITLE "scop"
# define ENGINE_TITLE "gvEngine"
# define WIN_W 1600 // TODO: CHANGE TO FULL SCREEN
# define WIN_H 900

// Camera defines
# define FOV 80.0f
# define NEAR 0.1f
# define FAR 1000.0f
# define SPEED 3.0f // TODO Put it to 1
# define SPRINT 20.0f
# define ROTATE 45.0f

const int MAX_FRAMES_IN_FLIGHT = 2;

// Chunk defines
# define CHUNK_SIZE 32
# define CHUNK_HEIGHT 256
const int	CHUNK_SIZE2 = CHUNK_SIZE * CHUNK_SIZE;
const int	CHUNK_TOTAL_SIZE = CHUNK_SIZE2 * CHUNK_HEIGHT;

const std::vector<std::string>	CUBE_TEXTURES = {
	"grass-up", "dirt-up", "stone-up", "water-up", "snow-up",
	"ice-up", "sand-up", "lava-up", "iron-up", "diamond-up",
	"grass-side", "dirt-side", "stone-side", "water-side", "snow-side",
	"ice-side", "sand-side", "lava-side", "iron-side", "diamond-side",
	"grass-down", "dirt-down", "stone-down", "water-down", "snow-down",
	"ice-down", "sand-down", "lava-down", "iron-down", "diamond-down"
};

// Cluster defines
# define CLUSTER_SIZE 8
const int	CLUSTER_SIZE2 = CLUSTER_SIZE * CLUSTER_SIZE;

#endif
