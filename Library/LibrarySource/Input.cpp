#include "input.h"
#include"Framework.h"
#include <math.h>
#include<minmax.h>


namespace Source
{
	namespace Input
	{
		bool DInput::InitDInput(HINSTANCE hInstApp, HWND hWnd)
		{
			HRESULT hr = DirectInput8Create(hInstApp, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_DInput, NULL);
			if (FAILED(hr))
				return false;

			return true;
		}

		bool DInput::InitDInputKeyboard(HWND hwnd)
		{
			HRESULT hr = S_FALSE;
			if (m_DInput == NULL)
			{
				return false;
			}

			hr = m_DInput->CreateDevice(GUID_SysKeyboard, &m_DIKeyboard, NULL);
			if (FAILED(hr))
			{
				return false;

			}

			hr = m_DIKeyboard->SetDataFormat(&c_dfDIKeyboard);
			if (FAILED(hr))
			{
				return false;
			}

			hr = m_DIKeyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
			if (FAILED(hr))
			{
				return false;
			}

			m_DIKeyboard->Acquire();

			return true;
		}

		bool DInput::InitDInputMouse(HWND hWnd)
		{
			HRESULT hr = S_FALSE;
			if (m_DInput == NULL)
			{
				return false;
			}

			hr = m_DInput->CreateDevice(GUID_SysMouse, &m_DIMouse, NULL);
			if (FAILED(hr))
			{
				return false;
			}

			hr = m_DIMouse->SetDataFormat(&c_dfDIMouse);
			if (FAILED(hr))
			{
				return false;
			}

			hr = m_DIMouse->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
			if (FAILED(hr))
			{
				return false;
			}

			//DIPROPDWORD diprop;
			//diprop.diph.dwSize = sizeof(diprop);
			//diprop.diph.dwHeaderSize = sizeof(diprop.diph);
			//diprop.diph.dwObj = 0;
			//diprop.diph.dwHow = DIPH_DEVICE;
			//diprop.dwData = DIPROPAXISMODE_REL;	// 相対値モードで設定（絶対値はDIPROPAXISMODE_ABS）

			//ret = m_DIMouse->SetProperty(DIPROP_AXISMODE, &diprop.diph);
			//if (FAILED(ret)) 
			//{
			//	return false;
			//}

			m_DIMouse->Acquire();

			return true;
		}

		bool DInput::ReleaseDInput()
		{
			if (m_DInput)
			{
				m_DInput->Release();
				m_DInput = NULL;
			}
			return true;
		}

		bool DInput::ReleaseDInputKeyboard()
		{
			if (m_DIKeyboard)
			{
				m_DIKeyboard->Release();
				m_DIKeyboard = NULL;
			}
			return true;
		}

		bool DInput::ReleaseDInputMouse()
		{
			if (m_DIMouse) 
			{
				m_DIMouse->Release();
				m_DIMouse = NULL;
			}

			return true;
		}

		void DInput::GetHitKeyStateAll(BYTE* key)
		{
			if (!m_DIKeyboard)return;
			HRESULT hr;
			ZeroMemory(key, sizeof(key));
			hr = m_DIKeyboard->GetDeviceState(sizeof(BYTE) * 256, key);
			if (FAILED(hr))
			{
				m_DIKeyboard->Acquire();
				m_DIKeyboard->GetDeviceState(sizeof(BYTE) * 256, key);
			}
		}

		void DInput::GetMouseState(DIMOUSESTATE& m_zdiMouseState)
		{
			if (m_DIMouse == NULL) return;

			DIMOUSESTATE g_zdiMouseState_bak;
			memcpy(&g_zdiMouseState_bak, &m_zdiMouseState, sizeof(g_zdiMouseState_bak));

			HRESULT	hr = m_DIMouse->GetDeviceState(sizeof(DIMOUSESTATE), &m_zdiMouseState);
			if (FAILED(hr))
			{
				m_DIMouse->Acquire();
				hr = m_DIMouse->GetDeviceState(sizeof(DIMOUSESTATE), &m_zdiMouseState);
			}

		}

