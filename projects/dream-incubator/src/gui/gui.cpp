/**
@file    gui.cpp
@author  weizhen.tan@digipen.edu
@date    19/05/2025

This file contains the definitions of the GUI class and helper functions that
encapsulates all Dear ImGui logic.

*//*__________________________________________________________________________*/

#include <gui/gui.hpp>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_glfw.h>
#include <imgui_internal.h>
//#include <imguizmo.h>
#include <sstream>
#include <iomanip>
#include <gui/guisettings.hpp>
#include <gui/scenehierarchygui.hpp>
#include <gui/viewportgui.hpp>
#include <gui/inspectorgui.hpp>
#include <gui/bvhgui.hpp>
#include <gui/octkdtreegui.hpp>
#include <components/boundingvolume.hpp>
#include <components/transform.hpp>
#include <components/renderable.hpp>
#include <components/material.hpp>
/*                                                                   includes
----------------------------------------------------------------------------- */

bool GUI::InitGUI(Window* _pWin)
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch
    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(_pWin->GetWindowPtr(), true);          
    if (!ImGui_ImplOpenGL3_Init()) { return false; }

    m_pWin = _pWin;
    m_MainWindowSize = vec2(_pWin->GetWindowSize().x, _pWin->GetWindowSize().y - MAINMENUBAR_HEIGHT);

    CreateOctreeKdtreeScene();
    // create child windows
    CreateSceneHierarchyGUI();
    CreateOctKDTreeGUI();
    CreateInspectorGUI();
    CreateMainViewportGUI();
    CreateSubViewportGUI();
    CreateTxtFileGUI("../projects/weizhen.tan-project-4/README.txt");

    return true;
}

void GUI::UpdateGUI()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    //ImGuizmo::BeginFrame();

    for (auto it = m_GUIWindows.begin(); it != m_GUIWindows.end(); ++it)
    { if (!((*it)->isWindowOpen())) { m_GUIWindows.erase(it); break; } }
}

void GUI::RenderGUI()
{
    auto dockspace_id = ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_AutoHideTabBar);
    RenderMainMenuBar();
    RenderMainWindow();
    
    // render sub windows
    for (auto const& wind : m_GUIWindows) 
    { ImGui::SetNextWindowBgAlpha(1.f); wind->Render(); }

    // check if sub windows closed
    m_GUIWindows.erase(
        std::remove_if(m_GUIWindows.begin(), m_GUIWindows.end(), 
        [](std::unique_ptr<IGUIWindow> const& _wind)
        { if (!_wind->isWindowOpen()) { _wind->CleanUp(); } return !_wind->isWindowOpen(); }), m_GUIWindows.end());

    DockChildWindows(dockspace_id);
    
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GUI::TerminateGUI()
{
    for (auto const& wind : m_GUIWindows) { wind->CleanUp(); }
    ImGui::ClearIniSettings();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void GUI::RenderMainMenuBar()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("[Scenes]")) // TODO: serialization
        {
            if (ImGui::MenuItem("Default Empty Scene")) { CreateDefaultEmptyScene(); }
           /* ImGui::SeparatorText("Project 3");
            if (ImGui::MenuItem("Bounding Volumes Hierarchy Scene"))  { CreateBVHScene(); }
            ImGui::SeparatorText("Project 2");
            if (ImGui::MenuItem("Bounding Volumes Scene")) { CreateBVScene(); }*/
            ImGui::SeparatorText("Project 1");
            ImGui::SeparatorText("Basic Intersection Tests");
            if (ImGui::MenuItem("Sphere vs Sphere"))    { CreateSpherevsSphereScene(); }
            if (ImGui::MenuItem("AABB vs Sphere"))      { CreateAABBvsSphereScene(); }
            if (ImGui::MenuItem("AABB vs AABB"))        { CreateAABBvsAABBScene(); }
            ImGui::SeparatorText("Point-Based Tests");
            if (ImGui::MenuItem("Point vs Sphere"))     { CreatePointvsSphereScene(); }
            if (ImGui::MenuItem("Point vs AABB"))       { CreatePointvsAABBScene(); }
            if (ImGui::MenuItem("Point vs Triangle"))   { CreatePointvsTriangleScene(); }
            if (ImGui::MenuItem("Point vs Plane"))      { CreatePointvsPlaneScene(); }
            ImGui::SeparatorText("Ray-Based Tests");
            if (ImGui::MenuItem("Ray vs Sphere"))       { CreateRayvsSphereScene(); }
            if (ImGui::MenuItem("Ray vs AABB"))         { CreateRayvsAABBScene(); }
            if (ImGui::MenuItem("Ray vs Triangle"))     { CreateRayvsTriangleScene(); }
            if (ImGui::MenuItem("Ray vs Plane"))        { CreateRayvsPlaneScene(); }
            ImGui::SeparatorText("Plane-Based Tests");
            if (ImGui::MenuItem("Plane vs AABB"))       { CreatePlanevsAABBScene(); }
            if (ImGui::MenuItem("Plane vs Sphere"))     { CreatePlanevsSphereScene(); }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("[Add Windows]"))
        {
            if (ImGui::MenuItem("Scene Hierarchy")) { CreateSceneHierarchyGUI(); }
            if (ImGui::MenuItem("Main Viewport"))   { CreateMainViewportGUI(); }
            if (ImGui::MenuItem("Sub Viewport"))    { CreateSubViewportGUI(); }
            if (ImGui::MenuItem("BVHierarchy Configuration"))         { CreateBVHGUI(); }
            if (ImGui::MenuItem("Inspector"))       { CreateInspectorGUI(); } 
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Open README.txt"))
        {
            CreateTxtFileGUI("../projects/weizhen.tan-project-4/README.txt");
            ImGui::EndMenu();
        }
        std::stringstream ssFPS;
        ssFPS <<  "FPS: " << std::setprecision(5) << m_pWin->GetFPS();
        ImGui::SetCursorPosX(ImGui::GetWindowSize().x - ImGui::CalcTextSize(ssFPS.str().c_str()).x * 1.5f);
        ImGui::Text(ssFPS.str().c_str());
        ImGui::EndMainMenuBar();
    }
}

