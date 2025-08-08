/**
@file    transform.hpp
@author  weizhen.tan@digipen.edu
@date    25/05/2025

This file contains the compulsory components for an entity, Transform, RenderOrder
and EntityName.

*//*__________________________________________________________________________*/

#ifndef TRANSFORM_HPP
#define TRANSFORM_HPP
/*                                                                      guard
----------------------------------------------------------------------------- */

#include <math.hpp>
/*                                                                   includes
----------------------------------------------------------------------------- */

struct Transform 
{
    vec3 position;
    vec3 rotation; ///< in degrees
    vec3 scale;
    mat4 getRotMtx()
    { 
        mat4 rotX = rotate(radians(rotation.x), vec3(1.0f, 0.0f, 0.0f));
        mat4 rotY = rotate(radians(rotation.y), vec3(0.0f, 1.0f, 0.0f));
        mat4 rotZ = rotate(radians(rotation.z), vec3(0.0f, 0.0f, 1.0f));
        mat4 rot = rotZ * rotY * rotX;
        return rot;
    };
    mat4 getMtx()
    {
        mat4 trans = translate(mat4(1.0f), position);
        mat4 sc = glm::scale(mat4(1.0f), scale);
        return trans * getRotMtx() * sc;
    };
    bool isDirty{ false };
    bool hasCollided{ false };
};

/**
 * @struct RenderOrder
 * @brief This class is responsible for scene hierarchy order of entities and rendering order of 
 * TODO: UI elements
 */
struct RenderOrder { unsigned value; };

struct EntityName  { std::string value; };

#endif /* TRANSFORM_HPP */