/**
@file    scenehierarchygui.hpp
@author  weizhen.tan@digipen.edu
@date    19/05/2025

This file contains the declaration of the SceneHierarchyGUI class.

*//*__________________________________________________________________________*/

#ifndef SCENEHIERARCHY_GUI_HPP
#define SCENEHIERARCHY_GUI_HPP
/*                                                                      guard
----------------------------------------------------------------------------- */

#include <gui/iguiwindow.hpp>
#include <components/boundingvolume.hpp>
/*                                                                   includes
----------------------------------------------------------------------------- */

/**
 * @class SceneHierarchyGUI
 * @brief This class is responsible for the scene hierarchy GUI window.
 */
class SceneHierarchyGUI : public IGUIWindow
{

public:

    /**
      * @brief Displays draggable EntityNames based on RenderOrder.
      */
    void DisplayEntities();
    /**
      * @brief Creates a Menu using CollapsingHeader and Selectable for 
      * entity creation.
      */
    void CreateEntityMenu();
    /**
      * @brief Deletes all entities except those containing camera comp from scene.
      */
    void ClearSceneBtn();
    static void ClearScene();


    /*
     * Abstract functions.
     */

    void InitWindow() override;
    void Render() override;
    void CleanUp() override;

    /*
     * Ctors and Dtor.
     */

    SceneHierarchyGUI(std::string _GUIWindowTitle, vec2 _GUIWindowPos, vec2 _GUIWindowSize)
        : IGUIWindow(_GUIWindowTitle, _GUIWindowPos, _GUIWindowSize) { };
    ~SceneHierarchyGUI() {};

private:
    void ToggleBoundingVolumes(); // for ease of project 2
    void ToggleBoundingVolumes(BV_TYPE); // for ease of project 2
};

#endif /* SCENEHIERARCHY_GUI_HPP */