void GUI::RenderMainWindow()
{
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground |
        ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoResize;
    const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos({ main_viewport->Pos.x, MAINMENUBAR_HEIGHT });
    ImGui::SetNextWindowSize({ main_viewport->Size.x, main_viewport->Size.y - MAINMENUBAR_HEIGHT });
    ImGui::Begin("Main Window", nullptr, window_flags);
    ImGui::End();
}

void GUI::DockChildWindows(unsigned& _id)
{
    static bool once{ true }; // only for init
    if (once) {
        once = false;
        auto dock_id_left = ImGui::DockBuilderSplitNode(_id, ImGuiDir_Left, 0.225f, nullptr, &_id);
        auto dock_id_right = ImGui::DockBuilderSplitNode(_id, ImGuiDir_Right, 0.25f, nullptr, &_id);
        auto dock_id_down = ImGui::DockBuilderSplitNode(_id, ImGuiDir_Down, 0.275f, nullptr, &_id);
        ImGui::DockBuilderDockWindow("Scene Hierarchy##0", dock_id_left);
        ImGui::DockBuilderDockWindow("Project 4 Settings##1", dock_id_down);
        ImGui::DockBuilderDockWindow("Inspector##2", dock_id_right);
        //ImGui::DockBuilderDockWindow("README.txt##2", dock_id_down);
        ImGui::DockBuilderDockWindow("Main Viewport##3", _id);
        auto dock_id_left_btm = ImGui::DockBuilderSplitNode(dock_id_left, ImGuiDir_Down, 0.2825f, nullptr, &dock_id_left);
        ImGui::DockBuilderDockWindow("Sub Viewport##4", dock_id_left_btm);
    }
}

