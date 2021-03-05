#pragma once
#include <d3d11.h>
#include <iostream>
#include <sstream>
#include <string>
#include "AnimationBlend.h"
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
			T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
			DirectX::XMStoreFloat4x4(&world, S * R * T);
		}
	};

	struct Status
	{
		float    attackPoint = 0.0f;
		float	 damagePoint = 0.0f;
		float    life = 0.0f;
		float	 maxLife = 0.0f;

		bool isExit = false;
		bool isAttack = false;
		bool isDamage = false;
		
		uint32_t serialVersion = 0;
		template<class T>
		void serialize(T& archive, const std::uint32_t version)
		{

			if (serialVersion <= version)
			{
				archive
				(
					life
				);
			}
			else
			{
				archive
				(
					life
				);
			}

		}
	};

	struct Move
	{
		VECTOR3F speed = {};
		VECTOR3F accle = {};
		VECTOR3F maxSpeed[2] = {};
		VECTOR3F velocity = {};
		float decleleration = 0.0f;
		float turnSpeed = 0.0f;

		bool isMove = false;
		bool isWalk = false;
		bool isRun  = false;

		uint32_t serialVersion = 0;
		template<class T>
		void serialize(T& archive, const std::uint32_t version)
		{

			if (serialVersion <= version)
			{
				archive
				(
					speed,
					accle,
					maxSpeed[0], maxSpeed[1],
					velocity,
					decleleration,
					turnSpeed
				);
			}
			else
			{
				archive
				(
					speed,
					accle,
					maxSpeed[0], maxSpeed[1],
					velocity,
					decleleration,
					turnSpeed
				);
			}

		}
	};

	struct Step
	{
		VECTOR3F speed = {};
		VECTOR3F maxSpeed = {};
		VECTOR3F deceleration = {};
		float frameCount = 0.0f;
		bool isStep = false;

		uint32_t serialVersion = 0;
		template<class T>
		void serialize(T& archive, const std::uint32_t version)
		{

			if (serialVersion <= version)
			{
				archive
				(
					speed,
					deceleration,
					frameCount
				);
			}
			else
			{
				archive
				(
					speed,
					deceleration,
					frameCount
				);
			}
		}
	};

	struct Recover
	{
		bool isPlayer;
		float healValue[2];
		float recoverMaxTime[2];
		float standardLv;

		uint32_t serialVersion = 0;
		template<class T>
		void serialize(T& archive, const std::uint32_t version)
		{

			if (serialVersion <= version)
			{
				archive
				(
					healValue[0],
					healValue[1],
					recoverMaxTime[0],
					recoverMaxTime[1],
					standardLv
				);
			}
			else
			{
				archive
				(
					healValue[0],
					healValue[1],
					recoverMaxTime[0],
					recoverMaxTime[1],
					standardLv
				);
			}
		}
	};

	struct Attack
	{
		uint32_t frameCount = 0;
		uint32_t inputRange[2] = {};
		uint32_t slowTimeFrameCount[2] = {};
		uint32_t attackTimerRange[2] = {};
		uint32_t moveFrameStart = 0;
		float attackPoint = 0.0f;
		float attackSpeed[2] = {};
		bool hasAttacked = false;
		VECTOR3F speed = {};
		VECTOR3F maxSpeed = {};
		VECTOR3F deceleration = {};
		std::vector<XINPUT_GAMEPAD_BUTTONS> buttons;

		uint32_t serialVersion = 0;

		template<class T>
		void serialize(T& archive, const std::uint32_t version)
		{

			if (serialVersion <= version)
			{
				archive
				(
					frameCount,
					inputRange[0], inputRange[1],
					attackPoint,buttons, slowTimeFrameCount[0],
					attackSpeed[0], attackSpeed[1], slowTimeFrameCount[1],
					speed, deceleration, attackTimerRange[0], attackTimerRange[1], moveFrameStart
				);
			}
			else
			{
				archive
				(
					frameCount,
					inputRange[0], inputRange[1],
					attackPoint, buttons, slowTimeFrameCount[0],
					attackSpeed[0], attackSpeed[1], slowTimeFrameCount[1],
					speed, deceleration, attackTimerRange[0], attackTimerRange[1]

				);
			}

		}
	};

	struct Effect
	{
		std::unique_ptr<DebugObject> ballEffects;
		VECTOR4F	scrollValue;
	};

	struct Change
	{
		enum class PlayerType
		{
			//SABER,
			FIGHTER,
			ARCHER,
			
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
		VECTOR3F lenght = {};
		float value = 0.0f;
		float focalLength = 0.0f;
		float heightAboveGround = 0.0f;
		float rightValue = 0.0f;
		uint32_t serialVersion = 0;
		template<class T>
		void serialize(T& archive, const std::uint32_t version)
		{
			if (serialVersion <= version)
			{
				archive
				(
					lenght,
					value,
					focalLength,
					heightAboveGround,
					rightValue
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
		float blendValueRange[2] = {};

		float attackBlendRtio = 0.0f;
		float idleBlendRtio = 0.0f;
		float moveBlendRatio = 0.0f;
		float damageBlendRatio = 0.0f;
		float blendRatio = 0.0f;
		int samplerSize ;
		float blendRatioMax = 1.0f;

		uint32_t serialVersion = 0;

		template<class T>
		void serialize(T& archive, const std::uint32_t version)
		{
			if (serialVersion <= version)
			{
				archive
				(
					attackBlendRtio,
					idleBlendRtio,
					moveBlendRatio,
					blendValueRange[0],
					blendValueRange[1],
					damageBlendRatio
				);
			}
			else
			{
				archive
				(
					attackBlendRtio,
					idleBlendRtio,
					moveBlendRatio,
					blendValueRange[0],
					blendValueRange[1]
				);
			}
		}
	};
	
	struct Collision
	{
		VECTOR3F position[2] = {};
		float radius = 0.0f;
		float scale = 0.0f;

		enum CollisionType
		{
			CUBE,
			SPHER,
			CAPSULE,
			CIRCLE,
			RECT,
			CYLINDER
		} collisionType;

		inline void SetCurrentMesh(const uint32_t& mesh,const uint32_t& num) { currentMesh[num] = mesh; }
		inline void SetCurrentBone(const uint32_t& bone,const uint32_t& num) { currentBone[num] = bone; }
		inline int GetCurrentMesh(const uint32_t& num) { return currentMesh[num]; }
		inline int GetCurrentBone(const uint32_t& num) { return currentBone[num]; }

		uint32_t serialVersion = 0;

		template<class T>
		void serialize(T& archive, const std::uint32_t version)
		{
			if (serialVersion <= version)
			{
				archive
				(
					radius,
					scale,
					currentMesh[0], currentMesh[1],
					currentBone[0], currentBone[1],
					collisionType
				);
			}
			else
			{
				archive
				(
					radius,
					scale,
					currentMesh[0], currentMesh[1],
					currentBone[0], currentBone[1],
					collisionType
				);
			}
		}

	private:
		uint32_t currentMesh[2] = {};
		uint32_t currentBone[2] = {};
	};

	struct Damage
	{
		VECTOR3F vector;
		VECTOR3F speed;
		VECTOR3F deceleration;
		VECTOR3F maxSpeed;
		float hitComparison;
		bool hasBigDamaged;

		uint32_t serialVersion = 0;

		template<class T>
		void serialize(T& archive, const std::uint32_t version)
		{

			if (serialVersion <= version)
			{
				archive
				(
					speed,
					hitComparison,
					deceleration
				);
			}
			else
			{
				archive
				(
					speed,
					hitComparison,
					deceleration
				);
			}

		}
	};

	struct DebugObjects
	{
		DebugObject debugObject;
		DebugObject controlPoint;
		std::unique_ptr<Source::GeometricPrimitive::GeometricCube> GetCube(ID3D11Device* device,std::string fileName) 
		{
			return std::make_unique<Source::GeometricPrimitive::GeometricCube>(device, fileName);
		}
		std::unique_ptr<Source::GeometricPrimitive::GeometricSphere> GetSphere(ID3D11Device* device, std::string fileName)
		{
			return std::make_unique<Source::GeometricPrimitive::GeometricSphere>(device, fileName);
		}
		std::unique_ptr<Source::GeometricPrimitive::GeometricCapsule> GetCapsule(ID3D11Device* device, std::string fileName)
		{
			return std::make_unique<Source::GeometricPrimitive::GeometricCapsule>(device,fileName);
		}
	};
};

CEREAL_CLASS_VERSION(CharacterParameter::Status, 12);
CEREAL_CLASS_VERSION(CharacterParameter::Move, 12);
CEREAL_CLASS_VERSION(CharacterParameter::Camera, 12);
CEREAL_CLASS_VERSION(CharacterParameter::Collision, 12);
CEREAL_CLASS_VERSION(CharacterParameter::BlendAnimation, 13);
CEREAL_CLASS_VERSION(CharacterParameter::Step, 12);
CEREAL_CLASS_VERSION(CharacterParameter::Attack, 17);
CEREAL_CLASS_VERSION(CharacterParameter::Damage, 0);