/**
@file    window.cpp
@author  weizhen.tan@digipen.edu
@date    08/05/2025

This file contains the definition of the Window class that encapsulates all GLFW
logic.

*//*__________________________________________________________________________*/

#include <window.hpp>
#include <iostream> // for debug output to console
#include <input.hpp>
/*                                                                   includes
----------------------------------------------------------------------------- */

bool Window::Init()
{
	// glfw init + create window
	if (!glfwInit()) { std::cout << "GLFW init has failed" << std::endl; return false; }

	glfwSetErrorCallback(OnError);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
	//glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Required for macOS

	return true;
}

bool Window::InitWindow()
{
	m_pWindow = glfwCreateWindow(m_WindowSize.x, m_WindowSize.y, m_WindowTitle.c_str(), nullptr, nullptr);
	if (m_pWindow == nullptr) { return false; }
	m_pWindows.push_back(this);
	glfwMakeContextCurrent(m_pWindow);
	glfwSwapInterval(1); // enable vsync

	glfwSetKeyCallback(m_pWindow, OnKeyboardInput);
	glfwSetMouseButtonCallback(m_pWindow, OnMouseButtonInput);
	glfwSetScrollCallback(m_pWindow, OnMouseScrollInput);
	glfwSetCursorPosCallback(m_pWindow, OnMousePos);
	glfwSetWindowSizeCallback(m_pWindow, OnWindowResize);

	// Ensure ESC key can be captured properly
	glfwSetInputMode(m_pWindow, GLFW_STICKY_KEYS, GL_TRUE);

	Input::GetInstance().SetMousePos({ static_cast<float>(m_WindowSize.x) * 0.5f, static_cast<float>(m_WindowSize.y) * 0.5f });
	glfwSetCursorPos(m_pWindow, static_cast<double>(Input::GetInstance().GetMousePos().x), static_cast<double>(Input::GetInstance().GetMousePos().y));

	// TODO:? shld this be shfited out
	// glew init 
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (err != GLEW_OK) { std::cout << "GLEW init has failed: " << glewGetErrorString(err) << std::endl; return false; }
	
	// Enable depth testing for proper 3D rendering
	glEnable(GL_DEPTH_TEST);

	//glEnable(GL_DEBUG_OUTPUT);
	//glDebugMessageCallback(OnGLDebugMessage, NULL);

	m_StartTime = glfwGetTime();

	return true;
}

double	 Window::m_StartTime		{ 0.0 };
double	 Window::m_DeltaTime		{ 0.0 };
unsigned Window::m_TargetFramerate	{ 60 };
unsigned Window::m_Frames			{ 0 };
void Window::Update()
{
	glfwPollEvents(); 

	static double prevTime = glfwGetTime();
	double currTime = glfwGetTime();
	m_DeltaTime = currTime - prevTime;
	prevTime = currTime; ++m_Frames;
	do
	{
		currTime = glfwGetTime(); // wait for next frame
		m_DeltaTime = currTime - prevTime;
	} while (m_DeltaTime < 1.0 / m_TargetFramerate);
}

void Window::Terminate() { glfwTerminate(); }

void Window::DestroyWindow() { glfwDestroyWindow(m_pWindow); }


Window::Window(ivec2 _size, std::string _title, bool _isVisible) 
	: m_WindowTitle(_title), m_WindowSize(_size), m_pWindow(nullptr) { }

Window::~Window() { }


bool Window::ShouldClose() { return glfwWindowShouldClose(m_pWindow); }
void Window::SwapBuffers() { glfwMakeContextCurrent(m_pWindow); glfwSwapBuffers(m_pWindow); }

void Window::OnError(int _err, char const* _desc)
{
	(void)_err; (void)_desc;
#ifdef _DEBUG
	std::cout << "GLFW error code " << _err << ": " << _desc << std::endl;
#endif
}

