#include "CollisionJudge.h"

bool CollisionJudge::JudgeAttack(CharacterParameter::Collision& target
	, CharacterParameter::Collision& attackColl)
{
	switch (attackColl.collisionType)
	{
	case CharacterParameter::Collision::SPHER:
		return JudgeSphereAndSphere(target, attackColl);
		break;
	case CharacterParameter::Collision::CAPSULE:
		return JudgeCapsuleAndSphere(target, attackColl);
		break;
	case CharacterParameter::Collision::CYLINDER:
		return JudgeCylinderAndCylinder(target, attackColl);
		break;
	}

	return false;
}

bool CollisionJudge::JudgeDistnace(CharacterAI* mySelf, CharacterAI* target)
{
	Collision::Circle mySelfC, targetC;
	VECTOR3F bodyPos = mySelf->GetCollision()[0].position[0];
	float radius = mySelf->GetCollision()[0].radius;
	float scale = mySelf->GetCollision()[0].scale;
	
	mySelfC.position = { bodyPos.x,bodyPos.z };
	mySelfC.radius = radius;
	mySelfC.scale = scale;

	bodyPos = target->GetCollision()[0].position[0];
	radius = target->GetCollision()[0].radius;
	scale = target->GetCollision()[0].scale;

	targetC.position = { bodyPos.x,bodyPos.z };
	targetC.radius   = radius;
	targetC.scale    = scale;

	return m_collision.JudgeCircleAndCircle(mySelfC, targetC);
}


bool CollisionJudge::JudgeSphereAndSphere(CharacterParameter::Collision& target,
	CharacterParameter::Collision& attackColl)
{
	Collision::Sphere mySelfC, targetC;
	mySelfC.position = attackColl.position[0];
	mySelfC.radius   = attackColl.radius;
	mySelfC.scale    = attackColl.scale;
	targetC.position = target.position[0];
	targetC.radius   = target.radius;
	targetC.scale    = target.scale;

	return m_collision.JudgeSphereAndSphere(mySelfC, targetC);
}

bool CollisionJudge::JudgeCapsuleAndSphere(CharacterParameter::Collision& target,
	CharacterParameter::Collision& attackColl)
{
	Collision::Capsule mySelfC;
	mySelfC.startPos = attackColl.position[0];
	mySelfC.endPos   = attackColl.position[1];
	mySelfC.radius   = attackColl.radius;
	mySelfC.scale    = attackColl.scale;
	Collision::Sphere targetC;
	targetC.position = target.position[0];
	targetC.radius   = target.radius;
	targetC.scale    = target.scale;

	return m_collision.JudgeCapsuleAndSphere(mySelfC, targetC);
}

bool CollisionJudge::JudgeCylinderAndCylinder(CharacterParameter::Collision& target,
	CharacterParameter::Collision& attackColl)
{
	Collision::Cylinder mySelfC, targetC;
	mySelfC.startPos = attackColl.position[0];
	mySelfC.endPos   = { attackColl.position[0].x,FLT_MAX,attackColl.position[0].z };
	mySelfC.radius   = attackColl.radius;
	mySelfC.scale    = attackColl.scale;
	targetC.startPos = target.position[0];
	targetC.endPos   = { target.position[0].x,FLT_MAX,target.position[0].z };
	targetC.radius   = target.radius;
	targetC.scale    = target.scale;

	return m_collision.JudgeCylinderAndCylinder(mySelfC, targetC);
}
