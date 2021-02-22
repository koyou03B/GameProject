#include "EnemyNearAttack3Task.h"
#include "MessengTo.h"
#include "Enemy.h"

void EnemyNearAttack3Task::Run(Enemy* enemy)
{
	auto& animation = enemy->GetBlendAnimation();
	switch (m_moveState)
	{
	case Action::START:
		m_taskState = TASK_STATE::RUN;
		m_animNo = Enemy::Animation::TURN_ATTACK_HEIGHT;
		m_hasFinishedBlend = false;
		m_isHit = false;
		if (animation.animationBlend.SearchSampler(m_animNo))
		{
			if (JudgeBlendRatio(animation))
			{
				++enemy->GetJudgeElement().attackCount;
				++m_moveState;
			}
		}
		else
		{
			animation.animationBlend.AddSampler(m_animNo, enemy->GetModel());
			animation.animationBlend.ResetAnimationFrame();
		}
		break;
	case Action::TURN_ATTACK:
		m_attackNo = Enemy::AttackType::TurnAttackLower;
		AttackMove(enemy);
		JudgeAttack(enemy, m_attackNo);
		if (JudgeAnimationRatio(enemy, m_attackNo, m_animNo))
		{
			m_isHit = false;
			animation.animationBlend.SetAnimationSpeed(1.0f);
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
			m_isUsed = true;
			animation.animationBlend.ResetAnimationSampler(0);
			m_coolTimer = 5.0f;
			m_moveState = Action::START;
			m_taskState = TASK_STATE::END;
		}
		break;
	}
}

bool EnemyNearAttack3Task::JudgeBlendRatio(CharacterParameter::BlendAnimation& animation, const bool isLoop)
{
	animation.animationBlend._blendRatio += kBlendValue;
	if (animation.animationBlend._blendRatio >= animation.blendRatioMax)//magicNumber
	{
		animation.animationBlend._blendRatio = 0.0f;
		size_t samplerSize = animation.animationBlend.GetSampler().size();
		for (size_t i = 0; i < samplerSize; ++i)
		{
			animation.animationBlend.ReleaseSampler(0);
		}
		if (!isLoop)
			animation.animationBlend.FalseAnimationLoop(0);
		return true;
	}

	return false;
}

void EnemyNearAttack3Task::JudgeAttack(Enemy* enemy, const int attackNo)
{
	uint32_t currentAnimationTime = enemy->GetBlendAnimation().animationBlend.GetAnimationTime(0);

	uint32_t attackTimer = 0;
	if (currentAnimationTime < kAnimationSpeed[0])
		enemy->GetBlendAnimation().animationBlend.SetAnimationSpeed(1.0f);
	else if (currentAnimationTime >= kAnimationSpeed[0] && currentAnimationTime < kAnimationSpeed[1])
		enemy->GetBlendAnimation().animationBlend.SetAnimationSpeed(1.4f);
	else
		enemy->GetBlendAnimation().animationBlend.SetAnimationSpeed(1.0f);

	attackTimer = kAnimationSpeed[0];

	if (currentAnimationTime > attackTimer && currentAnimationTime < kAnimationSpeed[1])
	{
		auto& attackParm = enemy->GetCollision().at(1);
		int attackMesh = attackParm.GetCurrentMesh(0);
		int attackBone = attackParm.GetCurrentBone(0);
		FLOAT4X4 getAttackBone = enemy->GetBlendAnimation().animationBlend._blendLocals[attackMesh].at(attackBone);
		FLOAT4X4 modelAxisTransform = enemy->GetModel()->_resource->axisSystemTransform;
		FLOAT4X4 worldTransform = enemy->GetWorldTransform().world;
		FLOAT4X4 attackTransform = getAttackBone * modelAxisTransform * worldTransform;

		attackParm.position[0] = { attackTransform._41,attackTransform._42,attackTransform._43 };
		enemy->GetStatus().attackPoint = enemy->GetAttack(attackNo).attackPoint;

		if (!m_isHit && MESSENGER.AttackingMessage(EnemyType::Boss, attackParm))
			m_isHit = true;
	}
}

