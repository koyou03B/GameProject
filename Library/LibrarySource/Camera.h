#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <memory>
#include "Input.h"
#include "Vector.h"
#include "ConstantBuffer.h"

namespace Source
{
	namespace CameraControlle
	{
		class Camera
		{
		public:
			Camera() = default;
			virtual ~Camera() = default;

			void Initialize();
			void Reset(const VECTOR3F& focus, const VECTOR3F & direction,
				const float& focalLength, const float& heightAboveGround)
			{
				m_focus = VECTOR4F(focus.x, focus.y, focus.z, 1.0f);
				m_eye.x = m_focus.x - direction.x * focalLength;
				m_eye.y = m_focus.y - direction.y * focalLength + heightAboveGround;
				m_eye.z = m_focus.z - direction.z * focalLength;
				m_eye.w = 1.0f;
			}

			void MoveEye(const VECTOR4F& eye);

			void DebugCamera();

			inline const VECTOR4F& GetEye()		const { return m_eye; }
			inline const VECTOR4F& GetFocus()	const { return m_focus; }
			inline const VECTOR4F& GetUp()		const { return m_up; }
			inline const VECTOR4F& GetRight()	const { return m_right; }
			inline const VECTOR4F& GetFront()	const { return m_front; }

			inline const float& GetFov()		const { return m_fov; }
			inline const float& GetAspect()		const { return m_aspect; }
			inline const float& GetNearZ()		const { return m_nearZ; }
			inline const float& GetFarZ()		const { return m_farZ; }


			inline void SetEye(VECTOR3F& eye)		{ m_eye = VECTOR4F(eye.x, eye.y, eye.z, 1.0f); }
			inline void SetFocus(VECTOR3F& focus)	{ m_focus = VECTOR4F(focus.x, focus.y, focus.z, 1.0f); }
			inline void SetFov(float& fov)			{ m_fov = fov; }
			inline void SetAspect(float aspect)		{ m_aspect = aspect; }
			inline void SetNearZ(float& nearZ)		{ m_nearZ = nearZ; }
			inline void SetFarZ(float& farZ)		{ m_farZ = farZ; }

		private:
			VECTOR4F m_eye = {};
			VECTOR4F m_focus = {};
			VECTOR4F m_up = {};
			VECTOR4F m_right = {};
			VECTOR4F m_front = {};

			VECTOR2F m_oldCursor = {};
			VECTOR2F m_newCursor = {};

			float m_fov = 0.0f;	 
			float m_aspect = 0.0f;
			float m_nearZ = 0.0f; 
			float m_farZ = 0.0f;	 
			
			float m_rotateX = 0.0f;
			float m_rotateY = 0.0f;
			float m_distance = 0.0f;

			float m_speed = 0.0f;
		};

		class CameraManager
		{
		public:
			enum CameraMode
			{
				LOCK_ON,
				CHANGE_OBJECT,
				ORBIT,
				FREE,
				END,
			};
		public:
			CameraManager() = default;
			~CameraManager() = default;

			void Initialize(ID3D11Device* device);
			void Update(float& elapsedTime);
			void LockON();
			void ChangeObject();
			void Vibrate(float elapsedTime);
			void Activate(ID3D11DeviceContext* immediateContext);
			void Deactivate(ID3D11DeviceContext* immediateContext) { m_constantsBuffer->Deactivate(immediateContext); }
			void Editor();

			void Reset(const VECTOR3F& target, const VECTOR3F& direction,
				const float& focalLength, const float& heightAboveGround)
			{
				m_target = target;
				m_direction = direction;
				m_focalLength = focalLength;
				m_heightAboveGround = heightAboveGround;
				m_camera->Reset(target, direction, focalLength, heightAboveGround);
			}

