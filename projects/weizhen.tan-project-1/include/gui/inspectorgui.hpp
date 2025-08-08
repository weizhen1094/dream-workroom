/**
@file    inspectorgui.hpp
@author  weizhen.tan@digipen.edu
@date    19/05/2025

This file contains the declaration of the InspectorGUI class.

*//*__________________________________________________________________________*/

#ifndef INSPECTOR_GUI_HPP
#define INSPECTOR_GUI_HPP
/*                                                                      guard
----------------------------------------------------------------------------- */

#include <gui/iguiwindow.hpp>
/*                                                                   includes
----------------------------------------------------------------------------- */

/**
 * @class InspectorGUI
 * @brief This class is responsible for the inspector GUI window.
 */
class InspectorGUI : public IGUIWindow
{

public:

    /**
     * @brief Display the components the selected entity has.
     */
    void DisplayComponents();

    /**
      * @brief Creates a Menu using CollapsingHeader and Selectable for
      * adding components to the selected entity.
      */
    void CreateComponentMenu();

    /*
     * Helper functions.
     */
    void DisplayCameraComp();
    void DisplayLightComp();
    void DisplayMeshComp();
    void DisplayBVComp();
    void DisplayString(std::string& _str);
    void DisplayFloat(std::string _title, float& _data);
    bool DisplayBool(std::string _title, bool& _data);
    bool DisplayVec3(std::string _title, vec3& _data);
    void DisplayClrVec3(std::string _title, vec3& _data);

    /*
     * Abstract functions.
     */

    void InitWindow() override;
    void Render() override;
    void CleanUp() override;

    /*
     * Ctors and Dtor.
     */

    InspectorGUI(std::string _GUIWindowTitle, vec2 _GUIWindowPos, vec2 _GUIWindowSize)
        : IGUIWindow(_GUIWindowTitle, _GUIWindowPos, _GUIWindowSize) {};
    ~InspectorGUI() {};

private:
};

#endif /* INSPECTOR_GUI_HPP */