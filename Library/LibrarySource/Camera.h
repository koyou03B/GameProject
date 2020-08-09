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
			void Reset(const VECTOR4F & target, const VECTOR4F & direction,
				const float& focalLength, const float& heightAboveGround)
			{
				m_focus = target;
				m_eye.x = m_focus.x - direction.x * focalLength;
				m_eye.y = m_focus.y - direction.y * focalLength + heightAboveGround;
				m_eye.z = m_focus.z - direction.z * focalLength;
				m_eye.w = 1.0f;
			}
			void DebugCamera();


			inline const VECTOR4F& GetEye() const { return m_eye; }
			inline const VECTOR4F& GetFocus() const { return m_focus; }
			inline const VECTOR4F& GetUp() const { return m_up; }
			inline const VECTOR4F& GetRight() const { return m_right; }
			inline const VECTOR4F& GetFront() const { return m_front; }

			inline const float& GetFov() const { return m_fov; }
			inline const float& GetAspect() const { return m_aspect; }
			inline const float& GetNearZ() const { return m_nearZ; }
			inline const float& GetFarZ() const { return m_farZ; }

			inline void SetFov(float& fov) { m_fov = fov; }
			inline void SetAspect(float aspect) { m_aspect = aspect; }
			inline void SetNearZ(float& nearZ) { m_nearZ = nearZ; }
			inline void SetFarZ(float& farZ) { m_farZ = farZ; }
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


		};

		class CameraManager
		{
		public:
			CameraManager() = default;
			~CameraManager() = default;

			void Initialize(ID3D11Device* device);
			void Update(float& elapsedTime);
			void Activate(ID3D11DeviceContext* immediateContext);
			void Deactivate(ID3D11DeviceContext* immediateContext) { m_constantsBuffer->Deactivate(immediateContext); }

			void Editor();

			inline const FLOAT4X4& GetView() const { return m_constantsBuffer->data.view; }
			inline const FLOAT4X4& GetProjection() const { return m_constantsBuffer->data.projection; }
			inline const FLOAT4X4& GetViewProjection() const { return m_constantsBuffer->data.viewProjection; }

			FLOAT4X4& GetCreateView();
			FLOAT4X4& GetCreateProjection();

			void SetTarget(const VECTOR4F& target) { this->m_target = target; }

			void Reset(const VECTOR4F& target, const VECTOR4F& direction,
				const float& focalLength, const float& heightAboveGround)
			{
				m_target = target;
				m_direction = direction;
				m_focalLength = focalLength;
				m_heightAboveGround = heightAboveGround;
				m_camera->Reset(target, direction, focalLength, heightAboveGround);
			}

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

			VECTOR4F m_direction = {};
			VECTOR4F m_target = {};

			VECTOR3F m_angle = {};
			float m_focalLength = 0.0f;
			float m_heightAboveGround = 0.0f;

			bool m_nowFreeMode = false;
		};


	}
}
