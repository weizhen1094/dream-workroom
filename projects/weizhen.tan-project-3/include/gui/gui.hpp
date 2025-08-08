/**
@file    gui.hpp
@author  weizhen.tan@digipen.edu
@date    19/05/2025

This file contains the declaration of the GUI class and helper functions that
encapsulates all Dear ImGui logic.

*//*__________________________________________________________________________*/

#ifndef GUI_HPP
#define GUI_HPP
/*                                                                      guard
----------------------------------------------------------------------------- */

#include <vector>
#include <memory>
#include <window.hpp>
#include <isingleton.hpp>
#include <math.hpp>
#include <gui/iguiwindow.hpp>
/*                                                                   includes
----------------------------------------------------------------------------- */

/**
 * @class GUI
 * @brief This class is responsible for initialization, update, render and
 * termination of Dear ImGui as well as creation, management and cleanup of the 
 * engine's GUI windows. Temporarily responsible for scene creation until serialization is added.
 */
class GUI : public ISingleton<GUI>
{

public:

    /**
      * Helper functions that encapsulates all Dear ImGui logic.
      */
    bool InitGUI(Window* _pWin);
    void UpdateGUI();
    void RenderGUI();
    void TerminateGUI();

private:

    friend class ISingleton<GUI>;

    /**
      * Helper functions that encapsulates all Dear ImGui logic.
      */
    void RenderMainMenuBar();
    void RenderMainWindow();
    void DockChildWindows(unsigned& _id);
    void CreateSceneHierarchyGUI();
    void CreateBVHGUI();
    void CreateInspectorGUI();
    void CreateMainViewportGUI();
    void CreateSubViewportGUI();
    void CreateTxtFileGUI(std::string _filePath);

    /**
      * Helper functions to create scenes. // TODO: remove when serialization is added
      */
    void CreateDefaultEmptyScene();
    void CreateBVScene();
    void CreateBVHScene();

    /*
     * Ctors and Dtor.
     */

    GUI() : m_MainWindowSize(vec2(1280,720)) {};
    ~GUI() {};

    std::vector<std::unique_ptr<IGUIWindow>> m_GUIWindows;
    vec2 m_MainWindowSize;
    Window* m_pWin;
};

#endif /* GUI_HPP */