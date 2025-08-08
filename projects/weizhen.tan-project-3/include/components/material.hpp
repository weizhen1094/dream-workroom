/**
@file    material.hpp
@author  weizhen.tan@digipen.edu
@date    25/05/2025

This file contains the Material component.

*//*__________________________________________________________________________*/

#ifndef MATERIAL_COMP_HPP
#define MATERIAL_COMP_HPP
/*                                                                      guard
----------------------------------------------------------------------------- */

#include <math.hpp>
/*                                                                   includes
----------------------------------------------------------------------------- */

/**
 * @struct Material
 * @brief This class is responsible for phong material. Default orange. 
 */
struct Material // TODO: read up on std140
{
	vec3 kAmbient{ 0.9f, 0.5f, 0.3f };
	vec3 kDiffuse{ 0.9f, 0.5f, 0.3f };
	vec3 kSpecular{ 0.8f, 0.8f, 0.8f };
	float shininess{ 100.f };
};
#endif /* MATERIAL_COMP_HPP */