			inline const std::unique_ptr<Camera>& GetCamera() const { return m_camera; }
			inline const CameraMode GetCameraMode()		const { return m_mode; }
			inline const VECTOR3F GetTarget()			const { return m_target; }
			inline const VECTOR3F GetEye()				const { return m_eye; }
			inline const VECTOR3F GetDirection()		const { return m_direction; }
			inline const VECTOR3F GetLength()			const { return m_length; }
			inline const VECTOR3F GetRight()			const { return m_right; }
			inline const FLOAT4X4& GetView()			const { return m_constantsBuffer->data.view; }
			inline const FLOAT4X4& GetProjection()		const { return m_constantsBuffer->data.projection; }
			inline const FLOAT4X4& GetViewProjection()  const { return m_constantsBuffer->data.viewProjection; }
			inline const bool& GetNowChangeTarget()		const { return m_nowChangeTarget; }

			FLOAT4X4& GetCreateView();
			FLOAT4X4& GetCreateProjection();


			inline void SetCameraMode(const CameraMode& mode) { this->m_mode = mode; }

			inline void SetDistance(const VECTOR3F& distance) { this->m_direction = distance; }
			inline void SetOldDistance(const VECTOR3F& oldDistance) { this->m_oldDirection = oldDistance; }

			inline void SetObject(const VECTOR3F& object) { this->m_object = object; }
			inline void SetOldObject(const VECTOR3F& object) { this->m_oldObject = object; }
			inline void SetTarget(const VECTOR3F& target) { this->m_target = target; }
			inline void SetOldTarget(const VECTOR3F& oldTarget) { this->m_oldTarget = oldTarget; }
			inline void SetLength(const VECTOR3F& length) { this->m_length = length; }
			inline void SetRigth(const VECTOR3F& length) { this->m_right = length; }

			inline void SetValue(const float& value) { this->m_value = value; }
			inline void SetFocalLength(const float& focalLength) { this->m_focalLength = focalLength; }
			inline void SetHeightAboveGround(const float& height) { this->m_heightAboveGround = height; }
			inline void SetNowChangeTarget(const bool& changeTarget) { this->m_nowChangeTarget = changeTarget; }

			inline void SetNextEye()
			{
				m_nextEye.x = m_object.x + m_direction.x * m_length.x ;
				m_nextEye.y = m_object.y + m_direction.y * m_length.y + m_heightAboveGround;
				m_nextEye.z = m_object.z + m_direction.z * m_length.z;

				m_nextEye += m_right;
			}

			inline void SetVibration(float range, float timer) { m_vibrateRange = range; m_vibrateTimer = timer; }
			inline static CameraManager* GetInstance()
			{
				static CameraManager instance;
				return &instance;
			}

		private:
			struct ShaderConstants
			{
				VECTOR4F position;
				VECTOR4F direction;
				FLOAT4X4 view;
				FLOAT4X4 projection;
				FLOAT4X4 viewProjection;
				FLOAT4X4 inverseView;
				FLOAT4X4 inverseViewProjection;
				FLOAT4X4 previousViewProjection;
			};
			std::unique_ptr<Source::ConstantBuffer::ConstantBuffer<ShaderConstants>> m_constantsBuffer;
			std::unique_ptr<Camera> m_camera;

			CameraMode m_mode = {};

			VECTOR3F m_direction = {};
			VECTOR3F m_oldDirection = {};

			VECTOR3F m_object = {};
			VECTOR3F m_oldObject = {};

			VECTOR3F m_eye = {};
			VECTOR3F m_nextEye = {};

			VECTOR3F m_target = {};
			VECTOR3F m_oldTarget = {};

			VECTOR3F m_length = {};
			VECTOR3F m_angle = {};

			VECTOR3F m_right = {};

			float m_focalLength = 0.0f;
			float m_heightAboveGround = 0.0f;
			float m_value = 0.0f;
			float m_vibrateTimer = 0.0f;
			float m_vibrateRange = 0.0f;
			bool m_nowFreeMode = false;
			bool m_nowChangeTarget = false;
		};
	}
}
