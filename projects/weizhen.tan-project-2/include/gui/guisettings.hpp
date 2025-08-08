/**
@file    guisettings.hpp
@author  weizhen.tan@digipen.edu
@date    19/05/2025

This file contains imgui style settings.

*//*__________________________________________________________________________*/

#ifndef GUISETTINGS_HPP
#define GUISETTINGS_HPP
/*                                                                      guard
----------------------------------------------------------------------------- */

// imgui style setting // TODO: settings menu and to serialize? 
static float constexpr WINDOWTITLE_FONTSIZE = 15.f;
static float constexpr WINDOW_PADDING = 8.f;
static float constexpr WINDOWFRAME_PADDINGX = 4.f;
static float constexpr WINDOWFRAME_PADDINGY = 3.f;
static float constexpr WINDOWTITLE_HEIGHT = WINDOWTITLE_FONTSIZE + WINDOWFRAME_PADDINGY * 2;
static float constexpr MAINMENUBAR_HEIGHT = WINDOWTITLE_FONTSIZE + WINDOWFRAME_PADDINGY;
static float constexpr BUTTON_HEIGHT = WINDOWTITLE_FONTSIZE + WINDOWFRAME_PADDINGY;
static float constexpr INDENT_SPACING = 21.f;

#endif /* GUISETTINGS_HPP */