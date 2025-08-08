/**
@file    iguiwindow.cpp
@author  weizhen.tan@digipen.edu
@date    19/05/2025

This file contains the definitions of the GUI class and helper functions that
encapsulates all Dear ImGui logic.

*//*__________________________________________________________________________*/

#include <gui/iguiwindow.hpp>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_glfw.h>
//#include <imguizmo.h>
#include <gui/guisettings.hpp>
#include <fstream>
/*                                                                   includes
----------------------------------------------------------------------------- */
unsigned IGUIWindow::counter = 0;

void DisplayTxtFileGUI::InitWindow()
{
    std::ifstream ifs;
    ifs.open(m_TxtFilepath, std::fstream::in);
    std::string line;
    while (std::getline(ifs, line))
    {  m_TxtFile.push_back(line); }
    ifs.close();
}

void DisplayTxtFileGUI::Render()
{
    ImGui::SetNextWindowPos({ m_GUIWindowPos.x, m_GUIWindowPos.y }, ImGuiCond_Once);
    ImGui::SetNextWindowSize({ m_GUIWindowSize.x, m_GUIWindowSize.y }, ImGuiCond_Once);
    if (ImGui::Begin(m_GUIWindowTitle.c_str(), &m_IsWindowOpen, m_GUIWindowFlags))
    {
        ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor::HSV(0.3f, 1.0f, 1.0f));
        ImGui::SeparatorText("drag to dock this window anywhere you like or close it! (reopen via menu bar at top of window)");
        ImGui::PopStyleColor(1);
        for (auto const& line : m_TxtFile) {  ImGui::Text(line.c_str()); }
    }
    ImGui::End();
}

void DisplayTxtFileGUI::CleanUp() {}