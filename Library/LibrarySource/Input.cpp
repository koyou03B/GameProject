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

		bool DInput::ReleaseDInput()
		{
			if (m_DInput)
			{
				m_DInput->Release();
				m_DInput = NULL;
			}
			return true;
		}



		bool KeyBoard::InitDInputKeyboard(HWND hwnd)
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

		void KeyBoard::GetHitKeyStateAll(BYTE* key)
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

		void KeyBoard::GetKeyStateAll()
		{
			static BYTE buf[256];

			GetHitKeyStateAll(buf);
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

		bool KeyBoard::ReleaseDInputKeyboard()
		{
			if (m_DIKeyboard)
			{
				m_DIKeyboard->Release();
				m_DIKeyboard = NULL;
			}
			return true;
		}



		bool Mouse::InitDInputMouse(HWND hWnd)
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

		void Mouse::GetMouseState(DIMOUSESTATE& m_zdiMouseState)
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

		void Mouse::MouseState(HWND hwnd)
		{
			GetMouseState(m_zdiMouseState);
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

		bool Mouse::ReleaseDInputMouse()
		{
			if (m_DIMouse)
			{
				m_DIMouse->Release();
				m_DIMouse = NULL;
			}

			return true;
		}



		void XInput::InputState()
		{
			if (CheckConnect())
			{
				PadButtonState();

				m_stickLeftXValue =  m_xPad.Gamepad.sThumbLX;
														   
				m_stickLeftYValue =  m_xPad.Gamepad.sThumbLY;

			}
		}

		void XInput::PadButtonState()
		{

			if (m_xPad.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP)
			{
				if (m_buttons[PAD_UP] == 0)
				{
					m_buttons[PAD_UP] = 1;
				}
				else if (m_buttons[PAD_UP] >= 1)
				{
					m_buttons[PAD_UP]++;
				}
			}
			else
			{
				if (m_buttons[PAD_UP] > 0)
				{
					m_buttons[PAD_UP] = -1;
				}
				else
				{
					m_buttons[PAD_UP] = 0;
				}
			}

			if (m_xPad.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN)
			{
				if (m_buttons[PAD_DOWN] == 0)
				{
					m_buttons[PAD_DOWN] = 1;
				}
				else if (m_buttons[PAD_DOWN] >= 1)
				{
					m_buttons[PAD_DOWN]++;
				}
			}
			else
			{
				if (m_buttons[PAD_DOWN] > 0)
				{
					m_buttons[PAD_DOWN] = -1;
				}
				else
				{
					m_buttons[PAD_DOWN] = 0;
				}
			}

			if (m_xPad.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT)
			{
				if (m_buttons[PAD_LEFT] == 0)
				{
					m_buttons[PAD_LEFT] = 1;
				}
				else if (m_buttons[PAD_LEFT] >= 1)
				{
					m_buttons[PAD_LEFT]++;
				}
			}
			else
			{
				if (m_buttons[PAD_LEFT] > 0)
				{
					m_buttons[PAD_LEFT] = -1;
				}
				else
				{
					m_buttons[PAD_LEFT] = 0;
				}
			}

			if (m_xPad.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)
			{
				if (m_buttons[PAD_RIGHT] == 0)
				{
					m_buttons[PAD_RIGHT] = 1;
				}
				else if (m_buttons[PAD_RIGHT] >= 1)
				{
					m_buttons[PAD_RIGHT]++;
				}
			}
			else
			{
				if (m_buttons[PAD_RIGHT] > 0)
				{
					m_buttons[PAD_RIGHT] = -1;
				}
				else
				{
					m_buttons[PAD_RIGHT] = 0;
				}
			}

			if (m_xPad.Gamepad.wButtons & XINPUT_GAMEPAD_START)
			{
				if (m_buttons[PAD_START] == 0)
				{
					m_buttons[PAD_START] = 1;
				}
				else if (m_buttons[PAD_START] >= 1)
				{
					m_buttons[PAD_START]++;
				}
			}
			else
			{
				if (m_buttons[PAD_START] > 0)
				{
					m_buttons[PAD_START] = -1;
				}
				else
				{
					m_buttons[PAD_START] = 0;
				}
			}

			if (m_xPad.Gamepad.wButtons & XINPUT_GAMEPAD_BACK)
			{
				if (m_buttons[PAD_BACK] == 0)
				{
					m_buttons[PAD_BACK] = 1;
				}
				else if (m_buttons[PAD_BACK] >= 1)
				{
					m_buttons[PAD_BACK]++;
				}
			}
			else
			{
				if (m_buttons[PAD_BACK] > 0)
				{
					m_buttons[PAD_BACK] = -1;
				}
				else
				{
					m_buttons[PAD_BACK] = 0;
				}
			}

			if (m_xPad.Gamepad.bLeftTrigger & XINPUT_GAMEPAD_LEFT_THUMB)
			{
				if (m_buttons[PAD_LTHUMB] == 0)
				{
					m_buttons[PAD_LTHUMB] = 1;
				}
				else if (m_buttons[PAD_LTHUMB] >= 1)
				{
					m_buttons[PAD_LTHUMB]++;
				}
			}
			else
			{
				if (m_buttons[PAD_LTHUMB] > 0)
				{
					m_buttons[PAD_LTHUMB] = -1;
				}
				else
				{
					m_buttons[PAD_LTHUMB] = 0;
				}
			}


			if (m_xPad.Gamepad.bRightTrigger & XINPUT_GAMEPAD_RIGHT_THUMB)
			{
				if (m_buttons[PAD_RTHUMB] == 0)
				{
					m_buttons[PAD_RTHUMB] = 1;
				}
				else if (m_buttons[PAD_RTHUMB] >= 1)
				{
					m_buttons[PAD_RTHUMB]++;
				}
			}
			else
			{
				if (m_buttons[PAD_RTHUMB] > 0)
				{
					m_buttons[PAD_RTHUMB] = -1;
				}
				else
				{
					m_buttons[PAD_RTHUMB] = 0;
				}
			}

			if (m_xPad.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER)
			{
				if (m_buttons[PAD_LSHOULDER] == 0)
				{
					m_buttons[PAD_LSHOULDER] = 1;
				}
				else if (m_buttons[PAD_LSHOULDER] >= 1)
				{
					m_buttons[PAD_LSHOULDER]++;
				}
			}
			else
			{
				if (m_buttons[PAD_LSHOULDER] > 0)
				{
					m_buttons[PAD_LSHOULDER] = -1;
				}
				else
				{
					m_buttons[PAD_LSHOULDER] = 0;
				}
			}


			if (m_xPad.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER)
			{
				if (m_buttons[PAD_RSHOULDER] == 0)
				{
					m_buttons[PAD_RSHOULDER] = 1;
				}
				else if (m_buttons[PAD_RSHOULDER] >= 1)
				{
					m_buttons[PAD_RSHOULDER]++;
				}
			}
			else
			{
				if (m_buttons[PAD_RSHOULDER] > 0)
				{
					m_buttons[PAD_RSHOULDER] = -1;
				}
				else
				{
					m_buttons[PAD_RSHOULDER] = 0;
				}
			}


			if (m_xPad.Gamepad.wButtons & XINPUT_GAMEPAD_A)
			{
				if (m_buttons[PAD_A] == 0)
				{
					m_buttons[PAD_A] = 1;
				}
				else if (m_buttons[PAD_A] >= 1)
				{
					m_buttons[PAD_A]++;
				}
			}
			else
			{
				if (m_buttons[PAD_A] > 0)
				{
					m_buttons[PAD_A] = -1;
				}
				else
				{
					m_buttons[PAD_A] = 0;
				}
			}


			if (m_xPad.Gamepad.wButtons & XINPUT_GAMEPAD_B)
			{
				if (m_buttons[PAD_B] == 0)
				{
					m_buttons[PAD_B] = 1;
				}
				else if (m_buttons[PAD_B] >= 1)
				{
					m_buttons[PAD_B]++;
				}
			}
			else
			{
				if (m_buttons[PAD_B] > 0)
				{
					m_buttons[PAD_B] = -1;
				}
				else
				{
					m_buttons[PAD_B] = 0;
				}
			}


			if (m_xPad.Gamepad.wButtons & XINPUT_GAMEPAD_X)
			{
				if (m_buttons[PAD_X] == 0)
				{
					m_buttons[PAD_X] = 1;
				}
				else if (m_buttons[PAD_X] >= 1)
				{
					m_buttons[PAD_X]++;
				}
			}
			else
			{
				if (m_buttons[PAD_X] > 0)
				{
					m_buttons[PAD_X] = -1;
				}
				else
				{
					m_buttons[PAD_X] = 0;
				}
			}


			if (m_xPad.Gamepad.wButtons & XINPUT_GAMEPAD_Y)
			{
				if (m_buttons[PAD_Y] == 0)
				{
					m_buttons[PAD_Y] = 1;
				}
				else if (m_buttons[PAD_Y] >= 1)
				{
					m_buttons[PAD_Y]++;
				}
			}
			else
			{
				if (m_buttons[PAD_Y] > 0)
				{
					m_buttons[PAD_Y] = -1;
				}
				else
				{
					m_buttons[PAD_Y] = 0;
				}
			}

		}

		VECTOR2F XInput::StickVectorLeft()
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

		VECTOR2F XInput::StickVectorRight()
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

		bool XInput::CheckConnect()
		{
			DWORD dwResult;
			dwResult = XInputGetState(m_userIndex, &m_xPad);
			if (!(dwResult == ERROR_SUCCESS))return false;

			return true;
		}



		void WInput::InputState()
		{
			if (CheckConnect())
			{
				PadButtonState();
			}
		}

		void WInput::PadButtonState()
		{
			////LRスティック押し込み
			//if (m_joyPad.dwButtons & JOY_BUTTON11)m_buttonNew |= BTN_6;
			//if (m_joyPad.dwButtons & JOY_BUTTON12)m_buttonNew |= BTN_7;

			static const int CrossKey[4] =
			{
				PAD_UP,			    	//上
				PAD_RIGHT,				//右
				PAD_DOWN,				//下
				PAD_LEFT,				//左
			};
			//十字キー
			if (m_joyPad.dwPOV != 0xFFFF)
			{
				 angle = m_joyPad.dwPOV / 9000;

				if (m_buttons[CrossKey[angle]] == 0)
				{
					m_buttons[CrossKey[angle]] = 1;
				}
				else if (m_buttons[CrossKey[angle]] >= 1)
				{
					m_buttons[CrossKey[angle]]++;
				}
			}
			else
			{
				for (int i = 0; i < 4; ++i)
				{
					if (m_buttons[CrossKey[i]] > 0)
					{
						m_buttons[CrossKey[i]] = -1;
					}
					else
					{
						m_buttons[CrossKey[i]] = 0;
					}
				}

			}

			if (m_joyPad.dwButtons & JOY_BUTTON1)
			{
				if (m_buttons[PAD_X] == 0)
				{
					m_buttons[PAD_X] = 1;
				}
				else if (m_buttons[PAD_X] >= 1)
				{
					m_buttons[PAD_X]++;
				}
			}
			else
			{
				if (m_buttons[PAD_X] > 0)
				{
					m_buttons[PAD_X] = -1;
				}
				else
				{
					m_buttons[PAD_X] = 0;
				}
			}

			if (m_joyPad.dwButtons & JOY_BUTTON2)
			{
				if (m_buttons[PAD_A] == 0)
				{
					m_buttons[PAD_A] = 1;
				}
				else if (m_buttons[PAD_A] >= 1)
				{
					m_buttons[PAD_A]++;
				}
			}
			else
			{
				if (m_buttons[PAD_A] > 0)
				{
					m_buttons[PAD_A] = -1;
				}
				else
				{
					m_buttons[PAD_A] = 0;
				}
			}

			if (m_joyPad.dwButtons & JOY_BUTTON3)
			{
				if (m_buttons[PAD_B] == 0)
				{
					m_buttons[PAD_B] = 1;
				}
				else if (m_buttons[PAD_B] >= 1)
				{
					m_buttons[PAD_B]++;
				}
			}
			else
			{
				if (m_buttons[PAD_B] > 0)
				{
					m_buttons[PAD_B] = -1;
				}
				else
				{
					m_buttons[PAD_B] = 0;
				}
			}

			if (m_joyPad.dwButtons & JOY_BUTTON4)
			{
				if (m_buttons[PAD_Y] == 0)
				{
					m_buttons[PAD_Y] = 1;
				}
				else if (m_buttons[PAD_Y] >= 1)
				{
					m_buttons[PAD_Y]++;
				}
			}
			else
			{
				if (m_buttons[PAD_Y] > 0)
				{
					m_buttons[PAD_Y] = -1;
				}
				else
				{
					m_buttons[PAD_Y] = 0;
				}
			}

			if (m_joyPad.dwButtons & JOY_BUTTON5)
			{
				if (m_buttons[PAD_LSHOULDER] == 0)
				{
					m_buttons[PAD_LSHOULDER] = 1;
				}
				else if (m_buttons[PAD_LSHOULDER] >= 1)
				{
					m_buttons[PAD_LSHOULDER]++;
				}
			}
			else
			{
				if (m_buttons[PAD_LSHOULDER] > 0)
				{
					m_buttons[PAD_LSHOULDER] = -1;
				}
				else
				{
					m_buttons[PAD_LSHOULDER] = 0;
				}
			}

			if (m_joyPad.dwButtons & JOY_BUTTON6)
			{
				if (m_buttons[PAD_RSHOULDER] == 0)
				{
					m_buttons[PAD_RSHOULDER] = 1;
				}
				else if (m_buttons[PAD_RSHOULDER] >= 1)
				{
					m_buttons[PAD_RSHOULDER]++;
				}
			}
			else
			{
				if (m_buttons[PAD_RSHOULDER] > 0)
				{
					m_buttons[PAD_RSHOULDER] = -1;
				}
				else
				{
					m_buttons[PAD_RSHOULDER] = 0;
				}
			}

			if (m_joyPad.dwButtons & JOY_BUTTON7)
			{
				if (m_buttons[PAD_LTHUMB] == 0)
				{
					m_buttons[PAD_LTHUMB] = 1;
				}
				else if (m_buttons[PAD_LTHUMB] >= 1)
				{
					m_buttons[PAD_LTHUMB]++;
				}
			}
			else
			{
				if (m_buttons[PAD_LTHUMB] > 0)
				{
					m_buttons[PAD_LTHUMB] = -1;
				}
				else
				{
					m_buttons[PAD_LTHUMB] = 0;
				}
			}

			if (m_joyPad.dwButtons & JOY_BUTTON8)
			{
				if (m_buttons[PAD_RTHUMB] == 0)
				{
					m_buttons[PAD_RTHUMB] = 1;
				}
				else if (m_buttons[PAD_RTHUMB] >= 1)
				{
					m_buttons[PAD_RTHUMB]++;
				}
			}
			else
			{
				if (m_buttons[PAD_RTHUMB] > 0)
				{
					m_buttons[PAD_RTHUMB] = -1;
				}
				else
				{
					m_buttons[PAD_RTHUMB] = 0;
				}
			}

			if (m_joyPad.dwButtons & JOY_BUTTON9)
			{
				if (m_buttons[PAD_BACK] == 0)
				{
					m_buttons[PAD_BACK] = 1;
				}
				else if (m_buttons[PAD_BACK] >= 1)
				{
					m_buttons[PAD_BACK]++;
				}
			}
			else
			{
				if (m_buttons[PAD_BACK] > 0)
				{
					m_buttons[PAD_BACK] = -1;
				}
				else
				{
					m_buttons[PAD_BACK] = 0;
				}
			}

			if (m_joyPad.dwButtons & JOY_BUTTON10)
			{
				if (m_buttons[PAD_START] == 0)
				{
					m_buttons[PAD_START] = 1;
				}
				else if (m_buttons[PAD_START] >= 1)
				{
					m_buttons[PAD_START]++;
				}
			}
			else
			{
				if (m_buttons[PAD_START] > 0)
				{
					m_buttons[PAD_START] = -1;
				}
				else
				{
					m_buttons[PAD_START] = 0;
				}
			}


		}

		bool WInput::CheckConnect()
		{
			m_joyPad.dwSize = sizeof(JOYINFOEX);
			m_joyPad.dwFlags = JOY_RETURNALL;		//すべての情報を取得

			static const UINT joyStickID[] =
			{
				JOYSTICKID1,
				JOYSTICKID2
			};

			return (joyGetPosEx(joyStickID[m_userIndex], &m_joyPad) == JOYERR_NOERROR);
		}

		VECTOR2F WInput::StickVectorLeft()
		{
			VECTOR2F vec2 = {};
			vec2.x = static_cast<float>((static_cast<int>(m_joyPad.dwXpos) - 0x7FFF)) / static_cast<float>(0x8000);
			vec2.y = static_cast<float>((static_cast<int>(m_joyPad.dwYpos) - 0x7FFF)) / static_cast<float>(0x8000);

			return vec2;
		}

		VECTOR2F WInput::StickVectorRight()
		{
			VECTOR2F vec2 = {};
			vec2.x = static_cast<float>((static_cast<int>(m_joyPad.dwZpos) - 0x7FFF)) / static_cast<float>(0x8000);
			vec2.y = static_cast<float>((static_cast<int>(m_joyPad.dwRpos) - 0x7FFF)) / static_cast<float>(0x8000);

			return vec2;
		}

		void Pads::SetUpPads()
		{
			for (int i = 0; i < 2; ++i)
			{
				std::shared_ptr<XInput> xinput = std::make_shared<XInput>();
				xinput->SetUserIndex(i);

				std::shared_ptr<WInput> winput = std::make_shared<WInput>();
				winput->SetUserIndex(i);

				if (!xinput->CheckConnect())
				{
					if (winput->CheckConnect())
					{
						m_pad[i] = winput;
					}
				}
				else if (xinput->CheckConnect())				
					m_pad[i] = xinput;
	
			}
		}

		void Pads::Update()
		{
			for (int i = 0; i < 2; ++i)
			{
				if (m_pad[i])
					m_pad[i]->InputState();
			}
		}

	}
}