		void KeyBoard::GetKeyStateAll()
		{
			static BYTE buf[256];

			DINPUT.GetHitKeyStateAll(buf);
			for (int i = 0; i < 256; i++)
			{
				if (buf[i])
				{
					if (_keys[i] == 0)
					{
						_keys[i] = 1;
					}
					else if (_keys[i] >= 1)
					{
						_keys[i]++;
					}
				}
				else
				{
					if (_keys[i] > 0)
					{
						_keys[i] = -1;
					}
					else
					{
						_keys[i] = 0;
					}
				}
			}
		}

		void Mouse::MouseState(HWND hwnd)
		{
			DINPUT.GetMouseState(m_zdiMouseState);
			GetCursorPos(&m_position);
			ScreenToClient(hwnd, &m_position);

			m_position.x = static_cast<LONG>(max(min(m_position.x, Framework::GetInstance().SCREEN_WIDTH), (LONG)0));
			m_position.y = static_cast<LONG>(max(min(m_position.y, Framework::GetInstance().SCREEN_HEIGHT), (LONG)0));

			for (int i = 0; i < 3; ++i)
			{
				m_previousState[i] = m_currentState[i];

				if ((static_cast<unsigned short>(m_zdiMouseState.rgbButtons[i] & 0x80)))
				{
					++m_currentState[i];
				}
				else
				{
					m_currentState[i] = 0;
				}
			}
		}

		bool Mouse::PressedState(MouseLabel button)
		{
			switch (button)
			{
			case MouseLabel::LEFT_BUTTON:
				return m_currentState[0] > 0;
				break;
			case MouseLabel::RIGHT_BUTTON:
				return m_currentState[1] > 0;
				break;
			case MouseLabel::MID_BUTTON:
				return m_currentState[2] > 0;
				break;
			default:
				break;
			}
			return false;
		}

		bool Mouse::RisingState(MouseLabel button)
		{
			switch (button)
			{
			case MouseLabel::LEFT_BUTTON:
				return m_currentState[0] > 0 && m_previousState[0] == 0;
				break;
			case MouseLabel::RIGHT_BUTTON:
				return m_currentState[1] > 0 && m_previousState[1] == 0;
				break;
			case MouseLabel::MID_BUTTON:
				return m_currentState[2] > 0 && m_previousState[2] == 0;
				break;
			default:
				break;
			}

			return false;
		}

		bool Mouse::FallingState(MouseLabel button)
		{
			switch (button)
			{
			case MouseLabel::LEFT_BUTTON:
				return m_currentState[0] == 0 && m_previousState[0] > 0;
				break;
			case MouseLabel::RIGHT_BUTTON:
				return m_currentState[1] == 0 && m_previousState[1] > 0;
				break;
			case MouseLabel::MID_BUTTON:
				return m_currentState[2] == 0 && m_previousState[2] > 0;
				break;
			default:
				break;
			}

			return false;
		}

		void XInput::XInputState()
		{
			XInputGetState(0, &m_xPad);
			XpadButtonState();
		}

