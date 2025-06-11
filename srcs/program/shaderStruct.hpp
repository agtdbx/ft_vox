/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shaderStruct.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aderouba <aderouba@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/06 12:55:17 by aderouba          #+#    #+#             */
/*   Updated: 2025/06/11 11:14:27 by aderouba         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SHADER_STRUCT_HPP
# define SHADER_STRUCT_HPP

# include <define.hpp>

# include <gmath.hpp>

// Uniform buffer object
struct UBO3DChunkPos {
	gm::Mat4f	model;
	gm::Mat4f	view;
	gm::Mat4f	proj;
	gm::Vec4f	pos;
};

struct UBO3DChunkCubes {
	int	cubes[CHUNK_TOTAL_SIZE];
};

#endif
