/**
@file    bvhgui.hpp
@author  weizhen.tan@digipen.edu
@date    11/07/2025

This file contains the declaration of the BVHGUI GUIWindow class.

*//*__________________________________________________________________________*/

#ifndef BVH_GUI_HPP
#define BVH_GUI_HPP
/*                                                                      guard
----------------------------------------------------------------------------- */

#include <gui/iguiwindow.hpp>
/*                                                                   includes
----------------------------------------------------------------------------- */

/**
 * @class BVHGUI
 * @brief This class is responsible for the bounding volume hierarchy GUI window.
 */
class BVHGUI : public IGUIWindow
{

public:

    void InitWindow() override;

    void Render() override;

    void CleanUp() override;

    /*
     * Ctors and Dtor.
     */

    BVHGUI(std::string _GUIWindowTitle, vec2 _GUIWindowPos, vec2 _GUIWindowSize)
        : IGUIWindow(_GUIWindowTitle, _GUIWindowPos, _GUIWindowSize) {
    };
    ~BVHGUI() {};

private:

};

#endif /* BVH_GUI_HPP */