		void XInput::XpadButtonState()
		{

			if (m_xPad.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP)
			{
				if (_buttons[PAD_UP] == 0)
				{
					_buttons[PAD_UP] = 1;
				}
				else if (_buttons[PAD_UP] >= 1)
				{
					_buttons[PAD_UP]++;
				}
			}
			else
			{
				if (_buttons[PAD_UP] > 0)
				{
					_buttons[PAD_UP] = -1;
				}
				else
				{
					_buttons[PAD_UP] = 0;
				}
			}

			if (m_xPad.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN)
			{
				if (_buttons[PAD_DOWN] == 0)
				{
					_buttons[PAD_DOWN] = 1;
				}
				else if (_buttons[PAD_DOWN] >= 1)
				{
					_buttons[PAD_DOWN]++;
				}
			}
			else
			{
				if (_buttons[PAD_DOWN] > 0)
				{
					_buttons[PAD_DOWN] = -1;
				}
				else
				{
					_buttons[PAD_DOWN] = 0;
				}
			}

			if (m_xPad.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT)
			{
				if (_buttons[PAD_LEFT] == 0)
				{
					_buttons[PAD_LEFT] = 1;
				}
				else if (_buttons[PAD_LEFT] >= 1)
				{
					_buttons[PAD_LEFT]++;
				}
			}
			else
			{
				if (_buttons[PAD_LEFT] > 0)
				{
					_buttons[PAD_LEFT] = -1;
				}
				else
				{
					_buttons[PAD_LEFT] = 0;
				}
			}

			if (m_xPad.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)
			{
				if (_buttons[PAD_RIGHT] == 0)
				{
					_buttons[PAD_RIGHT] = 1;
				}
				else if (_buttons[PAD_RIGHT] >= 1)
				{
					_buttons[PAD_RIGHT]++;
				}
			}
			else
			{
				if (_buttons[PAD_RIGHT] > 0)
				{
					_buttons[PAD_RIGHT] = -1;
				}
				else
				{
					_buttons[PAD_RIGHT] = 0;
				}
			}

			if (m_xPad.Gamepad.wButtons & XINPUT_GAMEPAD_START)
			{
				if (_buttons[PAD_START] == 0)
				{
					_buttons[PAD_START] = 1;
				}
				else if (_buttons[PAD_START] >= 1)
				{
					_buttons[PAD_START]++;
				}
			}
			else
			{
				if (_buttons[PAD_START] > 0)
				{
					_buttons[PAD_START] = -1;
				}
				else
				{
					_buttons[PAD_START] = 0;
				}
			}

			if (m_xPad.Gamepad.wButtons & XINPUT_GAMEPAD_BACK)
			{
				if (_buttons[PAD_BACK] == 0)
				{
					_buttons[PAD_BACK] = 1;
				}
				else if (_buttons[PAD_BACK] >= 1)
				{
					_buttons[PAD_BACK]++;
				}
			}
			else
			{
				if (_buttons[PAD_BACK] > 0)
				{
					_buttons[PAD_BACK] = -1;
				}
				else
				{
					_buttons[PAD_BACK] = 0;
				}
			}

			if (m_xPad.Gamepad.bLeftTrigger & XINPUT_GAMEPAD_LEFT_THUMB)
			{
				if (_buttons[PAD_LTHUMB] == 0)
				{
					_buttons[PAD_LTHUMB] = 1;
				}
				else if (_buttons[PAD_LTHUMB] >= 1)
				{
					_buttons[PAD_LTHUMB]++;
				}
			}
			else
			{
				if (_buttons[PAD_LTHUMB] > 0)
				{
					_buttons[PAD_LTHUMB] = -1;
				}
				else
				{
					_buttons[PAD_LTHUMB] = 0;
				}
			}


			if (m_xPad.Gamepad.bRightTrigger & XINPUT_GAMEPAD_RIGHT_THUMB)
			{
				if (_buttons[PAD_RTHUMB] == 0)
				{
					_buttons[PAD_RTHUMB] = 1;
				}
				else if (_buttons[PAD_RTHUMB] >= 1)
				{
					_buttons[PAD_RTHUMB]++;
				}
			}
			else
			{
				if (_buttons[PAD_RTHUMB] > 0)
				{
					_buttons[PAD_RTHUMB] = -1;
				}
				else
				{
					_buttons[PAD_RTHUMB] = 0;
				}
			}

			if (m_xPad.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER)
			{
				if (_buttons[PAD_LSHOULDER] == 0)
				{
					_buttons[PAD_LSHOULDER] = 1;
				}
				else if (_buttons[PAD_LSHOULDER] >= 1)
				{
					_buttons[PAD_LSHOULDER]++;
				}
			}
			else
			{
				if (_buttons[PAD_LSHOULDER] > 0)
				{
					_buttons[PAD_LSHOULDER] = -1;
				}
				else
				{
					_buttons[PAD_LSHOULDER] = 0;
				}
			}


			if (m_xPad.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER)
			{
				if (_buttons[PAD_RSHOULDER] == 0)
				{
					_buttons[PAD_RSHOULDER] = 1;
				}
				else if (_buttons[PAD_RSHOULDER] >= 1)
				{
					_buttons[PAD_RSHOULDER]++;
				}
			}
			else
			{
				if (_buttons[PAD_RSHOULDER] > 0)
				{
					_buttons[PAD_RSHOULDER] = -1;
				}
				else
				{
					_buttons[PAD_RSHOULDER] = 0;
				}
			}


			if (m_xPad.Gamepad.wButtons & XINPUT_GAMEPAD_A)
			{
				if (_buttons[PAD_A] == 0)
				{
					_buttons[PAD_A] = 1;
				}
				else if (_buttons[PAD_A] >= 1)
				{
					_buttons[PAD_A]++;
				}
			}
			else
			{
				if (_buttons[PAD_A] > 0)
				{
					_buttons[PAD_A] = -1;
				}
				else
				{
					_buttons[PAD_A] = 0;
				}
			}


			if (m_xPad.Gamepad.wButtons & XINPUT_GAMEPAD_B)
			{
				if (_buttons[PAD_B] == 0)
				{
					_buttons[PAD_B] = 1;
				}
				else if (_buttons[PAD_B] >= 1)
				{
					_buttons[PAD_B]++;
				}
			}
			else
			{
				if (_buttons[PAD_B] > 0)
				{
					_buttons[PAD_B] = -1;
				}
				else
				{
					_buttons[PAD_B] = 0;
				}
			}


			if (m_xPad.Gamepad.wButtons & XINPUT_GAMEPAD_X)
			{
				if (_buttons[PAD_X] == 0)
				{
					_buttons[PAD_X] = 1;
				}
				else if (_buttons[PAD_X] >= 1)
				{
					_buttons[PAD_X]++;
				}
			}
			else
			{
				if (_buttons[PAD_X] > 0)
				{
					_buttons[PAD_X] = -1;
				}
				else
				{
					_buttons[PAD_X] = 0;
				}
			}


			if (m_xPad.Gamepad.wButtons & XINPUT_GAMEPAD_Y)
			{
				if (_buttons[PAD_Y] == 0)
				{
					_buttons[PAD_Y] = 1;
				}
				else if (_buttons[PAD_Y] >= 1)
				{
					_buttons[PAD_Y]++;
				}
			}
			else
			{
				if (_buttons[PAD_Y] > 0)
				{
					_buttons[PAD_Y] = -1;
				}
				else
				{
					_buttons[PAD_Y] = 0;
				}
			}

		}



