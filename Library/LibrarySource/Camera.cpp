#include "Camera.h"
#include "Framework.h"
namespace Source
{
	namespace CameraControlle
	{
		void Camera::Initialize()
		{
			m_eye = VECTOR4F(0.0f, 0.0f, 0.0f, 1.0f);
			m_focus = VECTOR4F(0.0f, 0.0f, 1.0f, 1.0f);
			m_up = VECTOR4F(0.0f, 1.0f, 0.0f, 1.0f);
			m_right = VECTOR4F(1.0f, 0.0f, .0f, 1.0f);
			m_front = VECTOR4F(.0f, 0.0f, 1.0f, 1.0f);

			m_oldCursor = VECTOR2F(0.0f, 0.0f);
			m_newCursor = VECTOR2F(0.0f, 0.0f);

			m_fov = 30 * 0.01745f;
			m_aspect = 0.0f;
			m_nearZ = 0.1f;
			m_farZ = 3000.0f;

			m_rotateX = 0.0f;
			m_rotateY = 0.0f;
			m_distance = 0.0f;
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

			m_angle = VECTOR3F(0.0f, 0.0f, 0.0f);
			m_direction = VECTOR4F(sinf(m_angle.x), m_angle.y, cosf(m_angle.z), 1.f);

			m_target = VECTOR4F(0.0f, 0.0f, 0.0f, 1.0f);
			m_focalLength = 120.0f;
			m_heightAboveGround = 34.0f;

			m_nowFreeMode = false;

			m_camera = std::make_unique<Camera>();
			m_camera->Initialize();
		}

		void CameraManager::Update(float& elapsedTime)
		{
			if (XINPUT.StickDeadzoneRX(10000))
			{
				m_angle.y += 0.005f * (XINPUT.GetStickRxValue() / 33000.0f);
				if (m_angle.y > DirectX::XM_PI)
				{
					m_angle.y -= DirectX::XM_PI;
				}
			}

			m_direction = VECTOR4F(sinf(m_angle.y), 0.f, cosf(m_angle.y), 1.f);

			if (!m_nowFreeMode)
				m_camera->Reset(m_target, m_direction, m_focalLength, m_heightAboveGround);
			else
				m_camera->DebugCamera();
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

		FLOAT4X4& CameraManager::GetCreateView()
		{
			DirectX::XMVECTOR E = DirectX::XMLoadFloat4(&m_camera->GetEye());
			DirectX::XMVECTOR F = DirectX::XMLoadFloat4(&m_camera->GetFocus());
			DirectX::XMVECTOR U = DirectX::XMLoadFloat4(&m_camera->GetUp());
			DirectX::XMMATRIX V = DirectX::XMMatrixLookAtLH(E, F, U);
			DirectX::XMStoreFloat4x4(&m_constantsBuffer->data.view, V);

			return m_constantsBuffer->data.view;
		}
		FLOAT4X4& CameraManager::GetCreateProjection()
		{
			D3D11_VIEWPORT viewport;
			UINT num_viewports = 1;
			Framework::GetContext()->RSGetViewports(&num_viewports, &viewport);
			float aspectRatio = viewport.Width / viewport.Height;

			m_camera->SetAspect(aspectRatio);
			DirectX::XMMATRIX P = DirectX::XMMatrixPerspectiveFovLH(m_camera->GetFov(), m_camera->GetAspect(), m_camera->GetNearZ(), m_camera->GetFarZ());
			DirectX::XMStoreFloat4x4(&m_constantsBuffer->data.projection, P);

			return m_constantsBuffer->data.projection;
		}

		void CameraManager::Editor()
		{
#ifdef _DEBUG

			if (ImGui::CollapsingHeader("Mode_Config"))
			{
				bool isCameraFreeMode = m_nowFreeMode;
				VECTOR4F target;

				if (ImGui::Checkbox("CameraFree", &m_nowFreeMode))
				{
					if (!isCameraFreeMode)
					{
						target = m_target;
					}
				}
				if (isCameraFreeMode && !m_nowFreeMode)
				{
					Initialize(Framework::GetInstance().GetDevice());
					Reset(target, m_direction, m_focalLength, m_heightAboveGround);
				}
			}

			if (ImGui::CollapsingHeader("Camera_Config"))
			{
				if (ImGui::TreeNode(u8"Ž‹“_—v¿"))
				{
					float focalLength = m_focalLength;
					float heightAboveGround = m_heightAboveGround;
					ImGui::InputFloat(u8"focalLength", &focalLength, 1.0f, 1.0f);
					ImGui::InputFloat(u8"heightAboveGround", &heightAboveGround, 1.0f, 1.0f);
					CameraManager::GetInstance()->Reset(m_target, m_direction, focalLength, heightAboveGround);
					ImGui::TreePop();
				};
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
			ImGuiStyle& style = ImGui::GetStyle();
			style.Colors[ImGuiCol_Separator] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
			ImGui::Separator();
			ImGui::Separator();
			ImGui::BulletText("angle : %f", m_angle);
			ImGui::BulletText("focalLength : %f", m_focalLength);
			ImGui::BulletText("heightAboveGround : %f", m_heightAboveGround);

#endif // !USE_IMGUI
		}
	}
}

