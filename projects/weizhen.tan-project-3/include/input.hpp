/**
@file    input.hpp
@author  weizhen.tan@digipen.edu
@date    08/05/2025

This file contains the Input class.

*//*__________________________________________________________________________*/

#ifndef INPUT_HPP
#define INPUT_HPP
/*                                                                      guard
----------------------------------------------------------------------------- */

#include <bitset>
#include <isingleton.hpp>
#include <math.hpp>
/*                                                                   includes
----------------------------------------------------------------------------- */

/**
 * @class Input
 * @brief This class is responsible for storing and updating the states of key
 * presses and mouse buttons.
 */
class Input : public ISingleton<Input>
{

public:

	constexpr static int MAX_KEYS = 348; ///<  GLFW_KEY_LAST

	constexpr static int MAX_BTNS = 7;	 ///<  GLFW_MOUSE_BUTTON_LAST;



	/**
	 * @brief Updates the key states.
	 */
	void Update() { m_PrevKeyState = m_CurrKeyState; m_PrevBtnState = m_CurrBtnState; }
	/**
	 * @brief Reset all key states and mouse button states.
	 */
	void Reset() 
	{
		for (int key = 0; key < MAX_KEYS; ++key)
		{
			m_CurrKeyState[key] = false; m_PrevKeyState[key] = false;
		}
		for (int btn = 0; btn < MAX_BTNS; ++btn)
		{
			m_CurrBtnState[btn] = false; m_PrevBtnState[btn] = false;
		}
	}

	/**
	 * @brief Sets a key state.
	 */
	void SetKeyState(const int _key, const int _action) { if (_key >= 0 && _key < MAX_KEYS) { m_CurrKeyState.set(_key, _action); } }
	/**
	 * @brief Sets a mouse button state.
	 */
	void SetMouseBtnState(int _btn, int _action) { if (_btn >= 0 && _btn < MAX_BTNS) { m_CurrBtnState.set(_btn, _action); } }


	/*
	 * Key presses check functions.
	 * | ------------- | -------------------------- |
	 * | key triggered | // currState 1 prevState 0 |
     * | ------------- | -------------------------- |
	 * | key down      | // currState 1 prevState 1 |
	 * | key released  | // currState 0 prevState 1 |
	 * | ------------- | -------------------------- |
	 */
	 
	 /**
	  * @brief Checks if key press is triggered. // currState 1 prevState 0
	  */
	bool KeyTriggered(const int _key) { return m_CurrKeyState[_key] && !m_PrevKeyState[_key]; }
	/**
	  * @brief Checks if key press is being held down. // currState 1 prevState 1
	  */
	bool KeyDown(const int _key) { return m_CurrKeyState[_key] && m_PrevKeyState[_key]; }
	/**
	  * @brief Checks if key press is released. // currState 0 prevState 1
	  */
	bool KeyReleased(const int _key) { return !m_CurrKeyState[_key] && m_PrevKeyState[_key]; }

	/**
	  * @brief Checks if mouse button is being held down. // currState 1 prevState 1
	  */
	bool MouseDown(int _btn) { return m_CurrBtnState[_btn] && m_PrevBtnState[_btn]; }
	/**
	  * @brief Checks if mouse button is released. // currState 0 prevState 1
	  */
	bool MouseReleased(int _btn) { return !m_CurrBtnState[_btn] && m_PrevBtnState[_btn]; }

	/*
	 * Mutators and Accessors.
	 */

	vec2 GetMouseScrollOffset()						{ return m_MouseScrollOffset; }
	void SetMouseScrollOffset(vec2 _scrollOffset)	{ m_MouseScrollOffset = _scrollOffset; }
	vec2 GetMousePos()								{ return m_MousePos; }
	void SetMousePos(vec2 _pos)						{ m_MousePos = _pos; }
	vec2 GetMouseDelta()							{ return m_MouseDelta; }
	void SetMouseDelta(vec2 _delta)					{ m_MouseDelta = _delta; }

private:

	friend class ISingleton<Input>;

	/*
	 * Ctors and Dtor.
	 */

	Input() : m_MouseScrollOffset({ 0.f, 0.f }), m_MousePos({ 0.f, 0.f }), m_MouseDelta({ 0.f, 0.f }) {}
	~Input() {}

	std::bitset<MAX_KEYS> m_CurrKeyState, m_PrevKeyState;

	std::bitset<MAX_BTNS> m_CurrBtnState, m_PrevBtnState;

	vec2 m_MouseScrollOffset;
	vec2 m_MousePos;
	vec2 m_MouseDelta;

};

#define KEYTRIGGERED(_key) Input::GetInstance().KeyTriggered(_key) // macro for easy access
#define KEYDOWN(_key) Input::GetInstance().KeyDown(_key) // macro for easy access
#define KEYRELEASED(_key) Input::GetInstance().KeyReleased(_key) // macro for easy access

