#include "EnemyFarAttack2Task.h"
#include "MessengTo.h"
#include "Enemy.h"
#include "Stone.h"

void EnemyFarAttack2Task::Run(Enemy* enemy)
{
	auto& animation = enemy->GetBlendAnimation();

	switch (m_moveState)
	{
	case Action::START:		
		m_taskState = TASK_STATE::RUN;
		m_hasFinishedBlend = false;

		if (m_animNo != Enemy::Animation::RIGHT_PUNCH_UPPER)
		{
			if(JudgeLookTarget(enemy))
				m_animNo = Enemy::Animation::RIGHT_PUNCH_UPPER;
			else
			{
				animation.animationBlend.AddSampler(m_animNo, enemy->GetModel());
				animation.animationBlend.ResetAnimationFrame();
				++m_moveState;
				return;
			}
		}

		if (animation.animationBlend.SearchSampler(m_animNo))
		{
			if (JudgeBlendRatio(animation))
			{
				++enemy->GetJudgeElement().attackCount;
				m_moveState = Action::RPUNCH_UPPER;
			}
		}
		else
		{
			animation.animationBlend.AddSampler(m_animNo, enemy->GetModel());
			animation.animationBlend.ResetAnimationFrame();
		}
		break;
	case Action::LOOK_TARGET:
		if (IsTurnChase(enemy))
		{
			m_hasFinishedBlend = false;
			m_animNo = Enemy::Animation::RIGHT_PUNCH_UPPER;
			animation.animationBlend.AddSampler(m_animNo, enemy->GetModel());
			animation.animationBlend.ResetAnimationFrame();
			++m_moveState;
		}
		break;
	case Action::RPUNCH_UPPER:
	{
		enemy->GetStatus().attackPoint = 10.0f;
		m_attackNo = Enemy::AttackType::RightPunchUpper;
		size_t samplerSize = animation.animationBlend.GetSampler().size();
		if (samplerSize != 1)
			m_hasFinishedBlend = JudgeBlendRatio(animation);
		else
		{
			JudgeAttack(enemy, m_attackNo);
			PrepareForStone(enemy);
			if (JudgeAnimationRatio(enemy, m_attackNo, m_animNo))
			{
				m_isHit = false;
				m_hasFinishedBlend = false;
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
		}
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
			m_coolTimer = 6.0f;
			m_moveState = Action::START;
			m_taskState = TASK_STATE::END;
		}
		break;
	}
}

bool EnemyFarAttack2Task::JudgeBlendRatio(CharacterParameter::BlendAnimation& animation, const bool isLoop)
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

void EnemyFarAttack2Task::JudgeAttack(Enemy* enemy, const int attackNo)
{
	uint32_t currentAnimationTime = enemy->GetBlendAnimation().animationBlend.GetAnimationTime(0);

	if(currentAnimationTime < kAttackTimer[0])
		enemy->GetBlendAnimation().animationBlend.SetAnimationSpeed(0.8f);
	else
		enemy->GetBlendAnimation().animationBlend.SetAnimationSpeed(1.0f);


	//if (currentAnimationTime > kAttackTimer[0] && currentAnimationTime < kAttackTimer[1])
	//{
	//	auto& attackParm = enemy->GetCollision().at(1);
	//	int attackMesh = attackParm.GetCurrentMesh(0);
	//	int attackBone = attackParm.GetCurrentBone(0);
	//	FLOAT4X4 getAttackBone = enemy->GetBlendAnimation().animationBlend._blendLocals[attackMesh].at(attackBone);
	//	FLOAT4X4 modelAxisTransform = enemy->GetModel()->_resource->axisSystemTransform;
	//	FLOAT4X4 worldTransform = enemy->GetWorldTransform().world;
	//	FLOAT4X4 attackTransform = getAttackBone * modelAxisTransform * worldTransform;

	//	attackParm.position[0] = { attackTransform._41,attackTransform._42,attackTransform._43 };
	//	enemy->GetStatus().attackPoint = enemy->GetAttack(attackNo).attackPoint;

	//	if (!m_isHit && MESSENGER.EnemyAttackingMessage(enemy->GetID(), attackParm))
	//		m_isHit = true;

	//}

}

bool EnemyFarAttack2Task::JudgeAnimationRatio(Enemy* enemy, const int attackNo, const int nextAnimNo)
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

bool EnemyFarAttack2Task::JudgeLookTarget(Enemy* enemy)
{
	int targetID = enemy->GetJudgeElement().targetID;
	auto& player = MESSENGER.CallPlayerInstance(targetID);
	VECTOR3F playerPos = player->GetWorldTransform().position;
	VECTOR3F enemyPosition = enemy->GetWorldTransform().position;

	float direction = ToDistVec3(playerPos - enemyPosition);
	VECTOR3F normalizeDist = NormalizeVec3(playerPos - enemyPosition);

	VECTOR3F angle = enemy->GetWorldTransform().angle;
	VECTOR3F front = VECTOR3F(sinf(angle.y), 0.0f, cosf(angle.y));
	front = NormalizeVec3(front);

	float dot = DotVec3(front, normalizeDist);

	float cosTheta = acosf(dot);

	float frontValue = enemy->GetStandardValue().viewFrontValue;

	if (cosTheta <= frontValue)
		return true;
	
	VECTOR3F cross = CrossVec3(front, normalizeDist);
	if (cross.y > 0.0f)
		m_animNo = Enemy::Animation::RIGHT_TURN;
	else
		m_animNo = Enemy::Animation::LEFT_TURN;

	return false;
}

int EnemyFarAttack2Task::JudgeTurnChace(Enemy* enemy)
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

void EnemyFarAttack2Task::PrepareForStone(Enemy* enemy)
{
	uint32_t currentAnimationTime = enemy->GetBlendAnimation().animationBlend.GetAnimationTime(0);
	if (currentAnimationTime == kAttackTimer[0])
	{
		auto& stone = enemy->GetStone();
		FLOAT4X4 world = enemy->GetWorldTransform().world;
		VECTOR3F pos = { world._41,world._42,world._43 };
		VECTOR3F zAxis = { world._31,world._32,world._33 };
		VECTOR3F xAxis = { world._11,world._12,world._13 };
		zAxis = NormalizeVec3(zAxis);
		xAxis = NormalizeVec3(xAxis);
		pos += zAxis * stone->GetOffsetZ();
		pos += xAxis * stone->GetOffsetX();

		int targetID = enemy->GetJudgeElement().targetID;
		auto& player = MESSENGER.CallPlayerInstance(targetID);
		VECTOR3F playerPos = player->GetWorldTransform().position;
		VECTOR3F normalizeDist = NormalizeVec3(playerPos - pos);
		stone->PrepareForStone(pos, VECTOR3F(0.0f, 0.0f, 0.0f), normalizeDist);
		VECTOR3F dist = SphereLinearVec3(normalizeDist, xAxis, 0.1f);
		stone->PrepareForStone(pos, VECTOR3F(0.0f, 0.0f, 0.0f), dist);
		xAxis *= -1.0f;
		dist = SphereLinearVec3(normalizeDist, xAxis, 0.1f);
		stone->PrepareForStone(pos, VECTOR3F(0.0f, 0.0f, 0.0f), dist);
	}
}

void EnemyFarAttack2Task::TurningChase(Enemy* enemy)
{
	auto& enemyTransform = enemy->GetWorldTransform();
	int targetID = enemy->GetJudgeElement().targetID;
	auto& player = MESSENGER.CallPlayerInstance(targetID);
	VECTOR3F targetPosition = player->GetWorldTransform().position;
	VECTOR3F targetDistance = targetPosition - enemyTransform.position;
	VECTOR3F nVecToTarget = NormalizeVec3(targetDistance);

	VECTOR3F angle = enemy->GetWorldTransform().angle;
	VECTOR3F front = VECTOR3F(sinf(angle.y), 0.0f, cosf(angle.y));
	front = NormalizeVec3(front);

	float dot = DotVec3(front, nVecToTarget);
	float rot = 1.0f - dot;
	float limit = enemy->GetMove().turnSpeed;
	if (rot > limit)
	{
		rot = limit;
	}

	VECTOR3F cross = CrossVec3(front, nVecToTarget);
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

bool EnemyFarAttack2Task::IsTurnChase(Enemy* enemy)
{
	auto& animation = enemy->GetBlendAnimation();
	if (!m_hasFinishedBlend)
		m_hasFinishedBlend = JudgeBlendRatio(animation, true);

	VECTOR3F enemyPosition = enemy->GetWorldTransform().position;
	int targetID = enemy->GetJudgeElement().targetID;
	auto& player = MESSENGER.CallPlayerInstance(targetID);
	m_targetPosition = player->GetWorldTransform().position;
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

uint32_t EnemyFarAttack2Task::JudgePriority(const int id, const VECTOR3F playerPos)
{
	if (m_isUsed) return minPriority;

	std::shared_ptr<CharacterAI> enemy = MESSENGER.CallEnemyInstance(id);
	//uint32_t exhaustion = enemy->GetEmotion().exhaustionParm.exhaustionCost;
	//uint32_t maxExhaustion = enemy->GetEmotion().exhaustionParm.maxExhaustionCost;

	//if (exhaustion > static_cast<uint32_t>(maxExhaustion * 0.65f))
	//{
	//	auto player = MESSENGER.CallPlayersInstance();
	//	int targetID = enemy->GetJudgeElement().targetID;

	//	uint32_t damageCount = enemy->GetJudgeElement().damageCount;
	//	if (damageCount >= kDamageRatio)
	//		return m_priority;
	//}

	VECTOR3F playerPosition = playerPos;
	VECTOR3F enemyPosition = enemy->GetWorldTransform().position;

	float direction = ToDistVec3(playerPosition - enemyPosition);
	if (direction > kMinDirection)
		return m_priority;

	return minPriority;

}
