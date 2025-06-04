/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shaderStruct.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aderouba <aderouba@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/06 12:55:17 by aderouba          #+#    #+#             */
/*   Updated: 2025/06/04 15:27:31 by aderouba         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SHADER_STRUCT_HPP
# define SHADER_STRUCT_HPP

# include <gmath.hpp>

// Uniform buffer object
struct UBO3DMesh {
	gm::Mat4f	model;
	gm::Mat4f	view;
	gm::Mat4f	proj;
};

#endif
