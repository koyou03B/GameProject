#pragma once
#include ".\LibrarySource\Vector.h"

class AIParameter
{
public:
	struct Wrath
	{
		uint32_t wrathCost;
		uint32_t maxWrathCost;

		uint32_t attackWrathCost;
		uint32_t damageWrathCost;

		bool isWrath;
		uint32_t serialVersion = 1;

		template<class T>
		void serialize(T& archive, const std::uint32_t version)
		{

			if (serialVersion <= version)
			{
				archive
				(
					maxWrathCost
				);
			}
			else
			{
				archive
				(
					maxWrathCost
				);
			}

		}
	};

	struct Exhaustion
	{
		uint32_t exhaustionCost;
		uint32_t maxExhaustionCost; 
		uint32_t forgetExhaustionCost;

		uint32_t moveExhaustionCost;
		uint32_t attackExhaustionCost;
		uint32_t damageExhaustionCost;

		bool isExhaustion;
		uint32_t serialVersion = 1;

		template<class T>
		void serialize(T& archive, const std::uint32_t version)
		{

			if (serialVersion <= version)
			{
				archive
				(
					maxExhaustionCost,
					forgetExhaustionCost,
					moveExhaustionCost,
					attackExhaustionCost,
					damageExhaustionCost
				);
			}
			else
			{
				archive
				(
					maxExhaustionCost,
					forgetExhaustionCost,
					moveExhaustionCost,
					attackExhaustionCost,
					damageExhaustionCost
				);
			}

		}
	};

	struct Emotion
	{
		Wrath wrathParm;
		Exhaustion  exhaustionParm;

		uint32_t serialVersion = 1;

		template<class T>
		void serialize(T& archive, const std::uint32_t version)
		{
			if (serialVersion <= version)
			{
				archive
				(
					wrathParm,
					exhaustionParm
				);
			}
			else
			{
				archive
				(
					wrathParm,
					exhaustionParm
				);
			}

		}
	};

	struct JudgeElement
	{
		uint32_t damageCount = 0; //WaitNode
		uint32_t attackHitCount = 0; //
		uint32_t moveCount = 0;   //WaitNode
		uint32_t attackCount = 0; //WaitNode
		int targetID = 0;
	};

	struct StandardValue
	{
		uint32_t attackCountValue = 0;
		uint32_t attackHitCountValue = 0;
		float viewFrontValue = 0.0f;

		uint32_t serialVersion = 1;

		template<class T>
		void serialize(T& archive, const std::uint32_t version)
		{
			if (serialVersion <= version)
			{
				archive
				(
					attackCountValue,
					attackHitCountValue,
					viewFrontValue
				);
			}
			else
			{
				archive
				(
					attackCountValue,
					attackHitCountValue,
					viewFrontValue
				);
			}

		}
	};
};

CEREAL_CLASS_VERSION(AIParameter::Wrath, 1);
CEREAL_CLASS_VERSION(AIParameter::Exhaustion, 1);
CEREAL_CLASS_VERSION(AIParameter::Emotion, 1);
CEREAL_CLASS_VERSION(AIParameter::StandardValue, 1);