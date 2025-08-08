/**
 * @file main.cpp
 * @brief Entry point for a simple OpenGL application rendering a 3D cube.
 *
 * This program initializes an OpenGL context using GLFW and GLEW, sets up a
 * basic rendering scene, and continuously updates and renders a cube object.
 */

 // Scene class with basic lifecycle and renderable object management
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include <stdexcept>

#include <window.hpp>
#include <input.hpp>
#include <gui/gui.hpp>
#include <systems/isystem.hpp>
#include <systems/render.hpp>
#include <systems/movement.hpp>
#include <systems/collision.hpp>
#include <buffer.hpp>

/**
 * @brief Main function that sets up the OpenGL window and rendering loop.
 * @return Exit status (0 if successful).
 */
int main() 
{
#ifdef _DEBUG
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    // init glfw and glew
    if (!Window::Init()) { return -1; }

    // create window
    Window mainWindow = Window(ivec2(1280, 720), "CS3151 | weizhen.tan-project-2");
    if(!mainWindow.InitWindow()) { throw std::runtime_error("Could not create window"); }

    // create systems
    std::vector<std::unique_ptr<ISystem>> systems;
    systems.push_back(std::make_unique<Movement>());
    systems.push_back(std::make_unique<Collision>()); 
    systems.push_back(std::make_unique<Render>()); // loads and initializes meshes
    for (auto const& sys : systems) { sys->Init(); }

    // init imgui
    GUI::GetInstance().InitGUI(&mainWindow); // contains scene initialization


    // Main application loop
     while (!(Input::GetInstance().KeyTriggered(KEY_ESCAPE)
            || mainWindow.ShouldClose()))
    {
        Input::GetInstance().Update();
        Window::Update();// glfwPollEvents();         ///< Process input and events
        GUI::GetInstance().UpdateGUI();

        // disable/enable cursor for camera // this feels v out of place..
        if(Input::GetInstance().MouseDown(MOUSE_BUTTON_LEFT))
        { Window::DisableCursorMode(mainWindow.GetWindowPtr()); }
        else { Window::EnableCursorMode(mainWindow.GetWindowPtr());  }
        
        // update system
        for (auto const& sys : systems) { sys->Update(); }
#ifdef _DEBUG
        Buffer::CheckOpenGLError();
#endif 

        GUI::GetInstance().RenderGUI();
        mainWindow.SwapBuffers();//glfwSwapBuffers(window);  ///< Swap front and back buffers
    }

    // Cleanly shut down and exit
    for (auto const& sys : systems) { sys->CleanUp(); }
    GUI::GetInstance().TerminateGUI();
    mainWindow.DestroyWindow();
    Window::Terminate();

    return 0;
}