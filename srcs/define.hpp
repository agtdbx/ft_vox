/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   define.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aderouba <aderouba@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/06 12:55:17 by aderouba          #+#    #+#             */
/*   Updated: 2025/07/03 18:42:04 by aderouba         ###   ########.fr       */
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
# define WIN_W 1600 // TODO: CHANGE TO FULL SCREEN
# define WIN_H 900

const int MAX_FRAMES_IN_FLIGHT = 2;

// Camera defines
# define FOV 80.0f
# define NEAR 0.1f
# define FAR 10000.0f
# define SPEED 5.0f // TODO Put it to 1
# define SPRINT 20.0f
# define ROTATE 45.0f
# define SENSITIVITY 5.0f // 0.0 = no movement, 1.0f is normal, higher is faster (negatif is reverse)

// Skybox defines
const float SKYBOX_DIST = 10.0f;
const float SKYBOX_DIST2 = SKYBOX_DIST * 2.0f;

// Chunk defines
# define CHUNK_SIZE 32
# define CHUNK_HEIGHT 256
# define CHUNK_WATER_LEVEL 70
// # define CHUNK_MIN_TERRAIN_LEVEL 40
// # define CHUNK_MAX_TERRAIN_LEVEL 150
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
# define MAP_SIZE 4096
# define MAX_CHUNK_BUFFER_SIZE 150000
# define MIN_CHUNK_PER_THREAD 4

const int	MAP_NB_THREAD = 7;
const int	MAP_CLUSTER_ARROUND = 1; // Map have x cluster arround center one
const int	MAP_CLUSTER_WIDTH = MAP_CLUSTER_ARROUND + 1 + MAP_CLUSTER_ARROUND;
const int	MAP_CLUSTER_SIZE = MAP_CLUSTER_WIDTH * MAP_CLUSTER_WIDTH;

// Player defines
# define MAX_BLOCK_REACH 10

#endif


