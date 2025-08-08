/* !
@file    math.hpp
@author  weizhen.tan@digipen.edu
@date    08/05/2025

This file contains glm includes for math.

*//*__________________________________________________________________________*/

#ifndef MATH_HPP
#define MATH_HPP
/*                                                                      guard
----------------------------------------------------------------------------- */
 
#define GLM_ENABLE_EXPERIMENTAL // for string cast
 
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/epsilon.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/integer.hpp>
#include <glm/gtx/orthonormalize.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtx/string_cast.hpp>
#include <numbers>
/*                                                                   includes
----------------------------------------------------------------------------- */

// for easy access 
using glm::ivec2;
using glm::ivec3;
using glm::ivec4;
using glm::mat2;
using glm::mat3;
using glm::mat4; 
using glm::quat;
using glm::vec2;
using glm::vec3;
using glm::vec4;
 
using namespace glm;

 #endif /* MATH_HPP */
 