bool EnemyNearAttack3Task::JudgeAnimationRatio(Enemy* enemy, const int attackNo, const int nextAnimNo)
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

int EnemyNearAttack3Task::JudgeTurnChace(Enemy* enemy)
{
	int targetID = enemy->GetJudgeElement().targetID;
	PlayerType type = static_cast<PlayerType>(targetID);
	CharacterAI* player = MESSENGER.CallPlayerInstance(type);
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

bool EnemyNearAttack3Task::IsTurnChase(Enemy* enemy)
{
	auto& animation = enemy->GetBlendAnimation();
	if (!m_hasFinishedBlend)
		m_hasFinishedBlend = JudgeBlendRatio(animation, true);

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
		m_hasFinishedBlend = false;
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

void EnemyNearAttack3Task::AttackMove(Enemy* enemy)
{
	uint32_t currentAnimationTime = enemy->GetBlendAnimation().animationBlend.GetAnimationTime(0);
	if (currentAnimationTime >= kRowlingTimer[0] && currentAnimationTime <= kRowlingTimer[1])
	{
		auto& enemyTransform = enemy->GetWorldTransform();

		int targetID = enemy->GetJudgeElement().targetID;
		PlayerType type = static_cast<PlayerType>(targetID);
		CharacterAI* player = MESSENGER.CallPlayerInstance(type);
		VECTOR3F targetPosition = player->GetWorldTransform().position;
		VECTOR3F targetDistance = targetPosition - enemyTransform.position;
		VECTOR3F targetNormal = NormalizeVec3(targetDistance);
		float targetDist = ToDistVec3(targetDistance);
		VECTOR3F angle = enemy->GetWorldTransform().angle;
		VECTOR3F front = VECTOR3F(sinf(angle.y), 0.0f, cosf(angle.y));
		front = NormalizeVec3(front);

		float dot = DotVec3(front, targetNormal);
		float rot = 1.0f - dot;
		float limit = enemy->GetMove().turnSpeed;
		if (rot > limit)
			rot = limit;

		VECTOR3F cross = CrossVec3(front, targetNormal);
		if (cross.y > 0.0f)
		{
			enemyTransform.angle.y += rot;
		}
		else
		{
			enemyTransform.angle.y -= rot;
		}

		if (currentAnimationTime >= kRowlingTimer[0] && currentAnimationTime <= kRowlingMoveTimer)
		{
			enemy->GetMove().velocity = targetNormal * kAccel;
			enemyTransform.position += enemy->GetMove().velocity * enemy->GetElapsedTime();
		}

		enemyTransform.WorldUpdate();
	}
}

uint32_t EnemyNearAttack3Task::JudgePriority(const int id, const VECTOR3F playerPos)
{
	if (m_isUsed) return minPriority;

	EnemyType type = static_cast<EnemyType>(id);
	CharacterAI* enemy = MESSENGER.CallEnemyInstance(type);

	uint32_t exhaustion = enemy->GetEmotion().exhaustionParm.exhaustionCost;
	uint32_t maxExhaustion = enemy->GetEmotion().exhaustionParm.maxExhaustionCost;

	if (exhaustion > static_cast<uint32_t>(maxExhaustion * 0.45f))
	{
		{
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

			if (cosTheta > frontValue)
				return m_priority;

			if (direction <= kMinDirection)
				return m_priority;
		}

		{
			uint32_t attackHitCount = enemy->GetJudgeElement().attackHitCount;
			uint32_t attackCount = enemy->GetJudgeElement().attackCount;
			float attackRatio = enemy->GetStandardValue().attackRatio;
			float ratio = static_cast<float>(attackHitCount) / static_cast<float>(attackCount);
			if (ratio <= attackRatio)
				return m_priority;
		}
	}

	return minPriority;
}