void Window::OnKeyboardInput(GLFWwindow* _pWin, int _key, int _scancode, int _action, int _mod)
{
	(void)_mod; (void)_scancode;
	switch (_action)
	{
	case GLFW_PRESS:
	{
#ifdef _DEBUG
	//	std::cout << "Key pressed" << std::endl;
#endif
	} break;
	case GLFW_REPEAT:
	{
#ifdef _DEBUG
	//	std::cout << "Key repeated" << std::endl;
#endif
	} break;
	case GLFW_RELEASE:
	{
#ifdef _DEBUG
	//	std::cout << "Key released" << std::endl;
#endif
	} break;
	}

	Input::GetInstance().SetKeyState(_key, _action);
}

void Window::OnMouseButtonInput(GLFWwindow* _pWin, int _btn, int _action, int _mod)
{
	(void)_pWin; (void)_mod;
	switch (_btn)
	{
	case GLFW_MOUSE_BUTTON_LEFT:
	{
#ifdef _DEBUG
		//std::cout << "LMB ";
#endif
	} break;
	case GLFW_MOUSE_BUTTON_RIGHT:
	{
#ifdef _DEBUG
		//std::cout << "RMB ";
#endif
	} break;
	}
	switch (_action)
	{
	case GLFW_PRESS:
	{
#ifdef _DEBUG
		//std::cout << "pressed" << std::endl;
#endif
	} break;
	case GLFW_RELEASE:
	{
#ifdef _DEBUG
	//	std::cout << "released" << std::endl;
#endif
	} break;
	}

	Input::GetInstance().SetMouseBtnState(_btn, _action);
}

void Window::OnMouseScrollInput(GLFWwindow* _pWin, double _xOffset, double _yOffset)
{
	(void)_pWin;
#ifdef _DEBUG
//	std::cout << "Mouse scroll wheel offset: ("
	//	<< _xOffset << ", " << _yOffset << ")" << std::endl;
#endif
	Input::GetInstance().SetMouseScrollOffset({ _xOffset, _yOffset });
}

void Window::OnMousePos(GLFWwindow* _pWin, double _xPos, double _yPos)
{
	(void)_pWin;
#ifdef _DEBUG
	 //std::cout << "Mouse cursor position: (" << _xPos << ", " << _yPos << ")" << std::endl;
	//std::cout << "Mouse cursor delta: " << to_string(Input::GetInstance().GetMouseDelta()) << std::endl;
#endif
	Input::GetInstance().SetMouseDelta({ _xPos - Input::GetInstance().GetMousePos().x, _yPos - Input::GetInstance().GetMousePos().y });
	Input::GetInstance().SetMousePos({ _xPos, _yPos });
}

std::vector<Window*> Window::m_pWindows;
void Window::OnWindowResize(GLFWwindow* _pWin, int _width, int _height) 
{
	glfwSetWindowSize(_pWin, _width, _height); 
	for (Window* wind : m_pWindows) // update m_WindowSize
	{ if (wind->GetWindowPtr() == _pWin) { wind->SetWindowSize(ivec2(_width, _height)); return; } }
}

void Window::DisableCursorMode(GLFWwindow* _pWin)
{ glfwSetInputMode(_pWin, GLFW_CURSOR, GLFW_CURSOR_DISABLED); }
void Window::EnableCursorMode(GLFWwindow* _pWin)
{ glfwSetInputMode(_pWin, GLFW_CURSOR, GLFW_CURSOR_NORMAL); }


GLFWwindow* Window::GetWindowPtr() const			{ return m_pWindow; }

std::string const& Window::GetWindowTitle() const	{ return m_WindowTitle; }
void Window::SetWindowTitle(std::string _s)			{ m_WindowTitle = _s; glfwSetWindowTitle(m_pWindow, m_WindowTitle.c_str()); }

void Window::SetWindowSize(ivec2 _size)				{ m_WindowSize = _size; glfwSetWindowSize(m_pWindow, _size.x, _size.y); }
ivec2 Window::GetWindowSize() const					{ return m_WindowSize; }

double Window::GetDeltaTime() const					{ return m_DeltaTime; }
double Window::GetFPS() const						{ return 1.0 / m_DeltaTime; }
double Window::GetElaspedTime() const				{ return glfwGetTime() - m_StartTime;}
