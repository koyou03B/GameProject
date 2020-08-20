#pragma once
#include <d3d11.h>
#include <iostream>
#include <sstream>
#include <string>
#include "PartialBlendAnimation.h"
#include "DebugObject.h"
#include ".\LibrarySource\Vector.h"
#include ".\LibrarySource\GeometricPrimitve.h"

#undef max
#undef min
#include<cereal/cereal.hpp>
#include<cereal/archives/binary.hpp>
#include<cereal/types/memory.hpp>
#include<cereal/types/vector.hpp>

class  CharacterParameter
{
public:
	struct WorldTransform
	{
		VECTOR3F position = {};
		VECTOR3F angle = {};
		VECTOR3F scale = {};
		FLOAT4X4 world = {};

		void WorldUpdate()
		{
			DirectX::XMMATRIX S, R, T;
			S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
			R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
			//	R = DirectX::XMMatrixRotationQuaternion(DirectX::XMVectorSet(m_angle.x,m_angle.y,m_angle.z,m_angle.w));
			T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
			DirectX::XMStoreFloat4x4(&world, S * R * T);
		}
	};

	struct Status
	{
		VECTOR3F speed;
		VECTOR3F velocity;
		float    attackPoint;
		float    life;

		bool isExit = false;
		bool isAttack = false;
		bool isDamage = false;

		void Init()
		{
			speed = {};
			velocity = {};
			attackPoint = 0.0f;
			life = 0.0f;
		}

		template<class T>
		void serialize(T& archive, const std::uint32_t version)
		{

			if (1 <= version)
			{
				archive
				(
					speed,
					velocity,
					attackPoint,
					life
				);
			}
			else
			{
				archive
				(
					speed,
					velocity,
					attackPoint,
					life
				);
			}

		}
	};



	struct Step
	{
		VECTOR3F speed;
		VECTOR3F accel;

		float maxSpeed;
		bool isStep;

		void Init()
		{
			speed = {};
			accel = {};
			maxSpeed = 0.0f;
			isStep = false;
		}

		template<class T>
		void serialize(T& archive, const std::uint32_t version)
		{

			if (1 <= version)
			{
				archive
				(
					speed,
					accel,
					maxSpeed
				);
			}
			else
			{
				archive
				(
					speed,
					accel,
					maxSpeed
				);
			}

		}
	};

	struct Change
	{
		enum class PlayerType
		{
			SABER,
			ARCHER,
			FIGHTER
		};
		PlayerType changeType;
		bool isPlay = false;
	};

	struct MessageFlg
	{
		bool askAttack = false;
		bool askProtection = false;
		bool callFriend = false;
		bool callHelp = false;
		bool changePlayer = false;


	};

	struct Camera
	{
		VECTOR3F lenght;
		float value;
		float focalLength;
		float heightAboveGround;

		void Init()
		{
			lenght = {};
			value = 0.0f;
			focalLength = 0.0f;
			heightAboveGround = 0.0f;
		}
		template<class T>
		void serialize(T& archive, const std::uint32_t version)
		{
			if (1 <= version)
			{
				archive
				(
					lenght,
					value,
					focalLength,
					heightAboveGround
				);
			}
			else
			{
				archive
				(
					lenght,
					value,
					focalLength,
					heightAboveGround
				);
			}
		}
	};

	struct BlendAnimation
	{
		AnimationBlend animationBlend;
		PartialBlendAnimation partialBlend;
		float animtionTime = -1.0f;
		float animtionSpeed = 1.0f;
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