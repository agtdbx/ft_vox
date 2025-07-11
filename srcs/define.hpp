/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   define.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aderouba <aderouba@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/06 12:55:17 by aderouba          #+#    #+#             */
/*   Updated: 2025/07/11 14:38:41 by aderouba         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DEFINE_HPP
# define DEFINE_HPP

// Standar libs
# include <string>
# include <vector>
# include <thread>

// GLFW and Vulkan
# define VK_USE_PLATFORM_XCB_KHR
# define GLFW_INCLUDE_VULKAN
# include <GLFW/glfw3.h>
# define GLFW_EXPOSE_NATIVE_XCB
# include <GLFW/glfw3native.h>

// Global defines
# define PRINT_FPS_TIME 1.0
# define MAX_FPS 500.0
const float MIN_DELTA = 1.0 / MAX_FPS;

// Window defines
# define WIN_TITLE "vox"
# define ENGINE_TITLE "gvEngine"
# define WIN_W 1920
# define WIN_H 1080

const int MAX_FRAMES_IN_FLIGHT = 1;

// Camera defines
# define FOV 80.0f
# define NEAR 0.1f
# define FAR 10000.0f
# define SPEED 1.0f
# define SPRINT 20.0f
# define ROTATE 45.0f
# define SENSITIVITY 5.0f // 0.0 = no movement, 1.0f is normal, higher is faster (negatif is reverse)

// Skybox defines
const float SKYBOX_DIST = 10.0f;
const float SKYBOX_DIST2 = SKYBOX_DIST * 2.0f;

// Chunk defines
# define CHUNK_SIZE 32
# define CHUNK_HEIGHT 256
# define CHUNK_LIQUID_LEVEL 64
const int	CHUNK_MAX = CHUNK_SIZE - 1;
const int	CHUNK_MAX_H = CHUNK_HEIGHT - 1;
const int	CHUNK_SIZE2 = CHUNK_SIZE * CHUNK_SIZE;
const int	CHUNK_TOTAL_SIZE = CHUNK_SIZE2 * CHUNK_HEIGHT;
const int	CHUNK_MASK_SIZE = CHUNK_SIZE * CHUNK_HEIGHT;

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

// Map defines
// # define SEED 42
# define MAP_SIZE 4096
# define MAX_CHUNK_BUFFER_SIZE 150000
# define MIN_CHUNK_PER_THREAD 4

const int	MAP_NB_THREAD = 7;
const int	MAP_CLUSTER_ARROUND = 2; // Map have x cluster arround center one
const int	MAP_CLUSTER_WIDTH = MAP_CLUSTER_ARROUND + 1 + MAP_CLUSTER_ARROUND;
const int	MAP_CLUSTER_SIZE = MAP_CLUSTER_WIDTH * MAP_CLUSTER_WIDTH;

// Player defines
# define MAX_BLOCK_REACH 10

// Ui define
# define UPDATE_TEXT_TIME 0.2

#endif
