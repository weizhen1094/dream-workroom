/**
@file    viewportgui.cpp
@author  weizhen.tan@digipen.edu
@date    19/05/2025

This file contains the definitions of the GUI class and helper functions that
encapsulates all Dear ImGui logic.

*//*__________________________________________________________________________*/

#include <gui/viewportgui.hpp>
#include <gui/guisettings.hpp>

#include <components/transform.hpp>
#include <components/camera.hpp>
/*                                                                   includes
----------------------------------------------------------------------------- */

void ViewportGUI::InitWindow()
{
    m_GUIWindowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

}

void ViewportGUI::Render()
{
    ImGui::SetNextWindowPos({ m_GUIWindowPos.x, m_GUIWindowPos.y }, ImGuiCond_Once);
    ImGui::SetNextWindowSize({ m_GUIWindowSize.x, m_GUIWindowSize.y }, ImGuiCond_Once);
    if (ImGui::Begin(m_GUIWindowTitle.c_str(), &m_IsWindowOpen, m_GUIWindowFlags))
    {
        ECS.registry().get<Camera>(m_Camera).isActive = ImGui::IsWindowFocused() /* && ImGui::IsWindowHovered()*/;

        if (m_GUIWindowSize.x != ImGui::GetWindowSize().x || // if window resized reset fbo tex
            m_GUIWindowSize.y != ImGui::GetWindowSize().y)
        {
            m_GUIWindowSize = vec2(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y);
            ECS.registry().get<Framebuffer>(m_Camera).CreateFBO(m_GUIWindowSize);
        }

        ImGui::Image((ImTextureID)(ECS.registry().get<Framebuffer>(m_Camera).GetFramebufferTexture()), { m_GUIWindowSize.x - WINDOW_PADDING*2, m_GUIWindowSize.y - WINDOW_PADDING*2 }, ImVec2(0, 1), ImVec2(1, 0));
    }
    ImGui::End();
}

void ViewportGUI::CleanUp() 
{
    ECS.DestroyEntity(m_Camera);
}

ViewportGUI::ViewportGUI(vec3 _camPos, vec3 _camRot, float _camNear, float _camFar, std::string _GUIWindowTitle, vec2 _GUIWindowPos, vec2 _GUIWindowSize)
    : IGUIWindow(_GUIWindowTitle, _GUIWindowPos, _GUIWindowSize)
{
    // create viewport camera gui
    m_Camera = ECS.CreateCameraEntity();
    ECS.registry().get<EntityName>(m_Camera).value = "Camera - " + m_GUIWindowTitle;
    ECS.registry().get<Framebuffer>(m_Camera).CreateFBO(m_GUIWindowSize);
    ECS.registry().get<Transform>(m_Camera).position = _camPos;
    ECS.registry().get<Transform>(m_Camera).rotation = _camRot;
    ECS.registry().get<Camera>(m_Camera).near = _camNear;
    ECS.registry().get<Camera>(m_Camera).far = _camFar;
}