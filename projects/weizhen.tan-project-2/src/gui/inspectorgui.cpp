/**
@file    inspectorgui.cpp
@author  weizhen.tan@digipen.edu
@date    19/05/2025

This file contains the definitions of the InspectorGUI class.

*//*__________________________________________________________________________*/

#include <gui/inspectorgui.hpp>
#include <gui/guisettings.hpp>
#include <ecs.hpp>
#include <components/transform.hpp>
#include <components/renderable.hpp>
#include <components/camera.hpp>
#include <components/material.hpp>
#include <components/light.hpp>
#include <components/boundingvolume.hpp>
/*                                                                   includes
----------------------------------------------------------------------------- */


void InspectorGUI::InitWindow()
{

}

void InspectorGUI::Render()
{
    ImGui::SetNextWindowPos({ m_GUIWindowPos.x, m_GUIWindowPos.y }, ImGuiCond_Once);
    ImGui::SetNextWindowSize({ m_GUIWindowSize.x, m_GUIWindowSize.y }, ImGuiCond_Once);
    if (ImGui::Begin(m_GUIWindowTitle.c_str(), &m_IsWindowOpen, m_GUIWindowFlags))
    {
        DisplayComponents();
        CreateComponentMenu();
    }
    ImGui::End();
}

void InspectorGUI::CleanUp() {}

void InspectorGUI::DisplayComponents() // TODO: optimization? 
// figure out how to use properties library or try another one
{
    if (!ECS.registry().valid(ECS.selectedEnt())) { return; }
    ImGui::Separator();
    auto& name = ECS.registry().get<EntityName>(ECS.selectedEnt());
    DisplayString(name.value);
    ImGui::Separator();
    auto& xform = ECS.registry().get<Transform>(ECS.selectedEnt());
    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if (ImGui::TreeNode("Transform"))
    {
        xform.isDirty = false;
        xform.isDirty = DisplayVec3("Position", xform.position) ||
                        DisplayVec3("Rotation", xform.rotation) ||
                        DisplayVec3("Scale", xform.scale);
        ImGui::TreePop();
    }

    DisplayCameraComp();
    
    DisplayLightComp();

    DisplayMeshComp();

    DisplayBVComp();
}

void InspectorGUI::CreateComponentMenu()
{
    if (ImGui::CollapsingHeader("Add Component", ImGuiTreeNodeFlags_None))
    {
        auto viewLight = ECS.registry().view<Light>();
        if (!viewLight.contains(ECS.selectedEnt()))
        {
            ImGui::Separator();
            if (ImGui::Selectable("Light ##Add Component"))
            {ECS.registry().emplace<Light>(ECS.selectedEnt());}
        }
        auto viewMesh = ECS.registry().view<Renderable>();
        if (!viewMesh.contains(ECS.selectedEnt()))
        {
            ImGui::Separator();
            if (ImGui::Selectable("Mesh ##Add Component"))
            {
                ECS.registry().emplace<Renderable>(ECS.selectedEnt());
                ECS.registry().emplace<Material>(ECS.selectedEnt());
            }
        }
        auto viewBV = ECS.registry().view<BVList>();
        if (/*!viewBV.contains(ECS.selectedEnt())
            &&*/ viewMesh.contains(ECS.selectedEnt())
            && ECS.registry().get<Renderable>(ECS.selectedEnt()).GetMeshType().find(".obj") != std::string::npos)
        {
            ImGui::Separator();
            if (ImGui::Selectable("Bounding Volume ##Add Component"))
            { 
                // TODO: change to entt storage
                if (!viewBV.contains(ECS.selectedEnt()))
                { ECS.registry().emplace<BVList>(ECS.selectedEnt()); }
                ECS.registry().get<BVList>(ECS.selectedEnt()).push_back(std::make_shared<Aabb>());
                ECS.registry().get<BVList>(ECS.selectedEnt()).back()->InitBV(ECS.registry().get<Renderable>(ECS.selectedEnt()).GetMeshType());
                ECS.registry().get<BVList>(ECS.selectedEnt()).back()->UpdateBV(ECS.registry().get<Transform>(ECS.selectedEnt()));
            }
        }
	}
}

