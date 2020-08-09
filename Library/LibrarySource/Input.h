#pragma once

//-------------------------------------------------------------------------------------------
//								DirectInput
//-------------------------------------------------------------------------------------------
#define DIRECTINPUT_VERSION		0x0800		// DirectInputÇÃÉoÅ[ÉWÉáÉìèÓïÒ

#include <Windows.h>
#include <dinput.h>
#include <Xinput.h>
#include <DirectXMath.h>
#include "Vector.h"

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment (lib, "xinput.lib")
#pragma comment(lib, "xinput9_1_0.lib")

enum XINPUT_GAMEPAD_BUTTONS
{
	PAD_UP = 0,
	PAD_DOWN,
	PAD_LEFT,
	PAD_RIGHT,
	PAD_START,
	PAD_BACK,
	PAD_LTHUMB,
	PAD_RTHUMB,
	PAD_LSHOULDER,
	PAD_RSHOULDER,
	PAD_A,
	PAD_B,
	PAD_X,
	PAD_Y,
	GAMEPAD_MAX_BUTTON,
};


enum class MouseLabel
{
	LEFT_BUTTON = VK_LBUTTON,
	RIGHT_BUTTON = VK_RBUTTON,
	MID_BUTTON = VK_MBUTTON,
};


namespace Source
{
	namespace Input
	{
		class DInput
		{
		private:
			LPDIRECTINPUT8 m_DInput = NULL;
			LPDIRECTINPUTDEVICE8 m_DIKeyboard = NULL;
			LPDIRECTINPUTDEVICE8 m_DIMouse = NULL;
			BOOL m_AppActive = FALSE;
		public:
			DInput() = default;
			~DInput() = default;
			bool InitDInput(HINSTANCE hInstApp, HWND hWnd);
			bool InitDInputKeyboard(HWND hwnd);
			bool InitDInputMouse(HWND hWnd);

			void GetHitKeyStateAll(BYTE* _key);
			void GetMouseState(DIMOUSESTATE& m_zdiMouseState/*HWND hWnd*/);

			bool ReleaseDInputKeyboard();
			bool ReleaseDInputMouse();
			bool ReleaseDInput();

			inline static DInput& GetInstance()
			{
				static DInput dInput;
				return dInput;
			}
		};

		class KeyBoard
		{
		public:
			int _keys[256] = {};
		public:
			KeyBoard() = default;
			~KeyBoard() = default;

			void GetKeyStateAll();

			inline static KeyBoard& GetInstance()
			{
				static KeyBoard keyBoard;
				return keyBoard;
			}
		};

		class Mouse
		{
		private:
			POINT m_position;
			DIMOUSESTATE m_zdiMouseState;
			int m_currentState[3];
			int m_previousState[3];
		public:
			Mouse() = default;
			~Mouse() = default;

			void MouseState(HWND hwnd);
			bool PressedState(MouseLabel button);
			bool RisingState(MouseLabel button);
			bool FallingState(MouseLabel button);
			float GetCursorPosX() { return static_cast<float>(m_position.x); }
			float GetCursorPosY() { return static_cast<float>(m_position.y); }

			inline static Mouse& GetInstance()
			{
				static Mouse mouse;
				return mouse;
			}
		};


		class XInput
		{
		private:
			XINPUT_STATE m_xPad = {};
		public:
			int _buttons[GAMEPAD_MAX_BUTTON] = {};

			XInput() = default;
			~XInput() = default;

			void XInputState();
			void XpadButtonState();

			VECTOR2F StickVectorL();
			VECTOR2F StickVectorR();
			float StickAngleL();
			float StickAngleR();

			inline bool StickDeadzoneLX(int deadZone)
			{
				return m_xPad.Gamepad.sThumbLX > deadZone || m_xPad.Gamepad.sThumbLX < -deadZone ? true : false;
			}

			inline bool StickDeadzoneLY(int deadZone)
			{
				return m_xPad.Gamepad.sThumbLY > deadZone || m_xPad.Gamepad.sThumbLY < -deadZone ? true : false;
			}

			inline bool StickDeadzoneRX(int deadZone)
			{
				return m_xPad.Gamepad.sThumbRX > deadZone || m_xPad.Gamepad.sThumbRX < -deadZone ? true : false;
			}

			inline bool StickDeadzoneRY(int deadZone)
			{
				return m_xPad.Gamepad.sThumbRY > deadZone || m_xPad.Gamepad.sThumbRY < -deadZone ? true : false;
			}

			inline const float GetStickLxValue() const { return static_cast<float>(m_xPad.Gamepad.sThumbLX); }
			inline const float GetStickLyValue() const { return static_cast<float>(m_xPad.Gamepad.sThumbLY); }
			inline const float GetStickRxValue() const { return static_cast<float>(m_xPad.Gamepad.sThumbRX); }
			inline const float GetStickRyValue() const { return static_cast<float>(m_xPad.Gamepad.sThumbRX); }
		
			inline static XInput& GetInstance()
			{
				static XInput xInput;
				return xInput;
			}
		};
	}
}

#define DINPUT	 Source::Input::DInput::GetInstance()
#define KEYBOARD Source::Input::KeyBoard::GetInstance()
#define XINPUT	 Source::Input::XInput::GetInstance()
#define MOUSE	 Source::Input::Mouse::GetInstance()

