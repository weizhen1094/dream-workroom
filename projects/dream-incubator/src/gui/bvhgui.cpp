/**
@file    bvhgui.cpp
@author  weizhen.tan@digipen.edu
@date    11/07/2025

This file contains the definitions of the BVHGUI GUIWindow class.

*//*__________________________________________________________________________*/

#include <gui/bvhgui.hpp>
#include <bvhierarchy.hpp>
#include <components/renderable.hpp>
/*                                                                   includes
----------------------------------------------------------------------------- */
std::vector<vec3> BVHierarchy::s_BVHLvColors;

void BVHGUI::InitWindow()
{
    BVH.GetBVHLvColors() =
    {
        vec3(1.f,0.f,0.f),
        vec3(0.f,1.f,0.f),
        vec3(0.f,0.f,1.f),
        vec3(1.f,1.f,0.f),
        vec3(0.f,1.f,1.f),
        vec3(1.f,0.f,1.f),
        vec3(1.f,0.5f,0.f),
        vec3(0.5f,1.0f,0.f),
    };

    std::vector<entity> entList;
    auto viewBV = ECS.registry().view<Renderable, BVList>();
    viewBV.each([&entList](auto _ent, Renderable _r, BVList _bvL) { entList.push_back(_ent); });
    BVH.BuildTopDown(0, BVH.GetRoot(), entList);
    //BVH.GetConfig().isTopDown = false; BVH.BuildBottomUp(entList);
    std::cout << "BVH.IsTreeBalanced(): " << BVH.IsTreeBalanced() << std::endl;
}

