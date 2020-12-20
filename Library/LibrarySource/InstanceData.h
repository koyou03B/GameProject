#pragma once
#include <vector>
#include "Vector.h"

namespace Source
{
	namespace InstanceData
	{
		struct InstanceData
		{
		public:
			VECTOR3F position;
			VECTOR3F scale;
			VECTOR3F angle;
			VECTOR4F color;
			FLOAT4X4 world;

			void CreateWorld()
			{
				DirectX::XMMATRIX S, R, T;

				S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
				R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
				T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);

				DirectX::XMMATRIX W = S * R * T;
				DirectX::XMStoreFloat4x4(&world, W);
			}

			void ClearData()
			{
				position = {};
				scale = {};
				angle = {};
				color = {};
				world = {};
			}
		};
	}
}