void InspectorGUI::DisplayCameraComp()
{
    auto viewCamera = ECS.registry().view<Camera>();
    if (viewCamera.contains(ECS.selectedEnt()))
    {
        ImGui::Separator();
        auto& cam = ECS.registry().get<Camera>(ECS.selectedEnt());
        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        if (ImGui::TreeNode("Camera"))
        {
            if (ImGui::BeginCombo("##Camera Type", to_string(cam.type).c_str()))
            {
                for (size_t i = 0; i < static_cast<size_t>(CAMERA_TYPE::CAMERA_TYPE_TOTAL); ++i)
                {
                    if (ImGui::Selectable(to_string(static_cast<CAMERA_TYPE>(i)).c_str()))
                    {  cam.type = static_cast<CAMERA_TYPE>(i); }
                }
                ImGui::EndCombo();
            }
            ImGui::SetNextItemOpen(true, ImGuiCond_Once);
            /*if (ImGui::TreeNode("Projection"))
            {
                if (ImGui::BeginCombo("##Projection", cam.isPerspective ? "Perspective" : "Orthographic"))
                {
                    if (ImGui::Selectable("Perspective")) { cam.isPerspective = true; }
                    if (ImGui::Selectable("Orthographic")) { cam.isPerspective = false; }
                    ImGui::EndCombo();
                }
                ImGui::TreePop();
            }*/
            if (cam.isPerspective) { DisplayFloat("FOV", cam.fov); }
            DisplayFloat("Near", cam.near);
            DisplayFloat("Far", cam.far);
            /*if (ImGui::TreeNode("Show Frustum"))
            {
                ImGui::Checkbox("##Show Frustum", &cam.showFrustum);
                ImGui::TreePop();
            }*/
            ImGui::TreePop();
        }
    }
}

void InspectorGUI::DisplayLightComp()
{
    auto viewLight = ECS.registry().view<Light>();
    if (viewLight.contains(ECS.selectedEnt())) 
    {
        ImGui::Separator();
        auto& light = ECS.registry().get<Light>(ECS.selectedEnt());
        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        if (ImGui::TreeNode("Light"))
        {
            if (ImGui::BeginCombo("##Light Type", to_string(light.type).c_str()))
            {
                for (size_t i = 0; i < static_cast<size_t>(LIGHT_TYPE::LIGHT_TYPE_TOTAL); ++i)
                {
                    if (ImGui::Selectable(to_string(static_cast<LIGHT_TYPE>(i)).c_str()))
                    { light.type = static_cast<LIGHT_TYPE>(i);  }
                }
                ImGui::EndCombo();
            }
            DisplayClrVec3("Light", light.color);
            DisplayFloat("intensity", light.intensity);
            if (light.type != LIGHT_DIRECTIONAL) { DisplayFloat("range", light.range); }
            if (light.type == LIGHT_SPOT)        { DisplayFloat("angle", light.angle); }
            ImGui::TreePop();
        }
    }
    
}

void InspectorGUI::DisplayMeshComp()
{
    auto viewMesh = ECS.registry().view<Renderable, Material>();
    if (viewMesh.contains(ECS.selectedEnt())) 
    {
        ImGui::Separator();
        auto& mesh = ECS.registry().get<Renderable>(ECS.selectedEnt());
        auto& mat = ECS.registry().get<Material>(ECS.selectedEnt());
        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        if (ImGui::TreeNode("Mesh"))
        {
            DisplayBool("Render Wireframe", mesh.GetIsWireframe());
            DisplayBool("Hide Mesh", mesh.GetIsHidden());
            if (ImGui::BeginCombo("##Mesh Type", mesh.GetMeshType().c_str()))
            {
                for (auto const& [name, buf] : Renderable::GetBuffers())
                {  if (ImGui::Selectable(name.c_str())) 
                   { 
                        /*if (ECS.registry().view<BoundingVolume>().contains(ECS.selectedEnt()))
                        {
                            if (name.find(".obj") != std::string::npos)
                            {
                                for (auto bv : ECS.registry().get<BVList>(ECS.selectedEnt()))
                                {
                                    bv->InitBV(name);
                                    bv->UpdateBV(ECS.registry().get<Transform>(ECS.selectedEnt()));
                                }
                            }
                            else
                            { ECS.registry().remove<BoundingVolume>(ECS.selectedEnt()); }
                        }*/
                        mesh.SetMeshType(name); 
                   } 
                }
                ImGui::EndCombo();
            } ImGui::TreePop();
        }
        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        if (ImGui::TreeNode("Material"))
        {
            DisplayClrVec3("Ambient", mat.kAmbient);
            DisplayClrVec3("Diffuse", mat.kDiffuse);
            DisplayClrVec3("Specular", mat.kSpecular);
            DisplayFloat("Shininess", mat.shininess);
            ImGui::TreePop();
        }
    }
}

