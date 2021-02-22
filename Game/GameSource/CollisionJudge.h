#pragma once
#include "Collision.h"
#include "CharacterAI.h"
class CollisionJudge
{
public:
	CollisionJudge() = default;
	~CollisionJudge() = default;

	bool JudgeAttack(CharacterParameter::Collision& target, CharacterParameter::Collision& attackColl);
	bool JudgeDistnace(CharacterAI* mySelf,CharacterAI* target);
private:
	bool JudgeSphereAndSphere(CharacterParameter::Collision& target, CharacterParameter::Collision& attackColl);
	bool JudgeCapsuleAndSphere(CharacterParameter::Collision& target, CharacterParameter::Collision& attackColl);
	bool JudgeCylinderAndCylinder(CharacterParameter::Collision& target, CharacterParameter::Collision& attackColl);
private:
	const int body = 0;
	Collision m_collision;
};