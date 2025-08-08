/**
@file    iguiwindow.hpp
@author  weizhen.tan@digipen.edu
@date    19/05/2025

This file contains the declaration of the IGUIWindow interface and 
    ConsoleGUI,
    InspectorGUI GUIWindow classes.

*//*__________________________________________________________________________*/

#ifndef GUIWINDOW_HPP
#define GUIWINDOW_HPP
/*                                                                      guard
----------------------------------------------------------------------------- */

#include <filesystem>
#include <vector>
#include <string>
#include <imgui.h>
#include <math.hpp>
/*                                                                   includes
----------------------------------------------------------------------------- */

/**
 * @class IGUIWindow
 * @brief This is the interface for GUI window classes.
 */
class IGUIWindow 
{

public:

    virtual void InitWindow() = 0;
    virtual void Render() = 0;
    virtual void CleanUp() = 0;

    IGUIWindow(std::string _GUIWindowTitle, vec2 _GUIWindowPos, vec2 _GUIWindowSize)
        : m_GUIWindowTitle(_GUIWindowTitle+"##"+std::to_string(counter)), m_GUIWindowPos(_GUIWindowPos), m_GUIWindowSize(_GUIWindowSize), m_IsWindowOpen(true), m_GUIWindowFlags(ImGuiWindowFlags_NoCollapse) { ++counter; }
    virtual ~IGUIWindow() = default;

    bool& isWindowOpen()             { return m_IsWindowOpen; };
    void isWindowOpen(bool _isOpen) { m_IsWindowOpen = _isOpen; };

protected:

    std::string m_GUIWindowTitle;
    vec2 m_GUIWindowPos;
    vec2 m_GUIWindowSize;
    ImGuiWindowFlags m_GUIWindowFlags;
    bool m_IsWindowOpen;

    static unsigned counter;
};

/**
 * @class ConsoleGUI
 * @brief This class is responsible for display .txt file e.g. readme.txt.
 */
class DisplayTxtFileGUI : public IGUIWindow
{

public:

    void InitWindow() override;

    void Render() override;

    void CleanUp() override;

    /*
     * Ctors and Dtor.
     */

    DisplayTxtFileGUI(std::string _filePath, std::string _GUIWindowTitle, vec2 _GUIWindowPos, vec2 _GUIWindowSize)
        : m_TxtFilepath(_filePath), IGUIWindow(std::filesystem::path(_filePath).filename().string(), _GUIWindowPos, _GUIWindowSize) {};
    ~DisplayTxtFileGUI() {};

private:
    std::string m_TxtFilepath;
    std::vector<std::string> m_TxtFile;
};

#endif /* GUIWINDOW_HPP */