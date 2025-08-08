/**
@file    scenehierarchygui.cpp
@author  weizhen.tan@digipen.edu
@date    19/05/2025

This file contains the definitions of the SceneHierarchyGUI class.

*//*__________________________________________________________________________*/

#include <gui/scenehierarchygui.hpp>
#include <gui/guisettings.hpp>
#include <ecs.hpp>
#include <components/transform.hpp>
#include <components/camera.hpp>
#include <components/light.hpp>
#include <components/boundingvolume.hpp>
#include <components/renderable.hpp>
/*                                                                   includes
----------------------------------------------------------------------------- */


void SceneHierarchyGUI::InitWindow() 
{
    
}

void SceneHierarchyGUI::Render()
{
    ImGui::SetNextWindowPos({ m_GUIWindowPos.x, m_GUIWindowPos.y }, ImGuiCond_Once);
    ImGui::SetNextWindowSize({ m_GUIWindowSize.x, m_GUIWindowSize.y }, ImGuiCond_Once);

    if (ImGui::Begin(m_GUIWindowTitle.c_str(), &m_IsWindowOpen, m_GUIWindowFlags))
    {
        DisplayEntities();
        CreateEntityMenu();
        ClearSceneBtn();
        //ToggleBoundingVolumes();
    }
    ImGui::End();
}

void SceneHierarchyGUI::CleanUp() {  ClearScene(); }

void SceneHierarchyGUI::DisplayEntities()
{
    // sort 
    ECS.registry().sort<RenderOrder>([](const auto& lhs, const auto& rhs)
        { return lhs.value < rhs.value; });

    ImGui::PushItemFlag(ImGuiItemFlags_AllowDuplicateId, true);

    // loop through RenderOrder comp and display entity name
    auto viewCamera = ECS.registry().view<Camera>();
    auto view = ECS.registry().view<RenderOrder, EntityName>();
    static entt::entity prev;  static bool onRightClick = false;
    for (auto [_ent, _order, _name] : view.each()) 
    {
        if (_name.value == "BVHTree") { continue; } // hide bvh tree ent
        static bool orderChanged = false;
        if (orderChanged && _order.value > 0) { --_order.value; orderChanged = false; }

        std::string t = '#'+std::to_string(_order.value) + ": " + _name.value; 

        // display ent name
        // (and render order for now 
        // TODO: delete when dragging issue solved)
        if (ImGui::Selectable(t.c_str()/*_name.value.c_str()*/))
        {  ECS.selectedEnt(_ent); }

        // on right click open popup menu
        if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right))
        {
            ECS.selectedEnt(_ent);
            if (!viewCamera.contains(_ent)) 
                // entities with camera are destroyed only when viewport window is closed
            { 
                ImGui::OpenPopup("RightClickedOnEntity", ImGuiPopupFlags_MouseButtonRight); 
                onRightClick = true;
            }
            else { onRightClick = false; }
        }

        // on drag
        if (ImGui::IsItemActive() && !ImGui::IsItemHovered())
        {
            if (ImGui::GetMouseDragDelta(0).y < 0.f)
            {
                // set prev render order to this
                ECS.registry().get<RenderOrder>(prev).value = _order.value;
            }
            else // set next render order to this
            { orderChanged = true; }
            int n_next = _order.value + (ImGui::GetMouseDragDelta(0).y < 0.f ? -1 : 1);
            if (n_next >= 0 && n_next <= ECS.count())
            {
                _order.value = n_next;
                ImGui::ResetMouseDragDelta();
            }
        }
        prev = _ent;
    }
    ImGui::PopItemFlag();

    if (onRightClick)
    {
        if (ImGui::BeginPopupContextItem("RightClickedOnEntity"))
        {
            if (ImGui::Selectable("Destroy Entity"))
            { ECS.DestroyEntity(ECS.selectedEnt()); }
            ImGui::EndPopup();
        }
        else { ImGui::CloseCurrentPopup(); }
    }
}

