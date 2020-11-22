#include "EnemyNearAttack2Task.h"
#include "MessengTo.h"
#include "Enemy.h"

void EnemyNearAttack2Task::Run(Enemy* enemy)
{
#if 0
	auto& animation = enemy->GetBlendAnimation();
	switch (m_moveState)
	{
	case Action::START:
	
		m_taskState = TASK_STATE::RUN;
		if (animation.animationBlend.GetSampler().size() != 2)
		{
			m_animNo = Enemy::Animation::RIGHT_LOWER;
			animation.animationBlend.AddSampler(m_animNo, enemy->GetModel());
			animation.animationBlend.ResetAnimationFrame();
		}
		else
		{
			if (JudgeBlendRatio(animation))
			{
				++m_moveState;
				++enemy->GetJudgeElement().attackCount;
			}
		}

		break;
	case Action::RIGHT_PUNCH_LOWER:
	
		m_attackNo = static_cast<int>(Enemy::AttackType::RIGHT_LOWER);
		if (JudgeAnimationRatio(enemy, m_attackNo, m_animNo))
		{
			enemy->GetBlendAnimation().animationBlend.SetAnimationSpeed(1.0f);
			m_animNo = JudgeTurnChace(enemy);
			if (m_animNo <= 1)
			{
				m_animNo = Enemy::Animation::IDLE;
				animation.animationBlend.AddSampler(m_animNo, enemy->GetModel());
				animation.animationBlend.ResetAnimationFrame();
				m_moveState = Action::END;
				break;
			}
			animation.animationBlend.AddSampler(m_animNo, enemy->GetModel());
			animation.animationBlend.ResetAnimationFrame();
			++m_moveState;
		}

		JudgeAttack(enemy, m_attackNo);
	
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

	auto& animation = enemy->GetBlendAnimation();
	switch (m_moveState)
	{
	case Action::START:
		m_taskState = TASK_STATE::RUN;
		m_animNo = Enemy::Animation::FALL_FLAT_EDIT;
		m_blendValue = 0.2f;
		if (animation.animationBlend.SearchSampler(m_animNo))
		{
			if (JudgeBlendRatio(animation))
			{
				m_animNo = Enemy::Animation::STAND_UP;
				++enemy->GetJudgeElement().attackCount;
				++enemy->GetJudgeElement().moveCount;
				++m_moveState;
			}
		}
		else
		{
			animation.animationBlend.AddSampler(m_animNo, enemy->GetModel());
			animation.animationBlend.ResetAnimationFrame();
		}
		break;
	case Action::STAMP:
	{
		m_attackNo = Enemy::AttackType::FallFlat_edit;
		AttackMove(enemy);
		JudgeAttack(enemy, m_attackNo);
		if (JudgeAnimationRatio(enemy, m_attackNo, m_animNo))
		{
			m_isHit = false;
			++m_moveState;
		}
		break;
	}
	case Action::STAND_UP:
	{
		m_blendValue = 0.015f;
		static bool blendFinish = false;
		if (!blendFinish)
			blendFinish = JudgeBlendRatio(animation);
		else
		{
			animation.animationBlend.SetAnimationSpeed(1.5f);
			uint32_t currentAnimationTime = enemy->GetBlendAnimation().animationBlend.GetAnimationTime(0);
			if (currentAnimationTime >= 683)
			{
				blendFinish = false;
				animation.animationBlend.SetAnimationSpeed(1.0f);
				m_blendValue = 0.05f;

				m_animNo = JudgeTurnChace(enemy);
				if (m_animNo <= 1)
				{
					m_animNo = Enemy::Animation::IDLE;
					animation.animationBlend.AddSampler(m_animNo, enemy->GetModel());
					animation.animationBlend.ResetAnimationFrame();
					m_moveState = Action::END;
					break;
				}
				animation.animationBlend.AddSampler(m_animNo, enemy->GetModel());
				animation.animationBlend.ResetAnimationFrame();
				++m_moveState;
			}
		}

		break;
	}
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
			m_isUsed = true;
			animation.animationBlend.ResetAnimationSampler(0);
			m_coolTimer = 6.0f;
			m_moveState = Action::START;
			m_taskState = TASK_STATE::END;
		}
		break;
	}

}

bool EnemyNearAttack2Task::JudgeBlendRatio(CharacterParameter::BlendAnimation& animation, const bool isLoop)
{
	animation.animationBlend._blendRatio += m_blendValue;
	if (animation.animationBlend._blendRatio >= animation.blendRatioMax)//magicNumber
	{
		animation.animationBlend._blendRatio = 0.0f;
		animation.animationBlend.ResetAnimationSampler(0);
		animation.animationBlend.ReleaseSampler(0);
		if (!isLoop)
			animation.animationBlend.FalseAnimationLoop(0);
		return true;
	}

	return false;
}

