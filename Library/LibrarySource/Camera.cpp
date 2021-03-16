#include "Camera.h"
#include "Input.h"
#include "Framework.h"
namespace Source
{
	namespace CameraControlle
	{
		void Camera::Initialize()
		{
			m_eye = VECTOR4F(0.0f, 0.0f, 0.0f, 1.0f);
			m_focus = VECTOR4F(0.0f, 0.0f, 0.0f, 1.0f);

			m_up = VECTOR4F(0.0f, 1.0f, 0.0f, 1.0f);
			m_right = VECTOR4F(1.0f, 0.0f, .0f, 1.0f);
			m_front = VECTOR4F(.0f, 0.0f, 1.0f, 1.0f);

			m_oldCursor = VECTOR2F(0.0f, 0.0f);
			m_newCursor = VECTOR2F(0.0f, 0.0f);
			m_oldDirection = {};
			m_oldFocus = {};

			m_fov = 30 * 0.01745f;
			m_aspect = 0.0f;
			m_nearZ = 0.1f;
			m_farZ = 5000.0f;

			m_rotateX = 0.0f;
			m_rotateY = 0.0f;
			m_distance = 0.0f;
			m_lerpValue = 0.0f;

			m_speed = 0.0f;
			m_focalLength = 40.0f;
			m_heightAboveGround = 10.0f;

			Reset(VECTOR3F(sinf(0.0f), 0.0f, cosf(0.0f)), m_focalLength, m_heightAboveGround);
		}
		void Camera::Reset(const VECTOR3F& direction, const float& focalLength, const float& heightAboveGround)
		{
			m_eye.x = m_focus.x - direction.x * focalLength;
			m_eye.y = m_focus.y - direction.y * focalLength + heightAboveGround;
			m_eye.z = m_focus.z - direction.z * focalLength;
			m_eye.w = 1.0f;
		}

		bool Camera::LockON(VECTOR3F& target, float& elapsedTimie)
		{
			VECTOR3F focus = { m_focus.x,m_focus.y,m_focus.z };
			VECTOR3F eye = { m_eye.x,m_eye.y,m_eye.z };
			static float timer = 0.0f;
			switch (m_state)
			{
			case 0:
				//Player‚ðŠî€‚É‚µ‚Ä‚¢‚½focus,Œü‚«‚ðŽæ“¾
				m_oldDirection = NormalizeVec3(eye - focus);
				m_distance = ToDistVec3(eye - focus);
				timer = 0.0f;
				++m_state;
			break;
			case 1:

				//target‚Æfocus(Player)‚ÌŒü‚«
				VECTOR3F direction = NormalizeVec3(focus - target);
				//ƒvƒŒƒCƒ„[Œü‚«‚Æ“GŒü‚«‚ÅüŒ`•âŠÔ
				VECTOR3F nDistance = SphereLinearVec3(m_oldDirection, direction, m_lerpValue);
				m_eye.x = m_focus.x + nDistance.x * m_distance;
				m_eye.y = m_focus.y + nDistance.y * m_distance;
				m_eye.z = m_focus.z + nDistance.z * m_distance;
				if (m_lerpValue > 1.0f)
				{
					m_lerpValue = 0;
					m_state = 0;
					return true;
					//timer += elapsedTimie;
					//if (timer > 1.0f)
					//{

					//}
				}
				else
					m_lerpValue += 0.07f;
			}

			return false;
		}

		bool Camera::Vibrate(float& elapsedTime)
		{
			switch (m_state)
			{
			case 0:
				m_oldFocus = m_oldFocus;
				++m_state;
				break;
			case 1:
				if (m_vibrateTimer < 0.0f)
				{
					m_state = 0;
					return true;
				}

				m_vibrateTimer -= elapsedTime;

				float dx = (rand() % 3 - 1) * m_vibrateRange * m_vibrateTimer;
				float dy = (rand() % 3 - 1) * m_vibrateRange * m_vibrateTimer;
				float dz = (rand() % 3 - 1) * m_vibrateRange * m_vibrateTimer;

				m_focus.x += dx;
				m_focus.y += dy;
				m_focus.z += dz;

				break;
			}
			return false;
		}

		void Camera::OrbitCamera(float& elapsedTimie)
		{
			float yOffset, xOffset = 0.0f;
			Source::Input::Input* input = PAD.GetPad(0);
			if (!input) return;

			yOffset = input->StickDeadzoneRX(3000) ? input->StickVectorRight().x : 0.0f;
			xOffset = input->StickDeadzoneRY(3000) ? input->StickVectorRight().y : 0.0f;
			float sensitivityY = 2.0f * elapsedTimie;
			float sensitivityX = 2.0f * elapsedTimie;

			DirectX::XMVECTOR CP = DirectX::XMLoadFloat4(&m_eye);
			DirectX::XMVECTOR CF = DirectX::XMLoadFloat4(&m_focus);
			DirectX::XMVECTOR CX, CY, CZ;
			CY = DirectX::XMVectorSet(0, 1, 0, 0);
			using namespace DirectX;
			CZ = DirectX::XMVector3Normalize(CF - CP);
			CX = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(CY, CZ));
			DirectX::XMMATRIX RY = DirectX::XMMatrixRotationAxis(CY, yOffset * sensitivityY);
			DirectX::XMMATRIX RX = DirectX::XMMatrixRotationAxis(CX, xOffset * sensitivityX);
			CP = m_focalLength * DirectX::XMVector3Normalize(DirectX::XMVector3Transform(CP - CF, RX * RY)) + CF;
			VECTOR4F eye, focus;
			DirectX::XMStoreFloat4(&eye, CP);
			if (eye.y < 7.0f) eye.y = 7.0f;
			if (eye.y > 44.0f) eye.y = 44.0f;

