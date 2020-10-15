#include "EnemyNearAttack1Task.h"
#include "MessengTo.h"
#include "Enemy.h"

//magicNumber
void EnemyNearAttack1Task::Run(Enemy* enemy)
{
	auto& animation = enemy->GetBlendAnimation();

	switch (m_moveState)
	{
	case 0:
	{
		m_taskState = TASK_STATE::START;
		animation.animationBlend.AddSampler(13, enemy->GetModel());
		animation.animationBlend.ResetAnimationFrame();
		++m_moveState;
		enemy->GetStatus().isAttack = false;
	}
	break;
	case 1:
	{
		m_taskState = TASK_STATE::RUN;
		if (JudgeBlendRatio(animation))
			++m_moveState;
	}
	break;
	case 2:
	{
		if (JudgeAnimationRatio(enemy, 5, 1))
			++m_moveState;
	}
	break;
	case 3:
	{
		enemy->GetStatus().isAttack = false;
		if (JudgeBlendRatio(animation))
		{
			animation.animationBlend.ResetAnimationSampler(0);
			m_moveState = 0;
			m_taskState = TASK_STATE::END;
		}
	}
	break;
	}

	bool isHitAttack = enemy->GetStatus().isAttack;
	if (m_moveState == 2 && !isHitAttack)
	{
		auto& rightPunch = enemy->GetCollision().at(1);
		int rightPunchMesh = rightPunch.GetCurrentMesh(0);
		int rightPunchBone = rightPunch.GetCurrentBone(0);
		FLOAT4X4 getAttackBone = animation.animationBlend._blendLocals[rightPunchMesh].at(rightPunchBone);
		FLOAT4X4 modelAxisTransform = enemy->GetModel()->_resource->axisSystemTransform;
		FLOAT4X4 worldTransform = enemy->GetWorldTransform().world;
		FLOAT4X4 AttackTransform = getAttackBone * modelAxisTransform * worldTransform;

		rightPunch.position[0] = { AttackTransform._41,AttackTransform._42,AttackTransform._43 };
		enemy->GetStatus().attackPoint = enemy->GetAttack(5).attackPoint;

		MESSENGER.EnemyAttackingMessage(enemy->GetID(), rightPunch);
	}
	uint32_t currentAnimationTime = enemy->GetBlendAnimation().animationBlend.GetAnimationTime(0);
	if (m_moveState == 2 && currentAnimationTime >= 30 && currentAnimationTime <= 120)
	{
		auto& enemyTransform = enemy->GetWorldTransform();

		auto& player = MESSENGER.CallPlayerInstance(enemy->GetJudgeElement().targetID);
		VECTOR3F targetPosition = player->GetWorldTransform().position;
		VECTOR3F targetDistance = targetPosition - enemyTransform.position;
		VECTOR3F targetNormal = NormalizeVec3(targetDistance);
		VECTOR3F angle = enemy->GetWorldTransform().angle;
		VECTOR3F front = VECTOR3F(sinf(angle.y), 0.0f, cosf(angle.y));
		front = NormalizeVec3(front);

		float dot = DotVec3(front, targetNormal);
		dot = acosf(dot);
		dot /= 60;

		VECTOR3F cross = CrossVec3(front, targetNormal);
		if (cross.y > 0.0f)
		{
			enemyTransform.angle.y += dot;
		}
		else
		{
			enemyTransform.angle.y -= dot;
		}

		enemyTransform.WorldUpdate();
	}
}

bool EnemyNearAttack1Task::JudgeBlendRatio(CharacterParameter::BlendAnimation& animation)
{
	m_taskState = TASK_STATE::RUN;
	animation.animationBlend._blendRatio += 0.045f;//magicNumber
	if (animation.animationBlend._blendRatio >= animation.blendRatioMax)//magicNumber
	{
		animation.animationBlend._blendRatio = 0.0f;
		animation.animationBlend.ResetAnimationSampler(0);
		animation.animationBlend.ReleaseSampler(0);
		animation.animationBlend.FalseAnimationLoop(0);
		return true;
	}

	return false;
}

bool EnemyNearAttack1Task::JudgeAnimationRatio(Enemy* enemy, const int attackNo, const int nextAnimNo)
{
	uint32_t currentAnimationTime = enemy->GetBlendAnimation().animationBlend.GetAnimationTime(0);
	uint32_t attackFrameCount = enemy->GetAttack(attackNo).frameCount;
	if (currentAnimationTime >= attackFrameCount)
	{
		enemy->GetBlendAnimation().animationBlend.AddSampler(nextAnimNo, enemy->GetModel());
		return true;
	}

	return false;
}

uint32_t EnemyNearAttack1Task::JudgePriority(const int id)
{
	auto player = MESSENGER.CallPlayersInstance();
	std::shared_ptr<CharacterAI> enemy = MESSENGER.CallEnemyInstance(id);

	int targetID = enemy->GetJudgeElement().targetID;

	VECTOR3F playerPosition = player.at(targetID)->GetWorldTransform().position;
	VECTOR3F enemyPosition = enemy->GetWorldTransform().position;

	float direction = ToDistVec3(playerPosition - enemyPosition);
	VECTOR3F normalizeDist = NormalizeVec3(playerPosition - enemyPosition);

	VECTOR3F angle = player.at(targetID)->GetWorldTransform().angle;
	VECTOR3F front = VECTOR3F(sinf(angle.y), 0.0f, cosf(angle.y));
	front = NormalizeVec3(front);

	float dot = DotVec3(front, normalizeDist);

	float cosTheta = acosf(dot);

	float frontValue = enemy->GetStandardValue().viewFrontValue;

	if (dot > frontValue)
		return m_priority;

	return minPriority;
}