void EnemyNearAttack2Task::JudgeAttack(Enemy* enemy, const int attackNo)
{
	uint32_t currentAnimationTime = enemy->GetBlendAnimation().animationBlend.GetAnimationTime(0);

	if (currentAnimationTime > kAttackSlowTimer)
	{
		enemy->GetBlendAnimation().animationBlend.SetAnimationSpeed(1.3f);
	}

	if (currentAnimationTime > kAttackTimer[0] && currentAnimationTime < kAttackTimer[1])
	{
		auto& attackParm = enemy->GetCollision().at(3);
		int attackMesh = attackParm.GetCurrentMesh(0);
		int attackBone = attackParm.GetCurrentBone(0);
		FLOAT4X4 getAttackBone = enemy->GetBlendAnimation().animationBlend._blendLocals[attackMesh].at(attackBone);
		FLOAT4X4 modelAxisTransform = enemy->GetModel()->_resource->axisSystemTransform;
		FLOAT4X4 worldTransform = enemy->GetWorldTransform().world;
		FLOAT4X4 attackTransform = getAttackBone * modelAxisTransform * worldTransform;

		attackParm.position[0] = { attackTransform._41,attackTransform._42,attackTransform._43 };
		enemy->GetStatus().attackPoint = enemy->GetAttack(attackNo).attackPoint;

		if (!m_isHit && MESSENGER.EnemyAttackingMessage(enemy->GetID(), attackParm))
			m_isHit = true;
	}
}

bool EnemyNearAttack2Task::JudgeAnimationRatio(Enemy* enemy, const int attackNo, const int nextAnimNo)
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

int EnemyNearAttack2Task::JudgeTurnChace(Enemy* enemy)
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

bool EnemyNearAttack2Task::IsTurnChase(Enemy* enemy)
{
	auto& animation = enemy->GetBlendAnimation();
	static bool blendFinish = false;
	if (!blendFinish)
		blendFinish = JudgeBlendRatio(animation, true);

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
	{
		blendFinish = false;
		return true;
	}

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

void EnemyNearAttack2Task::AttackMove(Enemy* enemy)
{
	uint32_t currentAnimationTime = enemy->GetBlendAnimation().animationBlend.GetAnimationTime(0);
	if (currentAnimationTime < kAttackSlowTimer)
	{
		auto& enemyTransform = enemy->GetWorldTransform();

		VECTOR3F angle = enemy->GetWorldTransform().angle;
		VECTOR3F front = VECTOR3F(sinf(angle.y), 0.0f, cosf(angle.y));
		front = NormalizeVec3(front);

		VECTOR3F velocity = front * kAccel;

		enemyTransform.position += velocity * enemy->GetElapsedTime();
		enemyTransform.WorldUpdate();
	}

}

uint32_t EnemyNearAttack2Task::JudgePriority(const int id, const VECTOR3F playerPos) 
{
	if (m_isUsed) return minPriority;

	auto player = MESSENGER.CallPlayersInstance();
	std::shared_ptr<CharacterAI> enemy = MESSENGER.CallEnemyInstance(id);
	int targetID = enemy->GetJudgeElement().targetID;

	VECTOR3F playerPosition = playerPos;
	VECTOR3F enemyPosition = enemy->GetWorldTransform().position;

	float direction = ToDistVec3(playerPosition - enemyPosition);
	VECTOR3F normalizeDist = NormalizeVec3(playerPosition - enemyPosition);

	VECTOR3F angle = enemy->GetWorldTransform().angle;
	VECTOR3F front = VECTOR3F(sinf(angle.y), 0.0f, cosf(angle.y));
	front = NormalizeVec3(front);

	float dot = DotVec3(front, normalizeDist);

	float cosTheta = acosf(dot);

	float frontValue = enemy->GetStandardValue().viewFrontValue;

	//if (cosTheta <= frontValue)
	//		return m_priority;

	uint32_t attackHitCount = enemy->GetJudgeElement().attackHitCount;
	uint32_t attackCount = enemy->GetJudgeElement().attackCount;
	float attackRatio = enemy->GetStandardValue().attackRatio;
	float ratio = static_cast<float>(attackHitCount) / static_cast<float>(attackCount);
	if (ratio <= attackRatio)
		return m_priority;

	if (direction >= kMinDirection)
		return m_priority;

	return minPriority;
}

