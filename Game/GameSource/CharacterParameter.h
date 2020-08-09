#pragma once
#include <d3d11.h>
//#include "AnimationBlend.h"
#include "PartialBlendAnimation.h"
#include "DebugObject.h"
#include ".\LibrarySource\Vector.h"
#include ".\LibrarySource\GeometricPrimitve.h"

class  CharacterParameter
{
public:
	struct WorldTransform
	{
		VECTOR3F translate;
		VECTOR3F angle;
		VECTOR3F scale;
		FLOAT4X4 world;

		void WorldUpdate()
		{
			DirectX::XMMATRIX S, R, T;
			S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
			R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
			//	R = DirectX::XMMatrixRotationQuaternion(DirectX::XMVectorSet(m_angle.x,m_angle.y,m_angle.z,m_angle.w));
			T = DirectX::XMMatrixTranslation(translate.x, translate.y, translate.z);
			DirectX::XMStoreFloat4x4(&world, S * R * T);
		}
	};

	struct BlendAnimation
	{
		AnimationBlend animationBlend;
		PartialBlendAnimation partialBlend;
	};

	struct DebugObjects
	{
		DebugObject debugObject;

		std::unique_ptr<Source::GeometricPrimitive::GeometricCube> GetCube(ID3D11Device* device) 
		{
			return std::make_unique<Source::GeometricPrimitive::GeometricCube>(device);
		}

	};
};