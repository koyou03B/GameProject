#include "EnemyFarAttack1Task.h"
#include "MessengTo.h"
#include "Enemy.h"

void EnemyFarAttack1Task::Run(Enemy* enemy)
{
	auto& animation = enemy->GetBlendAnimation();

	switch (m_moveState)
	{
	case Action::START:
		m_taskState = TASK_STATE::RUN;
		if (animation.animationBlend.GetSampler().size() != 2)
		{
			animation.animationBlend.AddSampler(Enemy::Animation::RUN_SIGNAL, enemy->GetModel());
			animation.animationBlend.ResetAnimationFrame();
		}
		else
		{
			auto player = MESSENGER.CallPlayerInstance(m_targetID);
			m_targetPosition = player->GetWorldTransform().position;
			++m_moveState;
		}
		m_nVecToTarget = {};
		m_speedToTarget = {};
		m_chaseTimer = 0;

		m_isNear = false;
		m_isTurning = false;
	
		break;
	case Action::TURNING:
	
		if (JudgeBlendRatio(animation))
			m_isTurning = true;

		if (m_isTurning)
		{
			uint32_t currentAnimationTime = enemy->GetBlendAnimation().animationBlend.GetAnimationTime(0);
			if (currentAnimationTime >= kTurningTimer)
			{
				enemy->GetBlendAnimation().animationBlend.AddSampler(Enemy::Animation::RUN, enemy->GetModel());
				++m_moveState;
			}
		}
		TurningChase(enemy);

		break;
	case Action::RUN:
		if (animation.animationBlend.GetSampler().size() == 2)
		{
			animation.animationBlend._blendRatio += kBlendValue;
			if (animation.animationBlend._blendRatio >= animation.blendRatioMax)
			{
				animation.animationBlend._blendRatio = 0.0f;
				animation.animationBlend.ResetAnimationSampler(0);
				animation.animationBlend.ReleaseSampler(0);
			}
		}

		if (m_isNear)
		{
			animation.animationBlend.AddSampler(Enemy::Animation::RIGHT_UPPER, enemy->GetModel());
			++m_moveState;
			AIParameter::JudgeElement judgeElement = enemy->GetJudgeElement();
			++judgeElement.attackCount;
			++judgeElement.moveCount;
		}
		else
		{
			auto& enemyTransform = enemy->GetWorldTransform();

			if (m_chaseTimer <= kChaseMaxTimer)
			{
				enemy->GetMove().velocity = m_nVecToTarget * m_speedToTarget;
				enemyTransform.position += enemy->GetMove().velocity;
				enemyTransform.WorldUpdate();
				++m_chaseTimer;
				m_attackNo = static_cast<int>(Enemy::AttackType::RUN_ATTACK);
				JudgeAttack(enemy, m_attackNo);
			}
			else
			{
				m_isNear = true;
				m_chaseTimer = 0;
			}
		}
		break;
	case Action::BLNEDNOW:
		if (JudgeBlendRatio(animation))
		{
			++m_moveState;
			AIParameter::JudgeElement judgeElement = enemy->GetJudgeElement();
			++judgeElement.attackCount;
		}
		break;
	case Action::RIGTH_UPPER:
		m_attackNo = static_cast<int>(Enemy::AttackType::RIGHT_UPPER);
		if (JudgeAnimationRatio(enemy, m_attackNo, Enemy::Animation::IDLE))
			++m_moveState;

		JudgeAttack(enemy, m_attackNo);
		break;
	case Action::END:
		if (JudgeBlendRatio(animation))
		{
			animation.animationBlend.ResetAnimationSampler(0);
			animation.animationBlend._blendRatio = 0.0f;
			m_moveState = Action::START;
			m_taskState = TASK_STATE::END;
		}

		break;
	}


}