/*
TODO: PATCH THIS ERROR

The Vulkan spec states: If the nullDescriptor feature is not enabled, all vertex input bindings accessed via vertex input variables declared in the vertex shader entry point's interface must not be VK_NULL_HANDLE (https://vulkan.lunarg.com/doc/view/1.4.313.0/linux/antora/spec/latest/chapters/drawing.html#VUID-vkCmdDrawIndexed-None-04008)
validation layer: vkCmdBindVertexBuffers(): pBuffers[0] is VK_NULL_HANDLE.
The Vulkan spec states: If the nullDescriptor feature is not enabled, all elements of pBuffers must not be VK_NULL_HANDLE (https://vulkan.lunarg.com/doc/view/1.4.313.0/linux/antora/spec/latest/chapters/fxvertex.html#VUID-vkCmdBindVertexBuffers-pBuffers-04001)
validation layer: vkCmdBindIndexBuffer(): buffer is VK_NULL_HANDLE.
The Vulkan spec states: If the maintenance6 feature is not enabled, buffer must not be VK_NULL_HANDLE (https://vulkan.lunarg.com/doc/view/1.4.313.0/linux/antora/spec/latest/chapters/drawing.html#VUID-vkCmdBindIndexBuffer-None-09493)
validation layer: vkCmdDrawIndexed(): the last bound pipeline has pVertexBindingDescriptions[0].binding (0) which was bound with a buffer of VK_NULL_HANDLE, but nullDescriptor is not enabled.


validation layer: vkQueueSubmit(): pSubmits[0].pSignalSemaphores[0] (VkSemaphore 0xe000000000e) is being signaled by VkQueue 0x2321a630, but it may still be in use by VkSwapchainKHR 0x40000000004.
Here are the most recently acquired image indices: [0], 1, 2.
(brackets mark the last use of VkSemaphore 0xe000000000e in a presentation operation)
Swapchain image 0 was presented but was not re-acquired, so VkSemaphore 0xe000000000e may still be in use and cannot be safely reused with image index 2.
Vulkan insight: One solution is to assign each image its own semaphore. Here are some common methods to ensure that a semaphore passed to vkQueuePresentKHR is not in use and can be safely reused:
	a) Use a separate semaphore per swapchain image. Index these semaphores using the index of the acquired image.
	b) Consider the VK_EXT_swapchain_maintenance1 extension. It allows using a VkFence with the presentation operation.
The Vulkan spec states: Each binary semaphore element of the pSignalSemaphores member of any element of pSubmits must be unsignaled when the semaphore signal operation it defines is executed on the device (https://vulkan.lunarg.com/doc/view/1.4.313.0/linux/antora/spec/latest/chapters/cmdbuffers.html#VUID-vkQueueSubmit-pSignalSemaphores-00067)
validation layer: vkQueueSubmit(): pSubmits[0].pSignalSemaphores[0] (VkSemaphore 0x110000000011) is being signaled by VkQueue 0x2321a630, but it may still be in use by VkSwapchainKHR 0x40000000004.
Here are the most recently acquired image indices: 0, [1], 2, 3.
(brackets mark the last use of VkSemaphore 0x110000000011 in a presentation operation)
Swapchain image 1 was presented but was not re-acquired, so VkSemaphore 0x110000000011 may still be in use and cannot be safely reused with image index 3.
Vulkan insight: One solution is to assign each image its own semaphore. Here are some common methods to ensure that a semaphore passed to vkQueuePresentKHR is not in use and can be safely reused:
	a) Use a separate semaphore per swapchain image. Index these semaphores using the index of the acquired image.
	b) Consider the VK_EXT_swapchain_maintenance1 extension. It allows using a VkFence with the presentation operation.
The Vulkan spec states: Each binary semaphore element of the pSignalSemaphores member of any element of pSubmits must be unsignaled when the semaphore signal operation it defines is executed on the device (https://vulkan.lunarg.com/doc/view/1.4.313.0/linux/antora/spec/latest/chapters/cmdbuffers.html#VUID-vkQueueSubmit-pSignalSemaphores-00067)
validation layer: vkDestroyBuffer(): can't be called on VkBuffer 0x277600000027760 that is currently in use by VkCommandBuffer 0x230524b0.
The Vulkan spec states: All submitted commands that refer to buffer, either directly or via a VkBufferView, must have completed execution (https://vulkan.lunarg.com/doc/view/1.4.313.0/linux/antora/spec/latest/chapters/resources.html#VUID-vkDestroyBuffer-buffer-00922)
validation layer: vkDestroyBuffer(): can't be called on VkBuffer 0x277630000027763 that is currently in use by VkCommandBuffer 0x230524b0.
The Vulkan spec states: All submitted commands that refer to buffer, either directly or via a VkBufferView, must have completed execution (https://vulkan.lunarg.com/doc/view/1.4.313.0/linux/antora/spec/latest/chapters/resources.html#VUID-vkDestroyBuffer-buffer-00922)
validation layer: vkDestroyBuffer(): can't be called on VkBuffer 0x277610000027761 that is currently in use by VkCommandBuffer 0x230524b0.
The Vulkan spec states: All submitted commands that refer to buffer, either directly or via a VkBufferView, must have completed execution (https://vulkan.lunarg.com/doc/view/1.4.313.0/linux/antora/spec/latest/chapters/resources.html#VUID-vkDestroyBuffer-buffer-00922)
validation layer: vkDestroyBuffer(): can't be called on VkBuffer 0x2776a000002776a that is currently in use by VkCommandBuffer 0x230524b0.
The Vulkan spec states: All submitted commands that refer to buffer, either directly or via a VkBufferView, must have completed execution (https://vulkan.lunarg.com/doc/view/1.4.313.0/linux/antora/spec/latest/chapters/resources.html#VUID-vkDestroyBuffer-buffer-00922)
validation layer: vkDestroyBuffer(): can't be called on VkBuffer 0x277700000027770 that is currently in use by VkCommandBuffer 0x233a3b30.
The Vulkan spec states: All submitted commands that refer to buffer, either directly or via a VkBufferView, must have completed execution (https://vulkan.lunarg.com/doc/view/1.4.313.0/linux/antora/spec/latest/chapters/resources.html#VUID-vkDestroyBuffer-buffer-00922)
validation layer: vkDestroyBuffer(): can't be called on VkBuffer 0x277720000027772 that is currently in use by VkCommandBuffer 0x233a3b30.
The Vulkan spec states: All submitted commands that refer to buffer, either directly or via a VkBufferView, must have completed execution (https://vulkan.lunarg.com/doc/view/1.4.313.0/linux/antora/spec/latest/chapters/resources.html#VUID-vkDestroyBuffer-buffer-00922)
validation layer: vkDestroyBuffer(): can't be called on VkBuffer 0x277780000027778 that is currently in use by VkCommandBuffer 0x233a3b30.
The Vulkan spec states: All submitted commands that refer to buffer, either directly or via a VkBufferView, must have completed execution (https://vulkan.lunarg.com/doc/view/1.4.313.0/linux/antora/spec/latest/chapters/resources.html#VUID-vkDestroyBuffer-buffer-00922)
validation layer: vkDestroyBuffer(): can't be called on VkBuffer 0x2777a000002777a that is currently in use by VkCommandBuffer 0x233a3b30.
The Vulkan spec states: All submitted commands that refer to buffer, either directly or via a VkBufferView, must have completed execution (https://vulkan.lunarg.com/doc/view/1.4.313.0/linux/antora/spec/latest/chapters/resources.html#VUID-vkDestroyBuffer-buffer-00922)
validation layer: vkDestroyBuffer(): can't be called on VkBuffer 0x277800000027780 that is currently in use by VkCommandBuffer 0x233a3b30.
The Vulkan spec states: All submitted commands that refer to buffer, either directly or via a VkBufferView, must have completed execution (https://vulkan.lunarg.com/doc/view/1.4.313.0/linux/antora/spec/latest/chapters/resources.html#VUID-vkDestroyBuffer-buffer-00922)
validation layer: (Warning - This VUID has now been reported 10 times, which is the duplicated_message_limit value, this will be the last time reporting it).
vkDestroyBuffer(): can't be called on VkBuffer 0x277820000027782 that is currently in use by VkCommandBuffer 0x233a3b30.
The Vulkan spec states: All submitted commands that refer to buffer, either directly or via a VkBufferView, must have completed execution (https://vulkan.lunarg.com/doc/view/1.4.313.0/linux/antora/spec/latest/chapters/resources.html#VUID-vkDestroyBuffer-buffer-00922)
validation layer: vkAcquireNextImageKHR(): Semaphore must not have any pending operations.
The Vulkan spec states: If semaphore is not VK_NULL_HANDLE, it must not have any uncompleted signal or wait operations pending (https://vulkan.lunarg.com/doc/view/1.4.313.0/linux/antora/spec/latest/chapters/VK_KHR_surface/wsi.html#VUID-vkAcquireNextImageKHR-semaphore-01779)
validation layer: vkResetFences(): pFences[0] (VkFence 0xf000000000f) is in use.
The Vulkan spec states: Each element of pFences must not be currently associated with any queue command that has not yet completed execution on that queue (https://vulkan.lunarg.com/doc/view/1.4.313.0/linux/antora/spec/latest/chapters/synchronization.html#VUID-vkResetFences-pFences-01123)
validation layer: vkResetCommandBuffer(): (VkCommandBuffer 0x233a3b30) is in use.
The Vulkan spec states: commandBuffer must not be in the pending state (https://vulkan.lunarg.com/doc/view/1.4.313.0/linux/antora/spec/latest/chapters/cmdbuffers.html#VUID-vkResetCommandBuffer-commandBuffer-00045)
validation layer: vkBeginCommandBuffer(): on active VkCommandBuffer 0x233a3b30 before it has completed. You must check command buffer fence before this call.
The Vulkan spec states: commandBuffer must not be in the recording or pending state (https://vulkan.lunarg.com/doc/view/1.4.313.0/linux/antora/spec/latest/chapters/cmdbuffers.html#VUID-vkBeginCommandBuffer-commandBuffer-00049)
validation layer: vkQueueSubmit(): pSubmits[0].pCommandBuffers[0] VkCommandBuffer 0x233a3b30 is already in use and is not marked for simultaneous use.
The Vulkan spec states: If any element of the pCommandBuffers member of any element of pSubmits was not recorded with the VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT, it must not be in the pending state (https://vulkan.lunarg.com/doc/view/1.4.313.0/linux/antora/spec/latest/chapters/cmdbuffers.html#VUID-vkQueueSubmit-pCommandBuffers-00071)
terminate called after throwing an instance of 'std::runtime_error'
  what():  Draw command buffer submit failed
Aborted
*/