		VECTOR2F XInput::StickVectorL()
		{
			VECTOR2F vec2 = {};

			vec2.x = static_cast<float>(m_xPad.Gamepad.sThumbLX) / 32767;
			vec2.y = static_cast<float>(m_xPad.Gamepad.sThumbLY) / 32767;

			float len = sqrtf(vec2.x * vec2.x + vec2.y * vec2.y);
			float mag = 1 / len;
			vec2.x *= mag;
			vec2.y *= mag;

			return vec2;
		}

		VECTOR2F XInput::StickVectorR()
		{
			VECTOR2F vec2 = {};

			vec2.x = static_cast<float>(m_xPad.Gamepad.sThumbRX) / 32767;
			vec2.y = static_cast<float>(m_xPad.Gamepad.sThumbRY) / 32767;

			float len = sqrtf(vec2.x * vec2.x + vec2.y * vec2.y);
			float mag = 1 / len;
			vec2.x *= mag;
			vec2.y *= mag;

			return vec2;
		}

		float XInput::StickAngleL()
		{
			float angle;
			angle = atan2f(StickVectorL().x, StickVectorL().y);

			return angle;
		}

		float XInput::StickAngleR()
		{
			float angle;
			angle = atan2f(StickVectorR().x, StickVectorR().y);

			return angle;
		}
	}
}