void SceneHierarchyGUI::CreateEntityMenu()
{
    if (ImGui::CollapsingHeader("Add Entity", ImGuiTreeNodeFlags_None))
    {
        ImGui::Separator();
        if (ImGui::Selectable("Default Empty")) { ECS.CreateDefaultEntity(); }
        ImGui::Separator();
        if (ImGui::CollapsingHeader("Mesh", ImGuiTreeNodeFlags_None))
        {
            ImGui::Separator();
            if (ImGui::Selectable("Point3D"))   { ECS.CreatePoint3DEntity(); }
            ImGui::Separator();
            if (ImGui::Selectable("Plane"))     { ECS.CreatePlaneEntity(); }
            ImGui::Separator();
            if (ImGui::Selectable("Triangle"))  { ECS.CreateTriangleEntity(); }
            ImGui::Separator();
            if (ImGui::Selectable("Sphere"))    { ECS.CreateSphereEntity(); }
            ImGui::Separator();
            if (ImGui::Selectable("AABB"))      { ECS.CreateAABBEntity(); }
            ImGui::Separator();
            if (ImGui::Selectable("Ray"))       { ECS.CreateRayEntity(); }
            ImGui::Separator();
        }
        ImGui::Separator();
        if (ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_None))
        {
            ImGui::Separator();
            if (ImGui::Selectable("Directional Light")) { ECS.CreateDirectionLightEntity(); }
            ImGui::Separator();
            if (ImGui::Selectable("Point Light"))       { ECS.CreatePointLightEntity(); }
            ImGui::Separator();
            //if (ImGui::Selectable("Spot Light"))        { ECS.CreateSpotLightEntity(); }
        }
        ImGui::Separator();
        //if (ImGui::Selectable("Camera")) { ECS.CreateCameraEntity(); } // camera entity created when a viewport window is opened
    }
}

void SceneHierarchyGUI::ClearSceneBtn()
{
    ImGui::SetCursorPosX(ImGui::GetWindowSize().x * 0.25f);
    ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.0f, 0.6f, 0.6f));
    if (ImGui::Button("Clear Scene", { ImGui::GetWindowSize().x * 0.5f, BUTTON_HEIGHT }))
    { ClearScene(); }
    ImGui::PopStyleColor(1);
}
void SceneHierarchyGUI::ClearScene()
{
    auto view = ECS.registry().view<entity>(exclude<Camera, Light>);
    for (auto entity : view) {  ECS.DestroyEntity(entity);  }
}

void SceneHierarchyGUI::ToggleBoundingVolumes()
{
    vec3 clr = bvColors.at(static_cast<BV_TYPE>(AABB)) * 0.25f;
    ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor(clr.r, clr.g, clr.b));
    if (ImGui::Button("Show only AABBs", { ImGui::GetWindowSize().x, BUTTON_HEIGHT * 2 }))
    { ToggleBoundingVolumes(AABB); }
    ImGui::PopStyleColor(1);
     clr = bvColors.at(static_cast<BV_TYPE>(BSPHERE_Ritters)) * 0.25f;
    ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor(clr.r, clr.g, clr.b));
    if (ImGui::Button("Show only Ritters", { ImGui::GetWindowSize().x , BUTTON_HEIGHT * 2 }))
    {  ToggleBoundingVolumes(BSPHERE_Ritters); }
    ImGui::PopStyleColor(1);
     clr = bvColors.at(static_cast<BV_TYPE>(BSPHERE_Larssons)) * 0.25f;
    ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor(clr.r, clr.g, clr.b));
    if (ImGui::Button("Show only Larssons", { ImGui::GetWindowSize().x , BUTTON_HEIGHT * 2 }))
    { ToggleBoundingVolumes(BSPHERE_Larssons); }
    ImGui::PopStyleColor(1);
     clr = bvColors.at(static_cast<BV_TYPE>(BSPHERE_PCA)) * 0.25f;
    ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor(clr.r, clr.g, clr.b));
    if (ImGui::Button("Show only bSphere PCA", { ImGui::GetWindowSize().x, BUTTON_HEIGHT * 2 }))
    { ToggleBoundingVolumes(BSPHERE_PCA); }
    ImGui::PopStyleColor(1);
     clr = bvColors.at(static_cast<BV_TYPE>(OBB_PCA)) * 0.25f;
    ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor(clr.r, clr.g, clr.b));
    if (ImGui::Button("Show only OBBs", { ImGui::GetWindowSize().x, BUTTON_HEIGHT*2 }))        
    { ToggleBoundingVolumes(OBB_PCA);  }
    ImGui::PopStyleColor(1);
}

void SceneHierarchyGUI::ToggleBoundingVolumes(BV_TYPE _type)
{
    ECS.registry().view<BVList>().each([&_type](BVList& _bvList)
    { for (auto& bv : _bvList)  { bv->isActive = bv->type == _type; }});
}