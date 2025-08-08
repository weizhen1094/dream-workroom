/**
@file    window.hpp
@author  weizhen.tan@digipen.edu
@date    08/05/2025

This file contains the declaration of the Window class that encapsulates all GLFW
logic.

*//*__________________________________________________________________________*/

#ifndef WINDOW_HPP
#define WINDOW_HPP
/*                                                                      guard
----------------------------------------------------------------------------- */

#include <GL/glew.h>  
#include <GLFW/glfw3.h>
#include <string>
#include <math.hpp>
/*                                                                   includes
----------------------------------------------------------------------------- */

/**
 * @class Window
 * @brief This class is responsible for creation, update and deletion of GLFW
 * system and windows. It also wraps GLFW callbacks for input and error as well
 * as calculation of FPS in Window::Update() using glfwGetTime().
 */
class Window
{

public:

    /**
     * @brief Initializes GLFW.
     * @return true if successfully initialized, false otherwise
     */
    static bool Init();
        /**
         * @brief Initializes window.
         * @return true if successfully initialized, false otherwise
         */
        bool InitWindow();
    /**
     * @brief Calls glfwPollEvents() and calculates FPS using glfwGetTime().
     */
    static void Update();
        void UpdateWindow();
        /**
         * @brief Destroys window.
         */
        void DestroyWindow();
    /**
     * @brief Terminates GLFW.
     */
	static void Terminate();


    /**
     * GLFW Callbacks
     */

    bool ShouldClose();
    void SwapBuffers();
    static void OnError(int _err, char const* _desc);
    static void OnKeyboardInput(GLFWwindow* _pWin, int _key, int _scancode, int _action, int _mod);
    static void OnMouseButtonInput(GLFWwindow* _pWin, int _btn, int _action, int _mod);
    static void OnMouseScrollInput(GLFWwindow* _pWin, double _xOffset, double _yOffset);
    static void OnMousePos(GLFWwindow* _pWin, double _xPos, double _yPos);
    static void OnWindowResize(GLFWwindow* _pWin, int _width, int _height);
    static void DisableCursorMode(GLFWwindow* _pWin);
    static void EnableCursorMode(GLFWwindow* _pWin);

    /*
     * Ctors and Dtor.
     */

    Window(ivec2 _size = ivec2(1280, 720), std::string = "CS3151", bool _isVisible = true);
    ~Window();


    /*
     * Mutators and Accessors.
     */

    GLFWwindow* GetWindowPtr() const;
    std::string const& GetWindowTitle() const;
    void SetWindowTitle(std::string _s);
    void SetWindowSize(ivec2 _size);
    ivec2 GetWindowSize() const;

    double GetDeltaTime() const;
    double GetFPS() const;
    double GetElaspedTime() const;

private:

    std::string m_WindowTitle;
    ivec2       m_WindowSize;
    GLFWwindow* m_pWindow;
    static std::vector<Window*> m_pWindows;

    // TODO:? shift fps out of window class
    static double   m_DeltaTime; 
    static unsigned m_TargetFramerate;
    static unsigned m_Frames;    ///< number of frames since application started
    static double   m_StartTime; ///< time application started
    
};


#endif /* WINDOW_HPP */