void GUI::CreateSceneHierarchyGUI()
{
    m_GUIWindows.push_back(std::make_unique<SceneHierarchyGUI>("Scene Hierarchy",
        vec2(0, MAINMENUBAR_HEIGHT), vec2(m_MainWindowSize.x * 0.25f, m_MainWindowSize.y * 0.75f)));
    m_GUIWindows.back()->InitWindow();
}
void GUI::CreateBVHGUI()
{
    m_GUIWindows.push_back(std::make_unique<BVHGUI>("Console",
        vec2(0, m_MainWindowSize.y * 0.75f + MAINMENUBAR_HEIGHT), vec2(m_MainWindowSize.x, m_MainWindowSize.y * 0.25f)));
    m_GUIWindows.back()->InitWindow();
}
void GUI::CreateOctKDTreeGUI()
{
    m_GUIWindows.push_back(std::make_unique<OctKDTreeGUI>("Project 4 Settings",
        vec2(0, m_MainWindowSize.y * 0.75f + MAINMENUBAR_HEIGHT), vec2(m_MainWindowSize.x, m_MainWindowSize.y * 0.25f)));
    m_GUIWindows.back()->InitWindow();
}
void GUI::CreateInspectorGUI()
{
    m_GUIWindows.push_back(std::make_unique<InspectorGUI>("Inspector",
        vec2(m_MainWindowSize.x * 0.75f, MAINMENUBAR_HEIGHT), vec2(m_MainWindowSize.x * 0.25f, m_MainWindowSize.y * 0.75f)));
    m_GUIWindows.back()->InitWindow();
}
void GUI::CreateMainViewportGUI()
{
    vec3 camPos = vec3(100475.f, 77790.f, 51405.f);
    vec3 camRot = vec3(-20.f, -280.f, 0.0f);
    float near{ 0.5f }; float far{ 100000000 };
    m_GUIWindows.push_back(std::make_unique<ViewportGUI>(camPos, camRot, near, far, "Main Viewport",
        vec2(m_MainWindowSize.x * 0.25f, MAINMENUBAR_HEIGHT), vec2(m_MainWindowSize.x * 0.5f, m_MainWindowSize.y * 0.75f)));
    m_GUIWindows.back()->InitWindow();
}
void GUI::CreateSubViewportGUI()
{
    vec3 camPos = vec3(0.f, 147375.f, 27550.f);
    vec3 camRot = vec3(-90.f, 90.f, 0.0f);
    float near{ 0.3f }; float far{ 100000000 };
    m_GUIWindows.push_back(std::make_unique<ViewportGUI>(camPos, camRot, near, far, "Sub Viewport",
        vec2( m_MainWindowSize.x * 0.64f, m_MainWindowSize.y * 0.55f), vec2(m_MainWindowSize.x * 0.15f, m_MainWindowSize.y * 0.2f))); 
    m_GUIWindows.back()->InitWindow();
}
void GUI::CreateTxtFileGUI(std::string _filePath)
{
    m_GUIWindows.push_back(std::make_unique<DisplayTxtFileGUI>(_filePath, _filePath,
        vec2(m_MainWindowSize.x * 0.1f, m_MainWindowSize.y * 0.35f + MAINMENUBAR_HEIGHT), vec2(m_MainWindowSize.x * 0.75f, m_MainWindowSize.y * 0.5f)));
    m_GUIWindows.back()->InitWindow();
}

void GUI::CreateDefaultEmptyScene()
{
    SceneHierarchyGUI::ClearScene();
    ECS.CreateDirectionLightEntity();
}

