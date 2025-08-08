/**
@file    octkdtreegui.hpp
@author  weizhen.tan@digipen.edu
@date    22/07/2025

This file contains the declaration of the OctKDTreeGUI GUIWindow class.

*//*__________________________________________________________________________*/

#ifndef OCT_KD_TREE__GUI_HPP
#define OCT_KD_TREE__GUI_HPP
/*                                                                      guard
----------------------------------------------------------------------------- */

#include <gui/iguiwindow.hpp>
/*                                                                   includes
----------------------------------------------------------------------------- */

/**
 * @class OctKDTreeGUI
 * @brief This class is responsible for the octree / k-d tree GUI window for 
 * project 4.
 */
class OctKDTreeGUI : public IGUIWindow
{

public:

    void InitWindow() override;

    void Render() override;

    void CleanUp() override;

    /*
     * Ctors and Dtor.
     */

    OctKDTreeGUI(std::string _GUIWindowTitle, vec2 _GUIWindowPos, vec2 _GUIWindowSize)
        : IGUIWindow(_GUIWindowTitle, _GUIWindowPos, _GUIWindowSize) {
    };
    ~OctKDTreeGUI() {};

private:

};

#endif /* OCT_KD_TREE__GUI_HPP */