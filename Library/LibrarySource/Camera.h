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
			void Reset(const VECTOR3F& direction,const float& focalLength, const float& heightAboveGround);
			bool LockON(VECTOR3F& target, float& elapsedTimie);
			bool Vibrate(float& elapsedTime);
			void OrbitCamera(float& elapsedTimie);
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
			inline void SetVibration(float range, float timer) { m_vibrateRange = range; m_vibrateTimer = timer; }


		private:
			VECTOR4F m_eye = {};
			VECTOR4F m_focus = {};
			VECTOR4F m_up = {};
			VECTOR4F m_right = {};
			VECTOR4F m_front = {};

			VECTOR2F m_oldCursor = {};
			VECTOR2F m_newCursor = {};

			VECTOR4F m_oldFocus = {};
			VECTOR3F m_oldDirection = {};
			int m_state = 0;

			float m_fov = 0.0f;	 
			float m_aspect = 0.0f;
			float m_nearZ = 0.0f; 
			float m_farZ = 0.0f;	 
			
			float m_rotateX = 0.0f;
			float m_rotateY = 0.0f;
			float m_distance = 0.0f;

			float m_speed = 0.0f;
			float m_vibrateTimer = 0.0f;
			float m_vibrateRange = 0.0f;
			float m_focalLength = 0.0f;
			float m_heightAboveGround = 0.0f;
			float m_lerpValue = 0.0f;

			float m_cameraHAngle = 0.0f;
			float m_cameraVAngle = 0.0f;
		};

		class CameraManager
		{
		public:
			enum CameraMode
			{
				LOCK_ON,
				ORBIT,
				VIBRATION,
				FREE,
				MONITOR,
				END,
			};
		public:
			CameraManager() = default;
			~CameraManager() = default;

			void Initialize(ID3D11Device* device);
			void Update(float& elapsedTime);
			void Activate(ID3D11DeviceContext* immediateContext);
			void Deactivate(ID3D11DeviceContext* immediateContext) { m_constantsBuffer->Deactivate(immediateContext); }
			void Editor();


			inline const std::unique_ptr<Camera>& GetCamera() const { return m_camera; }
			inline const CameraMode GetCameraMode()		const { return m_mode; }
			inline const FLOAT4X4& GetView()			const { return m_constantsBuffer->data.view; }
			inline const FLOAT4X4& GetProjection()		const { return m_constantsBuffer->data.projection; }
			inline const FLOAT4X4& GetViewProjection()  const { return m_constantsBuffer->data.viewProjection; }
			inline const bool& GetTutorialCommand() const { return m_tutorialCommand; }
			inline void SetCameraMode(const CameraMode& mode) { this->m_mode = mode; }
			inline void SetLockOnTarget(const VECTOR3F& target) { this->m_lockOnTarget = target; }
			void SetFocus(VECTOR3F& focus);
			void SetVibration(const float& vibrateRange, const float& vibrateTimer);
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
			VECTOR3F m_lockOnTarget = {};
			bool m_nowFreeMode = false;
			bool m_tutorialCommand = false;
		};
	}
}