void GUI::CreateSpherevsSphereScene()
{
    CreateDefaultEmptyScene();
    auto s0 = ECS.CreateSphereEntity();
    ECS.registry().get<Transform>(s0).position = vec3(-2, 0, 0);
    ECS.registry().get<Material>(s0).kAmbient 
        = ECS.registry().get<Material>(s0).kDiffuse 
        = vec3(0.f, 0.5f, 1.f);
    auto s1 = ECS.CreateSphereEntity();
    ECS.registry().get<Transform>(s1).position = vec3(2, 0, 0);
    ECS.registry().get<Material>(s1).kAmbient 
        = ECS.registry().get<Material>(s1).kDiffuse
        = vec3(1.f, 0, 1.f);
    ECS.selectedEnt(s1);
}
void GUI::CreateAABBvsSphereScene()
{
    CreateDefaultEmptyScene();
    auto aabb = ECS.CreateAABBEntity();
    ECS.registry().get<Transform>(aabb).position = vec3(-2, 0, 0);
    ECS.registry().get<Transform>(aabb).scale = vec3(1.25f);
    ECS.registry().get<Material>(aabb).kAmbient
        = ECS.registry().get<Material>(aabb).kDiffuse
        = vec3(0.f, 0.5f, 1.f);
    auto s = ECS.CreateSphereEntity();
    ECS.registry().get<Transform>(s).position = vec3(2, 0, 0);
    ECS.registry().get<Material>(s).kAmbient
        = ECS.registry().get<Material>(s).kDiffuse
        = vec3(1.f, 0, 1.f);
    ECS.selectedEnt(s);
}
void GUI::CreateAABBvsAABBScene()
{
    CreateDefaultEmptyScene();
    auto aabb0 = ECS.CreateAABBEntity();
    ECS.registry().get<Transform>(aabb0).position = vec3(-2, 0, 0);
    ECS.registry().get<Material>(aabb0).kAmbient
        = ECS.registry().get<Material>(aabb0).kDiffuse
        = vec3(0.f, 0.5f, 1.f);
    auto aabb1 = ECS.CreateAABBEntity();
    ECS.registry().get<Transform>(aabb1).position = vec3(2, 0, 0);
    ECS.registry().get<Material>(aabb1).kAmbient
        = ECS.registry().get<Material>(aabb1).kDiffuse
        = vec3(1.f, 0, 1.f);
    ECS.selectedEnt(aabb1);
}
void GUI::CreatePointvsSphereScene() 
{
    CreateDefaultEmptyScene();
    auto p = ECS.CreatePoint3DEntity();
    ECS.registry().get<Transform>(p).position = vec3(-2, 0, 0);
    ECS.registry().get<Material>(p).kAmbient
        = ECS.registry().get<Material>(p).kDiffuse
        = vec3(0.f, 0.5f, 1.f);
    auto s = ECS.CreateSphereEntity();
    ECS.registry().get<Transform>(s).position = vec3(2, 0, 0);
    ECS.registry().get<Material>(s).kAmbient
        = ECS.registry().get<Material>(s).kDiffuse
        = vec3(1.f, 0, 1.f);
    ECS.selectedEnt(p);
}
void GUI::CreatePointvsAABBScene()
{
    CreateDefaultEmptyScene();
    auto p = ECS.CreatePoint3DEntity();
    ECS.registry().get<Transform>(p).position = vec3(-2, 0, 0);
    ECS.registry().get<Material>(p).kAmbient
        = ECS.registry().get<Material>(p).kDiffuse
        = vec3(0.f, 0.5f, 1.f);
    auto aabb = ECS.CreateAABBEntity();
    ECS.registry().get<Transform>(aabb).position = vec3(2, 0, 0);
    ECS.registry().get<Material>(aabb).kAmbient
        = ECS.registry().get<Material>(aabb).kDiffuse
        = vec3(1.f, 0, 1.f);
    ECS.selectedEnt(p);
}
void GUI::CreatePointvsTriangleScene()
{
    CreateDefaultEmptyScene();
    auto p = ECS.CreatePoint3DEntity();
    ECS.registry().get<Transform>(p).position = vec3(-2, 0, 0);
    ECS.registry().get<Material>(p).kAmbient
        = ECS.registry().get<Material>(p).kDiffuse
        = vec3(0.f, 0.5f, 1.f);
    auto t = ECS.CreateTriangleEntity();
    ECS.registry().get<Transform>(t).position = vec3(2, 0, 0);
    ECS.registry().get<Material>(t).kAmbient
        = ECS.registry().get<Material>(t).kDiffuse
        = vec3(1.f, 0, 1.f);
    ECS.selectedEnt(p);
}
void GUI::CreatePointvsPlaneScene()
{ 
    CreateDefaultEmptyScene();
    auto pt = ECS.CreatePoint3DEntity();
    ECS.registry().get<Transform>(pt).position = vec3(0, 1, 0);
    ECS.registry().get<Material>(pt).kAmbient
        = ECS.registry().get<Material>(pt).kDiffuse
        = vec3(0.f, 0.5f, 1.f);
    auto p = ECS.CreatePlaneEntity();
    ECS.registry().get<Transform>(p).position = vec3(0, 0, 0);
    ECS.registry().get<Transform>(p).rotation = vec3(-60, 90, 0);
    ECS.registry().get<Transform>(p).scale = vec3(10);
    ECS.registry().get<Material>(p).kAmbient
        = ECS.registry().get<Material>(p).kDiffuse
        = vec3(1.f, 0, 1.f);
    ECS.selectedEnt(pt);
}
void GUI::CreateRayvsSphereScene()
{
    CreateDefaultEmptyScene();
    auto r = ECS.CreateRayEntity();
    ECS.registry().get<Transform>(r).position = vec3(-2, 0, 0);
    ECS.registry().get<Material>(r).kAmbient
        = ECS.registry().get<Material>(r).kDiffuse
        = vec3(0.f, 0.5f, 1.f);
    auto s = ECS.CreateSphereEntity();
    ECS.registry().get<Transform>(s).position = vec3(2, 0, 0);
    ECS.registry().get<Material>(s).kAmbient
        = ECS.registry().get<Material>(s).kDiffuse
        = vec3(1.f, 0, 1.f);
    ECS.selectedEnt(r);
}
void GUI::CreateRayvsAABBScene()
{
    CreateDefaultEmptyScene();
    auto r = ECS.CreateRayEntity();
    ECS.registry().get<Transform>(r).position = vec3(-2, 0, 0);
    ECS.registry().get<Material>(r).kAmbient
        = ECS.registry().get<Material>(r).kDiffuse
        = vec3(0.f, 0.5f, 1.f);
    auto aabb = ECS.CreateAABBEntity();
    ECS.registry().get<Transform>(aabb).position = vec3(2, 0, 0);
    ECS.registry().get<Transform>(aabb).scale = vec3(2);
    ECS.registry().get<Material>(aabb).kAmbient
        = ECS.registry().get<Material>(aabb).kDiffuse
        = vec3(1.f, 0, 1.f);
    ECS.selectedEnt(r);
}
void GUI::CreateRayvsTriangleScene()
{
    CreateDefaultEmptyScene();
    auto r = ECS.CreateRayEntity();
    ECS.registry().get<Transform>(r).position = vec3(-2, 0, 0);
    ECS.registry().get<Material>(r).kAmbient
        = ECS.registry().get<Material>(r).kDiffuse
        = vec3(0.f, 0.5f, 1.f);
    auto t = ECS.CreateTriangleEntity();
    ECS.registry().get<Transform>(t).position = vec3(2, 0, 0);
    ECS.registry().get<Material>(t).kAmbient
        = ECS.registry().get<Material>(t).kDiffuse
        = vec3(1.f, 0, 1.f);
    ECS.selectedEnt(r);
}
void GUI::CreateRayvsPlaneScene()
{
    CreateDefaultEmptyScene();
    auto r = ECS.CreateRayEntity();
    ECS.registry().get<Transform>(r).position = vec3(-2, 0, 0);
    ECS.registry().get<Material>(r).kAmbient
        = ECS.registry().get<Material>(r).kDiffuse
        = vec3(0.f, 0.5f, 1.f);
    auto p = ECS.CreatePlaneEntity();
    ECS.registry().get<Transform>(p).position = vec3(0, -1, 0);
    ECS.registry().get<Transform>(p).rotation = vec3(-60, 90, 0);
    ECS.registry().get<Transform>(p).scale = vec3(10);
    ECS.registry().get<Material>(p).kAmbient
        = ECS.registry().get<Material>(p).kDiffuse
        = vec3(1.f, 0, 1.f);
    ECS.selectedEnt(r);
}
void GUI::CreatePlanevsAABBScene()
{
    CreateDefaultEmptyScene();
    auto p = ECS.CreatePlaneEntity();
    ECS.registry().get<Transform>(p).position = vec3(0, -1, 0);
    ECS.registry().get<Transform>(p).rotation = vec3(-60, 90, 0);
    ECS.registry().get<Transform>(p).scale = vec3(10);
    ECS.registry().get<Material>(p).kAmbient
        = ECS.registry().get<Material>(p).kDiffuse
        = vec3(1.f, 0, 1.f);
    auto aabb = ECS.CreateAABBEntity();
    ECS.registry().get<Transform>(aabb).position = vec3(0, 1, 0);
    ECS.registry().get<Material>(aabb).kAmbient
        = ECS.registry().get<Material>(aabb).kDiffuse
        = vec3(0.f, 0.5f, 1.f);
    ECS.selectedEnt(aabb);
}
void GUI::CreatePlanevsSphereScene()
{
    CreateDefaultEmptyScene();
    auto p = ECS.CreatePlaneEntity();
    ECS.registry().get<Transform>(p).position = vec3(0, -1, 0);
    ECS.registry().get<Transform>(p).rotation = vec3(-60, 90, 0);
    ECS.registry().get<Transform>(p).scale = vec3(10);
    ECS.registry().get<Material>(p).kAmbient
        = ECS.registry().get<Material>(p).kDiffuse
        = vec3(1.f, 0, 1.f);
    auto s = ECS.CreateSphereEntity();
    ECS.registry().get<Transform>(s).position = vec3(0, 1, 0);
    ECS.registry().get<Material>(s).kAmbient
        = ECS.registry().get<Material>(s).kDiffuse
        = vec3(0.f, 0.5f, 1.f);
    ECS.selectedEnt(s);
}

