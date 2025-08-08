/**
@file    movement.cpp
@author  weizhen.tan@digipen.edu
@date    25/05/2025

This file contains the definition of the Movement system class.

*//*__________________________________________________________________________*/

#include <systems/movement.hpp>
#include <input.hpp>
#include <ecs.hpp>
#include <components/camera.hpp>
#include <components/transform.hpp>
/*                                                                   includes
----------------------------------------------------------------------------- */

void Movement::Init()
{

}

void Movement::Update()
{
    auto viewCamera = ECS.registry().view<Transform, Camera>();
    viewCamera.each([](Transform& _xform, Camera& _cam)
    {
        if (_cam.isActive) // poll for input
        {
            static bool isM1Down{ false };
            isM1Down = Input::GetInstance().MouseDown(MOUSE_BUTTON_LEFT);

            if (isM1Down)
            {
            switch (_cam.type)
            {
            case FPS:       
            {
                mat4 xformMtx = _xform.getMtx();
                vec3 camForward = vec3(xformMtx[2][0], xformMtx[2][1], xformMtx[2][2]);
                vec3 camUp = vec3(xformMtx[1][0], xformMtx[1][1], xformMtx[1][2]);
                vec3 camRight = vec3(xformMtx[0][0], xformMtx[0][1], xformMtx[0][2]);
                static float camSpeed{0.1f};
                if (KEYDOWN(KEY_W)) { _xform.position -= camSpeed * camForward; }
                if (KEYDOWN(KEY_S)) { _xform.position += camSpeed * camForward; }
                if (KEYDOWN(KEY_A)) { _xform.position -= camSpeed * normalize(camRight); }
                if (KEYDOWN(KEY_D)) { _xform.position += camSpeed * normalize(camRight); }
                static float camSensitivity{ 0.1f };
                _xform.rotation.x -= camSensitivity * Input::GetInstance().GetMouseDelta().y;
                _xform.rotation.y -= camSensitivity * Input::GetInstance().GetMouseDelta().x;
                // clamp pitch 
                _xform.rotation.x = clamp(_xform.rotation.x, -89.0f, 89.0f);
            } break;
            case ORBIT:    
            {
                static float camSensitivityX{ 0.05f };
                static float camSensitivityY{ 0.02f };
                const float r = length(_xform.position); 
                float pitch = asin(_xform.position.y / r); // pitch
                float yaw = atan2f(_xform.position.x, _xform.position.z); // yaw
                if (Input::GetInstance().GetMouseDelta().y < 0.0) {  pitch -= camSensitivityY; }
                else if (Input::GetInstance().GetMouseDelta().y > 0.0) { pitch += camSensitivityY; }
                if (Input::GetInstance().GetMouseDelta().x < 0.0) {  yaw += camSensitivityX; }
                else if (Input::GetInstance().GetMouseDelta().x > 0.0) { yaw -= camSensitivityX;  }
                // clamp pitch
                float halfpi = static_cast<float>(std::numbers::pi) * 0.5f;
                pitch = clamp(pitch, -halfpi + 0.01f, halfpi - 0.01f);
                _xform.position.x = r * cos(pitch) * sin(yaw);
                _xform.position.y = r * sin(pitch);
                _xform.position.z = r * cos(pitch) * cos(yaw);
            }  break;
            }
            }
            // mouse wheel for scroll
            //void onScroll(double xoffset, double yoffset, Program * pProgram = nullptr)
            //{
            //    if (cameraType == CameraType::ORBITING)
            //    {
            //        // Calculate the distance from the origin and adjust it based on scroll input
            //        float r = glm::sqrt(position.x * position.x + position.y * position.y + position.z * position.z);
            //        const float alpha = glm::asin(position.y / r);
            //        const float betta = std::atan2f(position.x, position.z);

            //        r += yoffset > 0.0f ? -1.0f : 1.0f; // Zoom in or out
            //        if (r < 1.0f) r = 1.0f; // Clamp minimum distance

            //        // Update position based on new distance
            //        position.x = r * glm::cos(alpha) * glm::sin(betta);
            //        position.y = r * glm::sin(alpha);
            //        position.z = r * glm::cos(alpha) * glm::cos(betta);
            //    }
            //    else if (cameraType == CameraType::WALKING)
            //    {
            //        // Adjust position and target based on scroll input for walking
            //        const glm::vec3 velocity = glm::normalize(yoffset > 0.0f ? target - position : position - target);
            //        position += velocity * glm::vec3(1.0f, 0.0f, 1.0f);
            //        target += velocity * glm::vec3(1.0f, 0.0f, 1.0f);
            //    }

            //    // Update shader program with the new camera settings
            //    setUniforms(pProgram);
            //}
        }
    });
}

void Movement::CleanUp()
{

}