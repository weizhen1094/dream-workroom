/**
@file    viewportgui.hpp
@author  weizhen.tan@digipen.edu
@date    19/05/2025

This file contains the declaration of the ViewportGUI class.

*//*__________________________________________________________________________*/

#ifndef VIEWPORT_GUI_HPP
#define VIEWPORT_GUI_HPP
/*                                                                      guard
----------------------------------------------------------------------------- */

#include <gui/iguiwindow.hpp>
#include <buffer.hpp>
#include <ecs.hpp>
/*                                                                   includes
----------------------------------------------------------------------------- */

/**
 * @class ViewportGUI
 * @brief This class is responsible for the viewport GUI window.
 */
class ViewportGUI : public IGUIWindow
{

public:

    void InitWindow() override;

    void Render() override;

    void CleanUp() override;

    /*
     * Ctors and Dtor.
     */

    ViewportGUI(vec3 _camPos, vec3 _camRot, float _camNear, float _camFar, std::string _GUIWindowTitle, vec2 _GUIWindowPos, vec2 _GUIWindowSize);
    ~ViewportGUI() {};

private:

    Framebuffer  m_FrameBuffer;
    entity m_Camera; ///<  the camera and fbo attached to the viewport
};

#endif /* VIEWPORT_GUI_HPP */