			m_eye = eye;
		}

		void Camera::DebugCamera()
		{
			float x = m_focus.x - m_eye.x;
			float y = m_focus.y - m_eye.y;
			float z = m_focus.z - m_eye.z;

			m_distance = ::sqrtf(x * x + y * y + z * z);

			m_oldCursor = m_newCursor;
			m_newCursor = VECTOR2F(MOUSE.GetCursorPosX(), MOUSE.GetCursorPosY());

			float move_x = (m_newCursor.x - m_oldCursor.x) * 0.02f;
			float move_y = (m_newCursor.y - m_oldCursor.y) * 0.02f;


			if (KEYBOARD._keys[DIK_LMENU])
			{
				if (MOUSE.PressedState(MouseLabel::LEFT_BUTTON))
				{
					// YŽ²‰ñ“]
					m_rotateY += move_x * 0.5f;
					if (m_rotateY > DirectX::XM_PI)
					{
						m_rotateY -= DirectX::XM_2PI;
					}
					else if (m_rotateY < -DirectX::XM_PI)
					{
						m_rotateY += DirectX::XM_2PI;
					}
					// XŽ²‰ñ“]
					m_rotateX += move_y * 0.5f;
					if (m_rotateX > DirectX::XM_PI)
					{
						m_rotateX -= DirectX::XM_2PI;
					}
					else if (m_rotateX < -DirectX::XM_PI)
					{
						m_rotateX += DirectX::XM_2PI;
					}
				}
				else if (MOUSE.PressedState(MouseLabel::MID_BUTTON))
				{
					// •½sˆÚ“®
					float s = m_distance * 0.035f;
					float x = -move_x * s;
					float y = move_y * s;

					m_focus.x += m_right.x * x;
					m_focus.y += m_right.y * x;
					m_focus.z += m_right.z * x;

					m_focus.x += m_up.x * y;
					m_focus.y += m_up.y * y;
					m_focus.z += m_up.z * y;
				}
				else if (MOUSE.PressedState(MouseLabel::RIGHT_BUTTON))
				{
					// ƒY[ƒ€
					m_distance += (-move_y - move_x) * m_distance * 0.1f;
				}

				float	x_sin = ::sinf(m_rotateX);
				float	x_cos = ::cosf(m_rotateX);
				float	y_sin = ::sinf(m_rotateY);
				float	y_cos = ::cosf(m_rotateY);

				DirectX::XMVECTOR front = DirectX::XMVectorSet(y_sin * x_cos, x_sin, y_cos * x_cos, 0.0f);
				DirectX::XMVECTOR right = DirectX::XMVectorSet(y_cos, 0, -y_sin, 0.0f);
				DirectX::XMVECTOR up = DirectX::XMVector3Cross(front, right);

				DirectX::XMVECTOR focus = DirectX::XMLoadFloat4(&m_focus);
				DirectX::XMVECTOR distance = DirectX::XMVectorSet(m_distance, m_distance, m_distance, 0.0f);
				DirectX::XMVECTOR eye = DirectX::XMVectorSubtract(focus, DirectX::XMVectorMultiply(front, distance));

				DirectX::XMMATRIX view = DirectX::XMMatrixLookAtLH(eye, focus, up);
				view = DirectX::XMMatrixTranspose(view);

				right = DirectX::XMVector3TransformNormal(DirectX::XMVectorSet(1, 0, 0, 0), view);
				up = DirectX::XMVector3TransformNormal(DirectX::XMVectorSet(0, 1, 0, 0), view);
				front = DirectX::XMVector3TransformNormal(DirectX::XMVectorSet(0, 0, 1, 0), view);

				DirectX::XMStoreFloat4(&m_eye, eye);
				DirectX::XMStoreFloat4(&m_up, up);
				DirectX::XMStoreFloat4(&m_right, right);

				DirectX::XMStoreFloat4(&m_front, front);
				DirectX::XMStoreFloat4(&m_right, right);
				DirectX::XMStoreFloat4(&m_up, up);
			}
		}

		void CameraManager::Initialize(ID3D11Device* device)
		{
			m_constantsBuffer = std::make_unique<Source::ConstantBuffer::ConstantBuffer<ShaderConstants>>(device);
			m_nowFreeMode = false;
			m_mode = CameraMode::ORBIT;
			m_camera = std::make_unique<Camera>();
			m_camera->Initialize();
		}