void InspectorGUI::DisplayBVComp()
{
    auto viewBV = ECS.registry().view<BVList>();
    if (viewBV.contains(ECS.selectedEnt()))
    {
        ImGui::Separator();
        auto& bvArr = ECS.registry().get<BVList>(ECS.selectedEnt());
        
        unsigned i = 0;
        for (auto& bv : bvArr)
        {
            ImGui::SetNextItemOpen(true, ImGuiCond_Once);
            std::string treeNodeName = "Bounding Volume " + std::to_string(i);
            if (ImGui::TreeNode(treeNodeName.c_str()))
            {
                ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor(bvColors.at(bv->type).r, bvColors.at(bv->type).g, bvColors.at(bv->type).b));
                if (ImGui::BeginCombo("##BoundingVolume Type", to_string(bv->type).c_str()))
                {
                    for (size_t i = 0; i < static_cast<size_t>(BV_TYPE::BV_TYPE_TOTAL); ++i)
                    {
                        vec3 clr = bvColors.at(static_cast<BV_TYPE>(i));
                        ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor(clr.r, clr.g, clr.b));
                        if (ImGui::Selectable(to_string(static_cast<BV_TYPE>(i)).c_str()))
                        {
                            if (bv->type != static_cast<BV_TYPE>(i))
                            {
                                switch (static_cast<BV_TYPE>(i))
                                {
                                case BSPHERE_Ritters:  bv = std::make_shared<Sphere>(); bv->isActive = true; break;
                                case BSPHERE_Larssons: bv = std::make_shared<Sphere>(Sphere(BSPHERE_Larssons)); bv->isActive = true; break;
                                case BSPHERE_PCA:      bv = std::make_shared<Sphere>(Sphere(BSPHERE_PCA)); bv->isActive = true; break;
                                case OBB_PCA:          bv = std::make_shared<Obb>(); break;
                                case AABB:             bv = std::make_shared<Aabb>(); break;
                                };
                                bv->InitBV(ECS.registry().get<Renderable>(ECS.selectedEnt()).GetMeshType());
                                bv->UpdateBV(ECS.registry().get<Transform>(ECS.selectedEnt()));
                            }
                        } ImGui::PopStyleColor(1);
                    }
                    ImGui::EndCombo();
                }
                if (DisplayBool("is Active", bv->isActive)) // if has been edited
                {
                    bv->InitBV(ECS.registry().get<Renderable>(ECS.selectedEnt()).GetMeshType());
                    bv->UpdateBV(ECS.registry().get<Transform>(ECS.selectedEnt()));
                    ECS.registry().get<Transform>(ECS.selectedEnt()).isDirty = true;
                }
                switch (bv->type)
                {
                case BSPHERE_Ritters:
                case BSPHERE_Larssons:
                case BSPHERE_PCA:      
                    DisplayVec3("center", std::dynamic_pointer_cast<Sphere>(bv)->center);
                    DisplayFloat("radius", std::dynamic_pointer_cast<Sphere>(bv)->radius);
                    break;
                case OBB_PCA:           
                    for(unsigned i = 0; i < 3; ++i)
                    { DisplayVec3("basis["+std::to_string(i)+"]:", std::dynamic_pointer_cast<Obb>(bv)->axes[i]); }
                case AABB:              
                    DisplayVec3("center", std::dynamic_pointer_cast<Aabb>(bv)->center);
                    DisplayVec3("halfExtents", std::dynamic_pointer_cast<Aabb>(bv)->halfExtents); break;
                };
                ImGui::TreePop(); ImGui::PopStyleColor(1); 
            } ++i;
        }
    }
}

void InspectorGUI::DisplayString(std::string& _str)
{
    static char nameBuffer[200];
    sprintf(nameBuffer, _str.c_str());
    ImGui::PushItemWidth(ImGui::GetWindowSize().x - INDENT_SPACING);
    std::string id = "##" + _str;
        ImGui::InputText(id.c_str(), nameBuffer, IM_ARRAYSIZE(nameBuffer));
    ImGui::PopItemWidth();
    std::string checkBuffer = nameBuffer;
    if (_str != checkBuffer && !ImGui::IsItemActive())
    {  _str = checkBuffer; }
}

bool InspectorGUI::DisplayBool(std::string _title, bool& _data)
{ 
    ImGui::Checkbox(_title.c_str(), &_data); 
    return ImGui::IsItemDeactivatedAfterEdit();
}

void InspectorGUI::DisplayFloat(std::string _title, float& _data)
{
    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if (ImGui::TreeNode(_title.c_str()))
    {
        std::string id = "##" + _title;
        ImGui::InputFloat(id.c_str(), &_data);
        ImGui::TreePop();
    }
}

bool InspectorGUI::DisplayVec3(std::string _title, vec3& _data)
{
    bool isEdited{ false }; static vec3 tmp = _data;
    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if (ImGui::TreeNode(_title.c_str()))
    {
        std::string id = "##" + _title;
        ImGui::DragFloat3(id.c_str(), &_data[0]);
        isEdited = ImGui::IsItemDeactivatedAfterEdit() && tmp != _data;
        if (isEdited) { tmp = _data; }
        ImGui::TreePop();
    } return isEdited;
}

void InspectorGUI::DisplayClrVec3(std::string _title, vec3& _data)
{
    std::string id = "##" + _title;
    ImGui::ColorEdit3(id.c_str(), &_data[0]);
}
