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
#include <window.hpp>
#include <components/boundingvolume.hpp>
#include <components/renderable.hpp>
/*                                                                   includes
----------------------------------------------------------------------------- */


void Movement::Init()
{

}

void Movement::Update()
{
    bool isM1Down = Input::GetInstance().MouseDown(MOUSE_BUTTON_LEFT);

    if (!isM1Down) { UpdateCurrentSelectedEntity(); }

    UpdateBV();

    auto viewCamera = ECS.registry().view<Transform, Camera>();
    viewCamera.each([&isM1Down](Transform& _xform, Camera& _cam)
    {
        if (_cam.isFocused) // poll for input
        {
            // toggle camera type
            if (Input::GetInstance().KeyTriggered(KEY_C)) 
            { _cam.type = _cam.type == ORBIT ? FPS : ORBIT; }

            // camera view and pos
            if (isM1Down)
            {
            switch (_cam.type)
            {
            case FPS:       
            {
                mat4 xformMtx = _xform.getMtx();
                vec3 camForward = vec3(xformMtx[2][0], xformMtx[2][1], xformMtx[2][2]);
                //vec3 camUp = vec3(xformMtx[1][0], xformMtx[1][1], xformMtx[1][2]);
                vec3 camRight = vec3(xformMtx[0][0], xformMtx[0][1], xformMtx[0][2]);
                static float camSpeed{0.1f}; // TODO: save in settings
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
        }
        if (_cam.isHovered)
        {
            // camera zoom
            if (Input::GetInstance().GetMouseScrollOffset().y != 0.f)
            {
                float scrollOffset = Input::GetInstance().GetMouseScrollOffset().y;
                static float zoomSpeed{ 0.5f };
                switch (_cam.type)
                {
                case FPS:
                {
                    mat4 xformMtx = _xform.getMtx();
                    vec3 camForward = vec3(xformMtx[2][0], xformMtx[2][1], xformMtx[2][2]);
                    _xform.position += zoomSpeed * scrollOffset * camForward;
                } break;
                case::ORBIT:
                {
                    float r = length(_xform.position);
                    float pitch = asin(_xform.position.y / r); // pitch
                    float yaw = atan2f(_xform.position.x, _xform.position.z); // yaw;
                    r += scrollOffset;
                    r = clamp(r, 1.0f, r);
                    _xform.position.x = r * cos(pitch) * sin(yaw);
                    _xform.position.y = r * sin(pitch);
                    _xform.position.z = r * cos(pitch) * cos(yaw);
                } break;
                }
            } Input::GetInstance().SetMouseScrollOffset(vec2(0));
        }
    });
}

void Movement::UpdateCurrentSelectedEntity()
{
    static float moveSpeed{ 3.f };
    static float rotSpeed{ 50.f };
    if (ECS.IsEntityValid(ECS.selectedEnt()))
    {
        auto& xform = ECS.registry().get<Transform>(ECS.selectedEnt());
        //if (!xform.hasCollided)
        //{
        if (Input::GetInstance().KeyDown(KEY_W))
        {  xform.position.z -= moveSpeed * Window::GetCurrentWindowDt(); xform.isDirty = true; }
        if (Input::GetInstance().KeyDown(KEY_S))
        {  xform.position.z += moveSpeed * Window::GetCurrentWindowDt(); xform.isDirty = true; }
        if (Input::GetInstance().KeyDown(KEY_A))
        {  xform.position.x -= moveSpeed * Window::GetCurrentWindowDt(); xform.isDirty = true; }
        if (Input::GetInstance().KeyDown(KEY_D))
        { xform.position.x += moveSpeed * Window::GetCurrentWindowDt(); xform.isDirty = true; }
        if (Input::GetInstance().KeyDown(KEY_Q))
        { xform.position.y += moveSpeed * Window::GetCurrentWindowDt(); xform.isDirty = true; }
        if (Input::GetInstance().KeyDown(KEY_E))
        { xform.position.y -= moveSpeed * Window::GetCurrentWindowDt(); xform.isDirty = true; }
        if (Input::GetInstance().KeyDown(KEY_UP))
        { xform.rotation.x -= rotSpeed * Window::GetCurrentWindowDt(); xform.isDirty = true; }
        if (Input::GetInstance().KeyDown(KEY_DOWN))
        { xform.rotation.x += rotSpeed * Window::GetCurrentWindowDt(); xform.isDirty = true; }
        if (Input::GetInstance().KeyDown(KEY_LEFT))
        { xform.rotation.y -= rotSpeed * Window::GetCurrentWindowDt(); xform.isDirty = true; }
        if (Input::GetInstance().KeyDown(KEY_RIGHT))
        { xform.rotation.y += rotSpeed * Window::GetCurrentWindowDt(); xform.isDirty = true; }
        if (Input::GetInstance().KeyDown(KEY_COMMA))
        { xform.rotation.z += rotSpeed * Window::GetCurrentWindowDt(); xform.isDirty = true; }
        if (Input::GetInstance().KeyDown(KEY_PERIOD))
        { xform.rotation.z -= rotSpeed * Window::GetCurrentWindowDt(); xform.isDirty = true;}
        //} 
    }
}

void Movement::UpdateBV()
{
    ECS.registry().view<Transform, BVList>().each([](auto _ent, Transform& _xform, BVList& _bvList)
        {
            for (auto& bv : _bvList)
            {
                if (bv == nullptr || !bv->isActive) { continue; }
                // update xform mtx accrd to current ent xform 
                if (_xform.isDirty)
                {
                    bool hasMdl = ECS.registry().view<Renderable>().contains(_ent);
                    if (hasMdl) { bv->InitBV(ECS.registry().get<Renderable>(_ent).GetMeshType()); }
                    bv->UpdateBV(_xform);
                }
            }
        });
}

void Movement::CleanUp()
{

}