static void UpdateBVH()
{
    BVH.ClearBVHData();
    if (!(BVH.GetConfig().exitFlags[MAX_NUM_OBJ].first ||
        BVH.GetConfig().exitFlags[MAX_TREE_HEIGHT].first))
    {
        // guard against no termination conditions being set
        BVH.GetConfig().exitFlags[MAX_NUM_OBJ].first = true;
    }
    if (!(BVH.GetConfig().mergeHeuristics[BTM_UP_NEAREST_NEIGHBOUR] ||
        BVH.GetConfig().mergeHeuristics[BTM_UP_MIN_CHILD_VOL] ||
        BVH.GetConfig().mergeHeuristics[BTM_UP_MIN_CHILD_SA]))
    {
        // guard against no merge heauristics being set
        BVH.GetConfig().mergeHeuristics[BTM_UP_NEAREST_NEIGHBOUR] = true;
    }
    std::vector<entity> entList;
    auto viewBV = ECS.registry().view<Transform, Renderable, BVList>();
    viewBV.each([&entList](auto _ent, Transform& _xform, Renderable& _r, BVList& _bvArr)
        {
            for (auto bv : _bvArr)
            { bv->InitBV(_r.GetMeshType());  bv->UpdateBV(_xform); }
            entList.push_back(_ent);
        });
    if (BVH.GetConfig().isTopDown) { BVH.BuildTopDown(0, BVH.GetRoot(), entList); }
    else { BVH.BuildBottomUp(entList); }

    std::cout << "BVH.IsTreeBalanced(): " << BVH.IsTreeBalanced() << std::endl;

}
void BVHGUI::Render()
{
    ImGui::SetNextWindowPos({ m_GUIWindowPos.x, m_GUIWindowPos.y }, ImGuiCond_Once);
    ImGui::SetNextWindowSize({ m_GUIWindowSize.x, m_GUIWindowSize.y }, ImGuiCond_Once);
    if (ImGui::Begin(m_GUIWindowTitle.c_str(), &m_IsWindowOpen, m_GUIWindowFlags))
    {
        ImGui::SeparatorText("Show BVH Level");
        unsigned numLevel = BVH.GetVisibilityFlags().size(); // tmp
        for (unsigned i = 0; i < numLevel; ++i)
        {
            // color code level text accrdingly
            vec3 clr = BVH.GetBVHLvColors()[i];
            ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor(clr.r, clr.g, clr.b));
            ImGui::Checkbox(std::to_string(i).c_str(), BVH.GetVisibilityFlags()[i][0]);
            for (auto flag : BVH.GetVisibilityFlags()[i]) { *flag = *BVH.GetVisibilityFlags()[i][0]; }
            ImGui::PopStyleColor(1); ImGui::SameLine();
        }
        ImGui::Text("");
        bool isTopDown = BVH.GetConfig().isTopDown;
        ImGui::SeparatorText("BVH method and type");
        if (ImGui::BeginCombo("##BVH approach", isTopDown ? "Top Down" : "Bottom Up"))
        {
            if (ImGui::Selectable("Top Down")) { BVH.GetConfig().isTopDown = true; UpdateBVH(); }
            if (ImGui::Selectable("Bottom Up")) { BVH.GetConfig().isTopDown = false; UpdateBVH(); }
            ImGui::EndCombo();
        }
        static int e = BVH.GetConfig().type; ImGui::SameLine();
        if (ImGui::RadioButton("AABB", &e, AABB)) 
        { BVH.GetConfig().type = AABB; UpdateBVH(); } ImGui::SameLine();
        if (ImGui::RadioButton("Sphere PCA", &e, BSPHERE_PCA)) 
        { BVH.GetConfig().type = BSPHERE_PCA; UpdateBVH(); }
        /*ImGui::RadioButton("OBB", &e, 2);*/
        ImGui::SeparatorText("Split Point Heuristics");
        if (isTopDown)
        {
            static int e = BVH.GetConfig().splitStrategy;
            if (ImGui::RadioButton("Median BV center", &e, TOP_DOWN_BV_CENTER_MEDIAN))
            { BVH.GetConfig().splitStrategy = TOP_DOWN_BV_CENTER_MEDIAN; }
            if (ImGui::IsItemDeactivatedAfterEdit()) { UpdateBVH(); }
            ImGui::SameLine();
            if (ImGui::RadioButton("Median BV extends", &e, TOP_DOWN_BV_EXTENT_MEDIAN))
            { BVH.GetConfig().splitStrategy = TOP_DOWN_BV_EXTENT_MEDIAN; }
            if (ImGui::IsItemDeactivatedAfterEdit()) { UpdateBVH(); }
            ImGui::SameLine();
            if (ImGui::RadioButton("k-even splits", &e, TOP_DOWN_K_EVEN_SPLIT_SINGLE_AXIS))
            {  BVH.GetConfig().splitStrategy = TOP_DOWN_K_EVEN_SPLIT_SINGLE_AXIS; }
            if (ImGui::IsItemDeactivatedAfterEdit()) { UpdateBVH(); }
            ImGui::SeparatorText("Termination Conditions");
            static int e1 = 0;
            ImGui::Checkbox("Max num obj in leaf", &BVH.GetConfig().exitFlags[MAX_NUM_OBJ].first);
            if (ImGui::IsItemDeactivatedAfterEdit()) { UpdateBVH(); }
            if (BVH.GetConfig().exitFlags[MAX_NUM_OBJ].first)
            {
                if (ImGui::RadioButton("Single Object in leaf", &e1, 0))
                { BVH.GetConfig().exitFlags[MAX_NUM_OBJ].second = 1; }
                if (ImGui::IsItemDeactivatedAfterEdit()) { UpdateBVH(); } ImGui::SameLine();
                if (ImGui::RadioButton("Max 2 Objects in leaf", &e1, 1))
                { BVH.GetConfig().exitFlags[MAX_NUM_OBJ].second = 2; }
                if (ImGui::IsItemDeactivatedAfterEdit()) { UpdateBVH(); }
            }
            ImGui::Checkbox("Max tree height = 2", &BVH.GetConfig().exitFlags[MAX_TREE_HEIGHT].first);
            if (ImGui::IsItemDeactivatedAfterEdit()) { UpdateBVH(); }
        }
        else
        {
            ImGui::Checkbox("Nearest neighbour", &BVH.GetConfig().mergeHeuristics[BTM_UP_NEAREST_NEIGHBOUR]);
            if (ImGui::IsItemDeactivatedAfterEdit()) { UpdateBVH(); } ImGui::SameLine();
            ImGui::Checkbox("Min sum of child volume", &BVH.GetConfig().mergeHeuristics[BTM_UP_MIN_CHILD_VOL]);
            if (ImGui::IsItemDeactivatedAfterEdit()) { UpdateBVH(); }ImGui::SameLine();
            ImGui::Checkbox("Min sum of child surface area", &BVH.GetConfig().mergeHeuristics[BTM_UP_MIN_CHILD_SA]);
            if (ImGui::IsItemDeactivatedAfterEdit()) { UpdateBVH(); }
        }
    }
    // check if any ent w mdl and bv moved
    auto viewBV = ECS.registry().view<Transform, Renderable, BVList>();
    viewBV.each([](Transform& _xform, Renderable _r, BVList _bvL) 
        { if (_xform.isDirty) { UpdateBVH(); } });
    ImGui::End();
}

void BVHGUI::CleanUp() {}