#define KEY_SPACE              32
#define KEY_APOSTROPHE         39  /* ' */
#define KEY_COMMA              44  /* , */
#define KEY_MINUS              45  /* - */
#define KEY_PERIOD             46  /* . */
#define KEY_SLASH              47  /* / */
#define KEY_0                  48
#define KEY_1                  49
#define KEY_2                  50
#define KEY_3                  51
#define KEY_4                  52
#define KEY_5                  53
#define KEY_6                  54
#define KEY_7                  55
#define KEY_8                  56
#define KEY_9                  57
#define KEY_SEMICOLON          59  /* ; */
#define KEY_EQUAL              61  /* = */
#define KEY_A                  65
#define KEY_B                  66
#define KEY_C                  67
#define KEY_D                  68
#define KEY_E                  69
#define KEY_F                  70
#define KEY_G                  71
#define KEY_H                  72
#define KEY_I                  73
#define KEY_J                  74
#define KEY_K                  75
#define KEY_L                  76
#define KEY_M                  77
#define KEY_N                  78
#define KEY_O                  79
#define KEY_P                  80
#define KEY_Q                  81
#define KEY_R                  82
#define KEY_S                  83
#define KEY_T                  84
#define KEY_U                  85
#define KEY_V                  86
#define KEY_W                  87
#define KEY_X                  88
#define KEY_Y                  89
#define KEY_Z                  90
#define KEY_LEFT_BRACKET       91  /* [ */
#define KEY_BACKSLASH          92  /* \ */
#define KEY_RIGHT_BRACKET      93  /* ] */
#define KEY_GRAVE_ACCENT       96  /* ` */
#define KEY_WORLD_1            161 /* non-US #1 */
#define KEY_WORLD_2            162 /* non-US #2 */
#define KEY_ESCAPE             256
#define KEY_ENTER              257
#define KEY_TAB                258
#define KEY_BACKSPACE          259
#define KEY_INSERT             260
#define KEY_DELETE             261
#define KEY_RIGHT              262
#define KEY_LEFT               263
#define KEY_DOWN               264
#define KEY_UP                 265
#define KEY_PAGE_UP            266
#define KEY_PAGE_DOWN          267
#define KEY_HOME               268
#define KEY_END                269
#define KEY_CAPS_LOCK          280
#define KEY_SCROLL_LOCK        281
#define KEY_NUM_LOCK           282
#define KEY_PRINT_SCREEN       283
#define KEY_PAUSE              284
#define KEY_F1                 290
#define KEY_F2                 291
#define KEY_F3                 292
#define KEY_F4                 293
#define KEY_F5                 294
#define KEY_F6                 295
#define KEY_F7                 296
#define KEY_F8                 297
#define KEY_F9                 298
#define KEY_F10                299
#define KEY_F11                300
#define KEY_F12                301
#define KEY_F13                302
#define KEY_F14                303
#define KEY_F15                304
#define KEY_F16                305
#define KEY_F17                306
#define KEY_F18                307
#define KEY_F19                308
#define KEY_F20                309
#define KEY_F21                310
#define KEY_F22                311
#define KEY_F23                312
#define KEY_F24                313
#define KEY_F25                314
#define KEY_KP_0               320
#define KEY_KP_1               321
#define KEY_KP_2               322
#define KEY_KP_3               323
#define KEY_KP_4               324
#define KEY_KP_5               325
#define KEY_KP_6               326
#define KEY_KP_7               327
#define KEY_KP_8               328
#define KEY_KP_9               329
#define KEY_KP_DECIMAL         330
#define KEY_KP_DIVIDE          331
#define KEY_KP_MULTIPLY        332
#define KEY_KP_SUBTRACT        333
#define KEY_KP_ADD             334
#define KEY_KP_ENTER           335
#define KEY_KP_EQUAL           336
#define KEY_LEFT_SHIFT         340
#define KEY_LEFT_CONTROL       341
#define KEY_LEFT_ALT           342
#define KEY_LEFT_SUPER         343
#define KEY_RIGHT_SHIFT        344
#define KEY_RIGHT_CONTROL      345
#define KEY_RIGHT_ALT          346
#define KEY_RIGHT_SUPER        347
#define KEY_MENU               348

#define MOUSE_BUTTON_1         0
#define MOUSE_BUTTON_2         1
#define MOUSE_BUTTON_3         2
#define MOUSE_BUTTON_4         3
#define MOUSE_BUTTON_5         4
#define MOUSE_BUTTON_6         5
#define MOUSE_BUTTON_7         6
#define MOUSE_BUTTON_8         7
#define MOUSE_BUTTON_LEFT      MOUSE_BUTTON_1
#define MOUSE_BUTTON_RIGHT     MOUSE_BUTTON_2
#define MOUSE_BUTTON_MIDDLE    MOUSE_BUTTON_3

#endif /* INPUT_HPP */