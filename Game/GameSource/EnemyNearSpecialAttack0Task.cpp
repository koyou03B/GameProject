#include "EnemyNearSpecialAttack0Task.h"
#include "Enemy.h"

void EnemyNearSpecialAttack0Task::Run(Enemy* enemy)
{
#if 0
	auto& animation = enemy->GetBlendAnimation();

	switch (m_moveState)
	{
	case Action::START:
		m_taskState = TASK_STATE::RUN;
		if (animation.animationBlend.GetSampler().size() != 2)
		{
			m_animNo = Enemy::Animation::WRATH_ATTACK;
			animation.animationBlend.AddSampler(m_animNo, enemy->GetModel());
			animation.animationBlend.ResetAnimationFrame();
		}
		else
		{
			if (JudgeBlendRatio(animation))
			{
				++m_moveState;
				++enemy->GetJudgeElement().attackCount;
				m_animNo = Enemy::Animation::IDLE;
			}
		}

		m_restTimer = 0.0f;
		break;
	case Action::WRATH_ATTACK:
		m_attackNo = static_cast<int>(Enemy::AttackType::WRATH_ATTACK);
		if (JudgeAnimationRatio(enemy, m_attackNo, m_animNo))
			++m_moveState;

		JudgeAttack(enemy, m_attackNo);
		break;
	case Action::ANIM_CHANGE:
		if (JudgeBlendRatio(animation))
		{
			animation.animationBlend.ResetAnimationSampler(0);
			animation.animationBlend.SetAnimationSpeed(3.0f);
			++m_moveState;
		}
		break;
	case Action::REST:
		if (m_restTimer >= kRestTimer)
		{
			AIParameter::JudgeElement judgeElement = enemy->GetJudgeElement();
			++judgeElement.moveCount;

			m_animNo = JudgeTurnChace(enemy);
			if (m_animNo == 0)
			{
				animation.animationBlend.ResetAnimationSampler(0);
				m_moveState = Action::START;
				m_taskState = TASK_STATE::END;

				break;
			}
			animation.animationBlend.AddSampler(m_animNo, enemy->GetModel());
			animation.animationBlend.ResetAnimationFrame();
			animation.animationBlend.SetAnimationSpeed(1.0f);
			++m_moveState;
		}
		else
			m_restTimer += enemy->GetElapsedTime();
		break;
	case Action::TURN_CHACE:
		if (IsTurnChase(enemy))
		{
			m_animNo = Enemy::Animation::IDLE;
			animation.animationBlend.AddSampler(m_animNo, enemy->GetModel());
			animation.animationBlend.ResetAnimationFrame();
			++m_moveState;
		}
		break;
	case Action::END:
		if (JudgeBlendRatio(animation))
		{
			animation.animationBlend.ResetAnimationSampler(0);
			m_moveState = Action::START;
			m_taskState = TASK_STATE::END;
		}
		break;
	}
#endif
}

bool EnemyNearSpecialAttack0Task::JudgeBlendRatio(CharacterParameter::BlendAnimation& animation)
{
	m_taskState = TASK_STATE::RUN;
	animation.animationBlend._blendRatio += kBlendValue;//magicNumber
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

bool EnemyNearSpecialAttack0Task::JudgeAnimationRatio(Enemy* enemy, const int attackNo, const int nextAnimNo)
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

bool EnemyNearSpecialAttack0Task::IsTurnChase(Enemy* enemy)
{
	auto& animation = enemy->GetBlendAnimation();

	if (animation.animationBlend.GetSampler().size() == 2)
	{
		animation.animationBlend._blendRatio += kBlendValue;//magicNumber
		if (animation.animationBlend._blendRatio >= animation.blendRatioMax)//magicNumber
		{
			animation.animationBlend._blendRatio = 0.0f;
			animation.animationBlend.ResetAnimationSampler(0);
			animation.animationBlend.ReleaseSampler(0);
		}
	}

	VECTOR3F enemyPosition = enemy->GetWorldTransform().position;

	float direction = ToDistVec3(m_targetPosition - enemyPosition);
	VECTOR3F normalizeDist = NormalizeVec3(m_targetPosition - enemyPosition);

	VECTOR3F angle = enemy->GetWorldTransform().angle;
	VECTOR3F front = VECTOR3F(sinf(angle.y), 0.0f, cosf(angle.y));
	front = NormalizeVec3(front);

	float dot = DotVec3(front, normalizeDist);
	float rot = 1.0f - dot;
	float limit = enemy->GetMove().turnSpeed;
	if (rot > limit)
		rot = limit;

	uint32_t currentAnimationTime = enemy->GetBlendAnimation().animationBlend.GetAnimationTime(0);
	float cosTheta = acosf(dot);
	float frontValue = enemy->GetStandardValue().viewFrontValue;
	if (cosTheta <= frontValue && currentAnimationTime == 0)
		return true;
	if (kTurnChanseTimer > currentAnimationTime)
	{
		auto& enemyTransform = enemy->GetWorldTransform();
		if (m_animNo == Enemy::Animation::LEFT_TURN)
			rot *= -1;

		enemyTransform.angle.y += rot;
		enemyTransform.WorldUpdate();
	}

	return false;
}

int EnemyNearSpecialAttack0Task::JudgeTurnChace(Enemy* enemy)
{
	int targetID = enemy->GetJudgeElement().targetID;
	auto& player = MESSENGER.CallPlayerInstance(targetID);

	m_targetPosition = player->GetWorldTransform().position;
	VECTOR3F enemyPosition = enemy->GetWorldTransform().position;

	float direction = ToDistVec3(m_targetPosition - enemyPosition);
	VECTOR3F normalizeDist = NormalizeVec3(m_targetPosition - enemyPosition);

	VECTOR3F angle = enemy->GetWorldTransform().angle;
	VECTOR3F front = VECTOR3F(sinf(angle.y), 0.0f, cosf(angle.y));
	front = NormalizeVec3(front);

	float dot = DotVec3(front, normalizeDist);

	float cosTheta = acosf(dot);
	float frontValue = enemy->GetStandardValue().viewFrontValue;
	if (cosTheta <= frontValue)
		return Enemy::Animation::IDLE;
	else
	{
		VECTOR3F cross = CrossVec3(front, normalizeDist);
		if (cross.y > 0.0f)
			return Enemy::Animation::RIGHT_TURN;
		else
			return Enemy::Animation::LEFT_TURN;
	}

	return 0;
}

void EnemyNearSpecialAttack0Task::JudgeAttack(Enemy* enemy, const int attackNo)
{
	uint32_t currentAnimationTime = enemy->GetBlendAnimation().animationBlend.GetAnimationTime(0);

	if (currentAnimationTime >= kAttackTimer[0] && currentAnimationTime <= kAttackTimer[1])
	{
		auto& attackParm = enemy->GetCollision().at(kCollisionNo);
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

uint32_t EnemyNearSpecialAttack0Task::JudgePriority(const int id, const VECTOR3F playerPos) 
{
	std::shared_ptr<CharacterAI> enemy = MESSENGER.CallEnemyInstance(id);
	bool isWrath = enemy->GetEmotion().wrathParm.isWrath;
	if (!isWrath)
		return minPriority;

	return m_priority;
}