void GUI::CreateBVScene()
{
    CreateDefaultEmptyScene();
    std::vector<entity> ents;
    auto sw = ECS.CreateMeshEntity("starwars1.obj");
    ECS.registry().get<Transform>(sw).position = vec3(-3, 0, -2);
    ECS.registry().get<Transform>(sw).scale = vec3(0.01f);
    ents.push_back(sw);
    auto c = ECS.CreateMeshEntity("cup.obj");
    ECS.registry().get<Transform>(c).position = vec3(5.5f, 1.f, 0);
    ECS.registry().get<Transform>(c).scale = vec3(10.f);
    ents.push_back(c);
    auto b = ECS.CreateMeshEntity("bunny.obj");
    ECS.registry().get<Transform>(b).position = vec3(4.f, 3.f, -2.f);
    ECS.registry().get<Transform>(b).scale = vec3(10.f);
    ents.push_back(b);
    auto o = ECS.CreateMeshEntity("ogre.obj");
    ECS.registry().get<Transform>(o).position = vec3(-2.f, 3.f, -5.f);
    ents.push_back(o);
    auto r = ECS.CreateMeshEntity("rhino.obj");
    ECS.registry().get<Transform>(r).scale = vec3(0.15f);
    ents.push_back(r);
    ECS.selectedEnt(r);
    unsigned i = 0;
    for (auto ent : ents)
    {
        auto& mesh = ECS.registry().get<Renderable>(ent);
        auto& xform = ECS.registry().get<Transform>(ent);
        auto& bvList = ECS.registry().emplace<BVList>(ent); 
        bvList.push_back(std::make_shared<Aabb>());
        bvList.back()->InitBV(mesh.GetMeshType()); bvList.back()->UpdateBV(xform);
        bvList.push_back(std::make_shared<Sphere>()); if (i == 0) { bvList.back()->isActive = true; }
        bvList.back()->InitBV(mesh.GetMeshType()); bvList.back()->UpdateBV(xform);
        bvList.push_back(std::make_shared<Sphere>(Sphere(BSPHERE_Larssons))); if (i == 1 || i == 3) {
            bvList.back()->isActive = true;
        }
        bvList.back()->InitBV(mesh.GetMeshType()); bvList.back()->UpdateBV(xform);
        bvList.push_back(std::make_shared<Sphere>(Sphere(BSPHERE_PCA)));  if (i == 2 || i == 3) {
            bvList.back()->isActive = true;
        }
        bvList.back()->InitBV(mesh.GetMeshType()); bvList.back()->UpdateBV(xform);
        bvList.push_back(std::make_shared<Obb>());
        bvList.back()->InitBV(mesh.GetMeshType()); bvList.back()->UpdateBV(xform);
        ++i;
    }
}

