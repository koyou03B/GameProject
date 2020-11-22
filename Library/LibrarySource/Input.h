#pragma once

//-------------------------------------------------------------------------------------------
//								DirectInput
//-------------------------------------------------------------------------------------------
#define DIRECTINPUT_VERSION		0x0800		// DirectInputÇÃÉoÅ[ÉWÉáÉìèÓïÒ

#include <Windows.h>
#include <dinput.h>
#include <Xinput.h>
#include <DirectXMath.h>
#include <memory>
#include "Vector.h"

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment (lib, "xinput.lib")
#pragma comment(lib, "xinput9_1_0.lib")
#pragma comment(lib,"winmm.lib")

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
		public:
			DInput() = default;
			~DInput() = default;
			bool InitDInput(HINSTANCE hInstApp, HWND hWnd);

			bool ReleaseDInput();
		protected:
			LPDIRECTINPUT8 m_DInput = NULL;
			BOOL m_AppActive = FALSE;

		};

		class KeyBoard : public DInput
		{
		public:
			int _keys[256] = {};
		public:
			KeyBoard() = default;
			~KeyBoard() = default;

			bool InitDInputKeyboard(HWND hwnd);
			void GetHitKeyStateAll(BYTE* _key);
			bool ReleaseDInputKeyboard();

			void GetKeyStateAll();

			inline static KeyBoard& GetInstance()
			{
				static KeyBoard keyBoard;
				return keyBoard;
			}
		private:
			LPDIRECTINPUTDEVICE8 m_DIKeyboard = NULL;

		};

		class Mouse : public DInput
		{
		public:
			Mouse() = default;
			~Mouse() = default;

			bool InitDInputMouse(HWND hWnd);
			void GetMouseState(DIMOUSESTATE& m_zdiMouseState/*HWND hWnd*/);
			bool ReleaseDInputMouse();

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
		private:
			LPDIRECTINPUTDEVICE8 m_DIMouse = NULL;
			POINT m_position = {};
			DIMOUSESTATE m_zdiMouseState = {};
			int m_currentState[3] = {};
			int m_previousState[3] = {};
		};


		class Input
		{
		public:
			Input() = default;
			~Input() = default;

			virtual void InputState() {};
			virtual void PadButtonState() {};
			virtual bool CheckConnect() { return false; };

			virtual VECTOR2F StickVectorLeft()  = 0;
			virtual VECTOR2F StickVectorRight() = 0;

			virtual bool StickDeadzoneLX(float deadZone) = 0;
			virtual bool StickDeadzoneLY(float deadZone) = 0;
			virtual bool StickDeadzoneRX(float deadZone) = 0;
			virtual bool StickDeadzoneRY(float deadZone) = 0;

			inline float& GetStickLeftXValue() { return m_stickLeftXValue; }
			inline float& GetStickLeftYValue() { return m_stickLeftYValue; }

			int GetButtons(XINPUT_GAMEPAD_BUTTONS command) { return m_buttons[command]; }
			void ResetButton(XINPUT_GAMEPAD_BUTTONS command) { m_buttons[command] = -1; }
			bool GetButtons()
			{
				for (int i = 0; i < GAMEPAD_MAX_BUTTON; ++i)
				{
					if (m_buttons[i] == 1)
						return true;
				}

				return false;
			}

		protected:
			int m_buttons[GAMEPAD_MAX_BUTTON] = {};
			int m_userIndex = -1;
			float m_stickLeftXValue;
			float m_stickLeftYValue;
			VECTOR2F m_vibration;
		};

		class XInput : public Input
		{
		public:
			XInput() = default;
			~XInput() = default;

			void InputState()override;
			void PadButtonState()override;
			bool CheckConnect() override;
		
			void SetUserIndex(const int index) { m_userIndex = index; };

			bool StickDeadzoneLX(float deadZone) override
			{
				return m_xPad.Gamepad.sThumbLX > deadZone || m_xPad.Gamepad.sThumbLX < -deadZone ? true : false;
			}
			bool StickDeadzoneLY(float deadZone) override
			{
				return m_xPad.Gamepad.sThumbLY > deadZone || m_xPad.Gamepad.sThumbLY < -deadZone ? true : false;
			}
			bool StickDeadzoneRX(float deadZone) override
			{
				return m_xPad.Gamepad.sThumbRX > deadZone || m_xPad.Gamepad.sThumbRX < -deadZone ? true : false;
			}
			bool StickDeadzoneRY(float deadZone) override
			{
				return m_xPad.Gamepad.sThumbRY > deadZone || m_xPad.Gamepad.sThumbRY < -deadZone ? true : false;
			}

			VECTOR2F StickVectorLeft() override;
			VECTOR2F StickVectorRight() override;

		private:
			XINPUT_STATE m_xPad = {};
			XINPUT_VIBRATION m_vibration;
		};

		class WInput : public Input
		{
		public:
			WInput() = default;
			~WInput() = default;

			void InputState()override;
			void PadButtonState()override;
			bool CheckConnect() override;

			void SetUserIndex(const int index) { m_userIndex = index; };

			bool StickDeadzoneLX(float deadZone) override
			{
				return static_cast<float>((static_cast<int>(m_joyPad.dwXpos) - 0x7FFF)) > deadZone || 
					static_cast<float>((static_cast<int>(m_joyPad.dwXpos) - 0x7FFF)) < -deadZone ? true : false;
			}
			bool StickDeadzoneLY(float deadZone) override
			{
				return static_cast<float>((static_cast<int>(m_joyPad.dwYpos) - 0x7FFF)) > deadZone || 
					static_cast<float>((static_cast<int>(m_joyPad.dwYpos) - 0x7FFF)) < -deadZone ? true : false;
			}
			bool StickDeadzoneRX(float deadZone) override
			{
				return static_cast<float>((static_cast<int>(m_joyPad.dwZpos) - 0x7FFF)) > deadZone ||
					static_cast<float>((static_cast<int>(m_joyPad.dwZpos) - 0x7FFF)) < -deadZone ? true : false;
			}
			bool StickDeadzoneRY(float deadZone) override
			{
				return static_cast<float>((static_cast<int>(m_joyPad.dwRpos) - 0x7FFF)) > deadZone || 
					static_cast<float>((static_cast<int>(m_joyPad.dwRpos) - 0x7FFF)) < -deadZone ? true : false;
			}

			VECTOR2F StickVectorLeft() override;
			VECTOR2F StickVectorRight() override;

		private:
			JOYINFOEX m_joyPad = {};
			int angle = 0;
 		};

		class Pads
		{
		public:
			Pads() = default;
			~Pads() = default;

			//xÇ©dÇ©Çîªï ÇµÇƒÇ§Ç≤Ç©Ç∑
			void SetUpPads();
			void Update();

			Input* GetPad(const int index) { return &(*m_pad[index]); };

			inline static Pads& GetInstance()
			{
				static Pads pads;
				return pads;
			};

		private:
			std::shared_ptr<Input> m_pad[2];
		};
	}
}

#define KEYBOARD Source::Input::KeyBoard::GetInstance()
#define MOUSE	 Source::Input::Mouse::GetInstance()
#define PAD     Source::Input::Pads::GetInstance()