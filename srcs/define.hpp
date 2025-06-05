/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   define.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aderouba <aderouba@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/06 12:55:17 by aderouba          #+#    #+#             */
/*   Updated: 2025/06/05 11:39:52 by aderouba         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DEFINE_HPP
# define DEFINE_HPP

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
# define FAR 100.0f
# define SPEED 1.0f
# define SPRINT 5.0f // TODO Put it to x20
# define ROTATE 45.0f

const int MAX_FRAMES_IN_FLIGHT = 2;

#endif