bool EnemyFarAttack1Task::JudgeBlendRatio(CharacterParameter::BlendAnimation& animation)
{
	m_taskState = TASK_STATE::RUN;
	if (animation.animationBlend.GetSampler().size() == 1) return false;

	animation.animationBlend._blendRatio += 0.025f;//magicNumber
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

bool EnemyFarAttack1Task::JudgeAnimationRatio(Enemy* enemy, const int attackNo, const int nextAnimNo)
{
	uint32_t currentAnimationTime = enemy->GetBlendAnimation().animationBlend.GetAnimationTime(0);
	uint32_t attackFrameCount = enemy->GetAttack(attackNo).frameCount;
	if (currentAnimationTime >= attackFrameCount)
	{
		enemy->GetBlendAnimation().animationBlend.AddSampler(nextAnimNo, enemy->GetModel());

		//enemy->GetBlendAnimation().animationBlend.ResetAnimationFrame();
		return true;
	}

	return false;
}

void EnemyFarAttack1Task::JudgeAttack(Enemy* enemy, const int attackNo)
{
	uint32_t currentAnimationTime = enemy->GetBlendAnimation().animationBlend.GetAnimationTime(0);

	if (attackNo == static_cast<int>(Enemy::AttackType::RIGHT_UPPER))
	{
		if (currentAnimationTime > kAttackTimer[0] && currentAnimationTime < kAttackTimer[1])
		{
			auto& attackParm = enemy->GetCollision().at(kCollisionNo[0]);
			int attackMesh = attackParm.GetCurrentMesh(0);
			int attackBone = attackParm.GetCurrentBone(0);
			FLOAT4X4 getAttackBone = enemy->GetBlendAnimation().animationBlend._blendLocals[attackMesh].at(attackBone);
			FLOAT4X4 modelAxisTransform = enemy->GetModel()->_resource->axisSystemTransform;
			FLOAT4X4 worldTransform = enemy->GetWorldTransform().world;
			FLOAT4X4 attackTransform = getAttackBone * modelAxisTransform * worldTransform;

			attackParm.position[0] = { attackTransform._41,attackTransform._42,attackTransform._43 };
			enemy->GetStatus().attackPoint = enemy->GetAttack(attackNo).attackPoint;

			MESSENGER.EnemyAttackingMessage(enemy->GetID(), attackParm);
		}
	}
	else
	{
		auto& attackParm = enemy->GetCollision().at(kCollisionNo[1]);
		int attackMesh = attackParm.GetCurrentMesh(0);
		int attackBone = attackParm.GetCurrentBone(0);
		FLOAT4X4 getAttackBone = enemy->GetBlendAnimation().animationBlend._blendLocals[attackMesh].at(attackBone);
		FLOAT4X4 modelAxisTransform = enemy->GetModel()->_resource->axisSystemTransform;
		FLOAT4X4 worldTransform = enemy->GetWorldTransform().world;
		FLOAT4X4 attackTransform = getAttackBone * modelAxisTransform * worldTransform;

		attackParm.position[0] = { attackTransform._41,attackTransform._42,attackTransform._43 };
		enemy->GetStatus().attackPoint = enemy->GetAttack(attackNo).attackPoint;
		MESSENGER.EnemyAttackingMessage(enemy->GetID(), attackParm);
	}
}

void EnemyFarAttack1Task::TurningChase(Enemy* enemy)
{
	auto& enemyTransform = enemy->GetWorldTransform();

	auto& player = MESSENGER.CallPlayerInstance(m_targetID);
	VECTOR3F targetPosition = player->GetWorldTransform().position;
	VECTOR3F targetDistance = targetPosition - enemyTransform.position;
	m_nVecToTarget = NormalizeVec3(targetDistance);
	float targetDist = ToDistVec3(targetDistance);
	m_speedToTarget = targetDist / kFourSecond;
	VECTOR3F angle = enemy->GetWorldTransform().angle;
	VECTOR3F front = VECTOR3F(sinf(angle.y), 0.0f, cosf(angle.y));
	front = NormalizeVec3(front);

	float dot = DotVec3(front, m_nVecToTarget);
	float rot = 1.0f - dot;
	float limit = enemy->GetMove().turnSpeed;
	if (rot > limit)
	{
		rot = limit;
	}

	VECTOR3F cross = CrossVec3(front, m_nVecToTarget);
	if (cross.y > 0.0f)
	{
		enemyTransform.angle.y += rot;
	}
	else
	{
		enemyTransform.angle.y -= rot;
	}

	enemyTransform.WorldUpdate();
}

uint32_t EnemyFarAttack1Task::JudgePriority(const int id)
{
	std::shared_ptr<CharacterAI> enemy = MESSENGER.CallEnemyInstance(id);
	uint32_t attackCount = enemy->GetJudgeElement().attackCount;
	uint32_t attackCountValue = enemy->GetStandardValue().attackCountValue;

	if (attackCount > attackCountValue)
		return m_priority;

	return minPriority;
}