		void CameraManager::Update(float& elapsedTime)
		{
			switch (m_mode)
			{
			case CameraMode::LOCK_ON:
				if (m_camera->LockON(m_lockOnTarget,elapsedTime))
					m_mode = CameraMode::ORBIT;
				break;
			case CameraMode::ORBIT:
				m_camera->OrbitCamera(elapsedTime);
				break;
			case CameraMode::VIBRATION:
				if (m_camera->Vibrate(elapsedTime))
					m_mode = CameraMode::ORBIT;
				break;
			case CameraMode::FREE:
				m_camera->DebugCamera();
				break;
			case CameraMode::MONITOR:
				break;
			default:
				break;
			}

			Source::Input::Input* input = PAD.GetPad(0);
			if (!input) return;
			if (m_mode == CameraMode::ORBIT && input->GetButtons(XINPUT_GAMEPAD_BUTTONS::PAD_RSHOULDER))
			{
				m_mode = CameraMode::LOCK_ON;
				m_tutorialCommand = true;
			}
		}

		void CameraManager::Activate(ID3D11DeviceContext* immediateContext)
		{
			D3D11_VIEWPORT viewport;
			UINT num_viewports = 1;
			immediateContext->RSGetViewports(&num_viewports, &viewport);
			float aspectRatio = viewport.Width / viewport.Height;

			m_camera->SetAspect(aspectRatio);

			DirectX::XMVECTOR E = DirectX::XMLoadFloat4(&m_camera->GetEye());
			DirectX::XMVECTOR F = DirectX::XMLoadFloat4(&m_camera->GetFocus());
			DirectX::XMVECTOR U = DirectX::XMLoadFloat4(&m_camera->GetUp());
			using namespace DirectX;
			DirectX::XMVECTOR D = DirectX::XMVector3Normalize(F - E);

			DirectX::XMStoreFloat4(&m_constantsBuffer->data.position, E);
			DirectX::XMStoreFloat4(&m_constantsBuffer->data.direction, D);

			DirectX::XMMATRIX V = DirectX::XMMatrixLookAtLH(E, F, U);
			DirectX::XMStoreFloat4x4(&m_constantsBuffer->data.view, V);
			DirectX::XMStoreFloat4x4(&m_constantsBuffer->data.inverseView, DirectX::XMMatrixInverse(nullptr, V));

			DirectX::XMMATRIX P = DirectX::XMMatrixPerspectiveFovLH(m_camera->GetFov(), m_camera->GetAspect(), m_camera->GetNearZ(), m_camera->GetFarZ());
			DirectX::XMStoreFloat4x4(&m_constantsBuffer->data.projection, P);
			m_constantsBuffer->data.previousViewProjection = m_constantsBuffer->data.viewProjection;
			DirectX::XMStoreFloat4x4(&m_constantsBuffer->data.viewProjection, V * P);
			DirectX::XMStoreFloat4x4(&m_constantsBuffer->data.inverseViewProjection, DirectX::XMMatrixInverse(0, V * P));

			m_constantsBuffer->Activate(immediateContext, SLOT1, true, true);
		}

		void CameraManager::Editor()
		{
#ifdef _DEBUG

			if (ImGui::CollapsingHeader("Mode_Config"))
			{
				bool isCameraFreeMode = m_nowFreeMode;
				VECTOR3F target;

				if (ImGui::Checkbox("CameraFree", &m_nowFreeMode))
				{
					m_mode = CameraMode::FREE;
				}
				if (isCameraFreeMode && !m_nowFreeMode)
				{
					m_mode = CameraMode::ORBIT;
				}
				if (ImGui::Button("LockOn"))
				{
					m_mode = CameraMode::LOCK_ON;
				}
				if (ImGui::Button("Vibration"))
				{
					m_mode = CameraMode::VIBRATION;
					SetVibration(0.5f, 0.5f);
				}
			}

			if (ImGui::CollapsingHeader("Camera_Config"))
			{
				if (ImGui::TreeNode(u8"‰æŠp’²®"))
				{
					float fov = m_camera->GetFov();
					ImGui::SliderFloat(u8"‰æŠp", &fov, 0.00f, 3.14f);
					m_camera->SetFov(fov);
					ImGui::TreePop();
				};
				if (ImGui::TreeNode(u8"Ž‹Šo‹——£’²®"))
				{
					float nearZ = m_camera->GetNearZ();
					float farZ = m_camera->GetFarZ();
					ImGui::InputFloat(u8"Near–Ê", &nearZ, 1.0f, 1.0f);
					ImGui::InputFloat(u8"far–Ê", &farZ, 10.0f, 10.0f);
					m_camera->SetNearZ(nearZ);
					m_camera->SetFarZ(farZ);
					ImGui::TreePop();
				};
			}

#endif // !USE_IMGUI
		}

		void CameraManager::SetFocus(VECTOR3F& focus)
		{
			m_camera->SetFocus(focus);
		}
		void CameraManager::SetVibration(const float& vibrateRange, const float& vibrateTimer)
		{
			m_camera->SetVibration(vibrateRange, vibrateTimer);
			m_mode = CameraMode::VIBRATION;
		}
	}
}

