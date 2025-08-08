/**
@file    octkdtreegui.cpp
@author  weizhen.tan@digipen.edu
@date    22/07/2025

This file contains the definitions of the OctKDTreeGUI GUIWindow class.

*//*__________________________________________________________________________*/

#include <gui/octkdtreegui.hpp>
#include <gui/guisettings.hpp>

#include <ecs.hpp>
#include <components/renderable.hpp>
#include <components/boundingvolume.hpp>
#include <components/material.hpp>
#include <octree.hpp>
#include <kdtree.hpp>
/*                                                                   includes
----------------------------------------------------------------------------- */

void OctKDTreeGUI::InitWindow()
{
    std::vector<entity> entList;
    ECS.registry().view<Renderable, BVList>().each([&entList](auto _ent, Renderable _r, BVList _bvL) { entList.push_back(_ent); });
    OCTREE.BuildOctree(entList);
    ECS.registry().get<Transform>(OCTREE.GetOctreeEnt()).isDirty = true;
}

static void OnSceneUpdate(bool _isOctree)
{
    ECS.registry().get<BVList>(OCTREE.GetOctreeEnt()).clear();
    ECS.registry().get<BVList>(KDTREE.GetKdTreeEnt()).clear();
    KDTREE.ClearTree();
    OCTREE.ClearTree();
    for (auto lv : KDTREE.GetVisibilityFlags()) { lv.clear(); } KDTREE.GetVisibilityFlags().clear();

    ECS.registry().view<Material>().each([](Material& _mat) 
        {_mat.kAmbient = _mat.kDiffuse = { 0.9f, 0.5f, 0.3f }; });

    // rebuild tree
    std::vector<entity> entList;
    ECS.registry().view<Renderable, BVList>().each([&entList](auto _ent, Renderable _r, BVList _bvL) { entList.push_back(_ent); });
    if (_isOctree) 
    {  OCTREE.BuildOctree(entList); ECS.registry().get<Transform>(OCTREE.GetOctreeEnt()).isDirty = true; }
    else { KDTREE.BuildKDTree(0, KDTREE.GetRoot(), entList);
    ECS.registry().get<Transform>(KDTREE.GetKdTreeEnt()).isDirty = true;
    }
}

void OctKDTreeGUI::Render()
{
    ImGui::SetNextWindowPos({ m_GUIWindowPos.x, m_GUIWindowPos.y }, ImGuiCond_Once);
    ImGui::SetNextWindowSize({ m_GUIWindowSize.x, m_GUIWindowSize.y }, ImGuiCond_Once);
    if (ImGui::Begin(m_GUIWindowTitle.c_str(), &m_IsWindowOpen, m_GUIWindowFlags))
    {
        static bool isOctree = true;
        if (ImGui::BeginCombo("##tree type", isOctree ? "Octree" : "k-d tree"))
        {
            if (ImGui::Selectable("Octree")) { isOctree = true; OnSceneUpdate(isOctree); }
            if (ImGui::Selectable("k-d tree")) { isOctree = false; OnSceneUpdate(isOctree); }
            ImGui::EndCombo();
        }
        if (isOctree)
        {
            ImGui::SeparatorText("Straddling method");
            static int e = OCTREE.GetStraddleMethod();
            if (ImGui::RadioButton("Object center", &e, OBJ_CENTER))
            { OCTREE.GetStraddleMethod() = OBJ_CENTER; OnSceneUpdate(isOctree);
            } ImGui::SameLine();
            if (ImGui::RadioButton("All overlapping cells", &e, ALL_OVERLAPPING_CELLS))
            {
                OCTREE.GetStraddleMethod() = ALL_OVERLAPPING_CELLS; 
                OCTREE.GetNumObjPerNode() = OCTREE.GetNumObjPerNode() < 6 ? 6 : OCTREE.GetNumObjPerNode(); 
                OnSceneUpdate(isOctree);
            }
        }
        else
        {
            static int e = KDTREE.GetSplitStrategy();
            if (ImGui::RadioButton("Median BV center", &e, TOP_DOWN_BV_CENTER_MEDIAN))
            { KDTREE.GetSplitStrategy() = TOP_DOWN_BV_CENTER_MEDIAN; OnSceneUpdate(isOctree); }
            ImGui::SameLine();
            if (ImGui::RadioButton("Median BV extends", &e, TOP_DOWN_BV_EXTENT_MEDIAN))
            { KDTREE.GetSplitStrategy() = TOP_DOWN_BV_EXTENT_MEDIAN; OnSceneUpdate(isOctree); }

            ImGui::SeparatorText("Show Levels");
            unsigned numLevel = KDTREE.GetVisibilityFlags().size();
            std::vector<std::vector<bool*>>& flags = KDTREE.GetVisibilityFlags();
            for (unsigned i = 0; i < numLevel; ++i)
            {
                // color code level text accrdingly
                vec3 clr = GetColors()[i];
                ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor(clr.r, clr.g, clr.b));
                ImGui::Checkbox(std::to_string(i).c_str(), flags[i][0]);
                for (auto flag : flags[i]) { *flag = *flags[i][0]; }
                ImGui::PopStyleColor(1); ImGui::SameLine();
            } ImGui::Text("");
        }
        ImGui::DragInt("Number of objects per cell/node", isOctree ? &OCTREE.GetNumObjPerNode() : &KDTREE.GetNumObjPerNode(), 1.f, 1, 82);
        if (ImGui::IsItemDeactivatedAfterEdit()) { OnSceneUpdate(isOctree); }

        if (ImGui::Button("Toggle Wireframe", { ImGui::GetWindowSize().x * 0.25f, BUTTON_HEIGHT * 2 }))
        {
            ECS.registry().view<Renderable>().each([](Renderable& _mesh)
                { _mesh.GetIsWireframe() = !_mesh.GetIsWireframe(); });
        } ImGui::SameLine();
        if (ImGui::Button("Regenerate Colors", { ImGui::GetWindowSize().x * 0.25f, BUTTON_HEIGHT * 2 })) 
        { OnSceneUpdate(isOctree); }


        auto viewBV = ECS.registry().view<Transform, Renderable, BVList>();
        viewBV.each([](Transform& _xform, Renderable _r, BVList _bvL)
            { if (_xform.isDirty) { OnSceneUpdate(isOctree); } });
    } ImGui::End();
}

void OctKDTreeGUI::CleanUp() {}