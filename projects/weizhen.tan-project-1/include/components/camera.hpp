/**
@file    camera.hpp
@author  weizhen.tan@digipen.edu
@date    25/05/2025

This file contains the Camera component.

*//*__________________________________________________________________________*/

#ifndef CAMERA_COMP_HPP
#define CAMERA_COMP_HPP
/*                                                                      guard
----------------------------------------------------------------------------- */

#include <math.hpp>
/*                                                                   includes
----------------------------------------------------------------------------- */

enum CAMERA_TYPE
{
    FPS,    ///< WASD for movement, mouse for yaw and pitch
    ORBIT,  ///< always target origin, mouse for yaw and pitch
    CAMERA_TYPE_TOTAL,
};

struct Camera // TODO: change to class..?
{
    CAMERA_TYPE type   { FPS }; ///< changes cam controls
    bool isPerspective { true };
    bool showFrustum   { true }; ///< must have multiple viewports

    float fov   { 60.f };
    float near  { 0.3f };
    float far   { 50.f };

    void UpdateCamera(mat4 _xformMtx, vec2 _windSize)
    {
        vec3 position = vec3(_xformMtx[3][0], _xformMtx[3][1], _xformMtx[3][2]);
        vec3 camForward = vec3(_xformMtx[2][0], _xformMtx[2][1], _xformMtx[2][2]);
        camUp = vec3(_xformMtx[1][0], _xformMtx[1][1], _xformMtx[1][2]);
        vec3 camRight = vec3(_xformMtx[0][0], _xformMtx[0][1], _xformMtx[0][2]);
        target = type == FPS ? position - camForward : vec3(0.f);
         viewMtx = lookAt(position, target, camUp);
        float camAR = _windSize.x / _windSize.y;
         projMtx = isPerspective ? perspective(radians(fov), camAR, near, far) :
            ortho(0.f, _windSize.x, 0.f, _windSize.y, near, far);
        vp = projMtx * viewMtx;
    };

    // for internal use
    bool isFocused{ false }; ///< internal use, currently selected viewport polls
    bool isHovered{ false }; ///< internal use, currently selected viewport polls
    vec3 target;
    vec3 camUp;
    mat4 projMtx;
    mat4 viewMtx; 
    mat4 vp; // proj * view
};

static std::string to_string(CAMERA_TYPE _type)
{
    switch (_type)
    {
    case FPS:   return "FPS";
    case ORBIT: return "Orbit";
    }
    return "";
}

struct Frustum
{
    vec4 planes[6]; // l,r,b,t,n,f
};

#endif /* CAMERA_COMP_HPP */