void GUI::CreateBVHScene()
{
    CreateDefaultEmptyScene();
    std::vector<entity> ents;
    auto sw = ECS.CreateMeshEntity("starwars1.obj");
    ECS.registry().get<Transform>(sw).position = vec3(-3, 0, -2);
    ECS.registry().get<Transform>(sw).rotation = vec3(0, 112, -2);
    ECS.registry().get<Transform>(sw).scale = vec3(0.01f);
    ents.push_back(sw);
    auto c = ECS.CreateMeshEntity("cup.obj");
    ECS.registry().get<Transform>(c).position = vec3(5.5f, 1.f, 0);
    ECS.registry().get<Transform>(c).scale = vec3(10.f);
    ents.push_back(c);
    auto b = ECS.CreateMeshEntity("bunny.obj");
    ECS.registry().get<Transform>(b).position = vec3(7.f, 1.f, 1.f);
    ECS.registry().get<Transform>(b).scale = vec3(10.f);
    ents.push_back(b);
    auto o = ECS.CreateMeshEntity("ogre.obj");
    ECS.registry().get<Transform>(o).position = vec3(0.f, 3.f, -3.f);
    ents.push_back(o);
    auto r = ECS.CreateMeshEntity("rhino.obj");
    ECS.registry().get<Transform>(r).position = vec3(-1.f, -1.f, -8.f);
    ECS.registry().get<Transform>(r).scale = vec3(0.15f);
    ents.push_back(r);
    auto s = ECS.CreateMeshEntity("4Sphere.obj");
    ECS.registry().get<Transform>(s).position = vec3(4.f, 1.f, -6.f);
    ECS.registry().get<Transform>(s).scale = vec3(0.01f);
    ents.push_back(s);
    auto h = ECS.CreateMeshEntity("head.obj");
    ECS.registry().get<Transform>(h).position = vec3(3.f, 0.f, 1.f);
    ents.push_back(h);
    auto l = ECS.CreateMeshEntity("lucy_princeton.obj");
    ECS.registry().get<Transform>(l).position = vec3(-1.f, 0.f, -9.f);
    ECS.registry().get<Transform>(l).scale = vec3(0.002f);
    ents.push_back(l);
    ECS.selectedEnt(l);
    for (auto ent : ents)
    {
        auto& mesh = ECS.registry().get<Renderable>(ent);
        auto& xform = ECS.registry().get<Transform>(ent);
        auto& bvList = ECS.registry().emplace<BVList>(ent);
        bvList.push_back(std::make_shared<Aabb>());
        bvList.back()->InitBV(mesh.GetMeshType()); bvList.back()->UpdateBV(xform); bvList.back()->isActive = false;
        bvList.push_back(std::make_shared<Sphere>(Sphere(BSPHERE_PCA)));
        bvList.back()->InitBV(mesh.GetMeshType()); bvList.back()->UpdateBV(xform); bvList.back()->isActive = false;
        bvList.push_back(std::make_shared<Obb>());
        bvList.back()->InitBV(mesh.GetMeshType()); bvList.back()->UpdateBV(xform); bvList.back()->isActive = false;
    }
}

void GUI::CreateOctreeKdtreeScene()
{
    //CreateBVHScene(); // test on smaller scene render.cpp line 59 Init() isPowerPlant = false;
    CreateDefaultEmptyScene();
    std::vector<entity> ents;
    // load powerplant model
    for (auto const& [name, buf] : Renderable::GetBuffers())
    {
        if (name.find(".obj") != std::string::npos && name.find("OBB") == std::string::npos)
        {  
            auto tmp = ECS.CreateMeshEntity(name); 
            auto& bvList = ECS.registry().emplace<BVList>(tmp);
            bvList.push_back(std::make_shared<Aabb>());
            bvList.back()->InitBV(name); bvList.back()->UpdateBV(ECS.registry().get<Transform>(tmp));
            bvList.back()->isActive = false;
            ents.push_back(tmp); 

        }
    }
}