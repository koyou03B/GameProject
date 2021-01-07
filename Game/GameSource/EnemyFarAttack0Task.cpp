#include "EnemyFarAttack0Task.h"
#include "MessengTo.h"
#include "Enemy.h"
#include "Collision.h"

void EnemyFarAttack0Task::Run(Enemy* enemy)
{
	auto& animation = enemy->GetBlendAnimation();

	switch (m_moveState)
	{
	case Action::START:
		m_taskState = TASK_STATE::RUN;
		m_animNo = Enemy::Animation::BACK_FLIP;
		m_hasFinishedBlend = false;
		m_hasFinishedStamp = false;
		m_isHit = false;
		m_isNear = false;
		m_setTarget = false;
		if (animation.animationBlend.SearchSampler(m_animNo))
		{
			m_blendValue = 0.05f;
			if (JudgeBlendRatio(animation))
			{
				m_animNo = Enemy::Animation::RUN;
				enemy->GetJudgeElement().moveCount += 2;
				JudgeVectorDirection(enemy);
				++m_moveState;
			}
		}
		else
		{
			animation.animationBlend.AddSampler(m_animNo, enemy->GetModel());
			animation.animationBlend.ResetAnimationFrame();
		}
		break;
	case Action::BACK_FLIP:
	{
		//		animation.animationBlend.SetAnimationSpeed(1.5f);
		uint32_t currentAnimationTime = enemy->GetBlendAnimation().animationBlend.GetAnimationTime(0);
		if (currentAnimationTime >= 129)
		{
			//	animation.animationBlend.SetAnimationSpeed(1.0f);
			m_animNo = Enemy::Animation::RUN;
			animation.animationBlend.AddSampler(m_animNo, enemy->GetModel());
			animation.animationBlend.ResetAnimationFrame();
			enemy->GetJudgeElement().moveCount+=2;
			enemy->GetJudgeElement().attackCount+=2;
			++m_moveState;
		}
		BackFlipTurn(enemy);
		break;
	}
	case Action::RUN_STAMP:
		m_attackNo = Enemy::AttackType::FallFlat_edit;
		if (!m_hasFinishedBlend)
			m_hasFinishedBlend = JudgeBlendRatio(animation, true);
		else
		{
			if (AttackMove(enemy))
			{
				m_animNo = Enemy::Animation::FALL_FLAT_EDIT;
				animation.animationBlend.AddSampler(m_animNo, enemy->GetModel());
				animation.animationBlend.ResetAnimationFrame();
				m_blendValue = 0.2f;
			}

			if (m_isNear)
			{
				if(!m_hasFinishedStamp)
					m_hasFinishedStamp = JudgeBlendRatio(animation);
				else
				{
					//m_animNo = ;
					if (JudgeAnimationRatio(enemy, m_attackNo, Enemy::Animation::STAND_UP))
					{
						m_hasFinishedBlend = false;
						m_hasFinishedStamp = false;
						m_isHit = false;
						m_isNear = false;
						m_setTarget = false;
						++m_moveState;
					}
				}
			}
			JudgeAttack(enemy, m_attackNo);
		}
		break;
	case Action::STAND_UP:
		m_blendValue = 0.015f;
		if (!m_hasFinishedBlend)
			m_hasFinishedBlend = JudgeBlendRatio(animation);
		else
		{
			animation.animationBlend.SetAnimationSpeed(1.7f);
			uint32_t currentAnimationTime = enemy->GetBlendAnimation().animationBlend.GetAnimationTime(0);
			if (currentAnimationTime >= 683)
			{
				m_blendValue = 0.05f;
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
			m_coolTimer = 9.0f;
			m_moveState = Action::START;
			m_taskState = TASK_STATE::END;
		}
		break;
	}

}


bool EnemyFarAttack0Task::JudgeBlendRatio(CharacterParameter::BlendAnimation& animation, const bool isLoop)
{
	animation.animationBlend._blendRatio += m_blendValue;
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

void EnemyFarAttack0Task::JudgeAttack(Enemy* enemy, const int attackNo)
{
	uint32_t currentAnimationTime = enemy->GetBlendAnimation().animationBlend.GetAnimationTime(0);
	uint32_t attackTimer[2] = {};
	if (m_animNo == Enemy::Animation::RUN)
	{
		attackTimer[0] = attackTimer[1] = 100;
	}
	else if (m_animNo == Enemy::Animation::FALL_FLAT_EDIT)
	{
		if (currentAnimationTime > kAttackSlowTimer)
		{
			enemy->GetBlendAnimation().animationBlend.SetAnimationSpeed(1.3f);
		}
		attackTimer[0] = kAttackTimer[0];
		attackTimer[1] = kAttackTimer[1];
	}

	if (currentAnimationTime > attackTimer[0] && currentAnimationTime < attackTimer[1])
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

bool EnemyFarAttack0Task::JudgeAnimationRatio(Enemy* enemy, const int attackNo, const int nextAnimNo)
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

void EnemyFarAttack0Task::BackFlipTurn(Enemy* enemy)
{
	uint32_t currentAnimationTime = enemy->GetBlendAnimation().animationBlend.GetAnimationTime(0);
	auto& enemyTransform = enemy->GetWorldTransform();

	if (currentAnimationTime < 40)
	{
		enemy->GetMove().velocity = m_nVecToTarget * kAccel;
		VECTOR3F targetPoint = enemyTransform.position + 
			enemy->GetMove().velocity * enemy->GetElapsedTime();

		auto& player = MESSENGER.CallPlayerInstance(enemy->GetJudgeElement().targetID);
		VECTOR3F targetDistance = enemyTransform.position - targetPoint;
		VECTOR3F targetNormal = NormalizeVec3(targetDistance);
		float targetDist = ToDistVec3(targetDistance);
		VECTOR3F angle = enemy->GetWorldTransform().angle;
		VECTOR3F front = VECTOR3F(sinf(angle.y), 0.0f, cosf(angle.y));
		front = NormalizeVec3(front);

		float dot = DotVec3(front, targetNormal);
		float rot = 1.0f - dot;
		float limit = enemy->GetMove().turnSpeed*2.0f;
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

		enemyTransform.WorldUpdate();
	}
	else if (currentAnimationTime >= 40 && currentAnimationTime <= 70) 
	{
		enemy->GetMove().velocity = m_nVecToTarget * kAccel;
		enemyTransform.position += enemy->GetMove().velocity * enemy->GetElapsedTime();
		enemyTransform.WorldUpdate();
	}
	else
	{
		auto& player = MESSENGER.CallPlayerInstance(enemy->GetJudgeElement().targetID);
		VECTOR3F targetPosition = player->GetWorldTransform().position;
		VECTOR3F targetDistance = targetPosition - enemyTransform.position;
		VECTOR3F targetNormal = NormalizeVec3(targetDistance);
		float targetDist = ToDistVec3(targetDistance);
		VECTOR3F angle = enemy->GetWorldTransform().angle;
		VECTOR3F front = VECTOR3F(sinf(angle.y), 0.0f, cosf(angle.y));
		front = NormalizeVec3(front);

		float dot = DotVec3(front, targetNormal);
		float rot = 1.0f - dot;
		float limit = enemy->GetMove().turnSpeed + 0.02f;
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
	}
}

int EnemyFarAttack0Task::JudgeTurnChace(Enemy* enemy)
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

void EnemyFarAttack0Task::JudgeVectorDirection(Enemy* enemy)
{
	m_nVecToTarget = {};
	VECTOR3F enemyPosition = enemy->GetWorldTransform().position;
	FLOAT4X4 world = enemy->GetWorldTransform().world;
	VECTOR3F axis = { world._11,world._12,world._13 };

	axis = NormalizeVec3(axis);

	VECTOR3F movePoint = enemyPosition + axis * 40.0f;
	Collision::Circle stageRange;
	stageRange.radius = 81.0f;
	stageRange.position = {};
	stageRange.scale = 1.0f;
	Collision coll;

	if (!coll.JudgeCircleAndpoint(stageRange, VECTOR2F(movePoint.x, movePoint.z)))
		m_nVecToTarget = axis;
	else
	{
		movePoint = enemyPosition + axis * -40.0f;
		if (!coll.JudgeCircleAndpoint(stageRange, VECTOR2F(movePoint.x, movePoint.z)))
			m_nVecToTarget = axis * -1.0f;
	}
	int targetID = enemy->GetJudgeElement().targetID;
	auto& player = MESSENGER.CallPlayerInstance(targetID);
	VECTOR3F playerPosition = player->GetWorldTransform().position;
	axis = enemyPosition - playerPosition;
	axis = NormalizeVec3(axis);
	movePoint = enemyPosition + axis * 40.0f;

	if (!coll.JudgeCircleAndpoint(stageRange, VECTOR2F(movePoint.x, movePoint.z)))
		m_nVecToTarget = axis;

}

bool EnemyFarAttack0Task::IsTurnChase(Enemy* enemy)
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

bool EnemyFarAttack0Task::AttackMove(Enemy* enemy)
{
	uint32_t currentAnimationTime = enemy->GetBlendAnimation().animationBlend.GetAnimationTime(0);

	auto& enemyTransform = enemy->GetWorldTransform();

	if (!m_setTarget)
	{	
		auto& player = MESSENGER.CallPlayerInstance(enemy->GetJudgeElement().targetID);
		m_targetPosition = player->GetWorldTransform().position;
		VECTOR3F targetDistance = m_targetPosition - enemyTransform.position;
		m_nVecToTarget = NormalizeVec3(targetDistance);
		m_setTarget = true;
		m_accel = 75.0f;
	}
	float dist = ToDistVec3(m_targetPosition - enemyTransform.position);
	if (dist <= 40.0f && !m_isNear)
	{
		m_isNear = true;
		return true;
	}

	if (m_isNear)
	{
		//dist *= (1.0f / 3.f);
		m_accel -= 1.0f;
		if (m_accel <= 5.0f)
			m_accel = 5.0f;
	}
	else
	{
		VECTOR3F targetDistance = m_targetPosition - enemy->GetWorldTransform().position;
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
	}
	
	enemy->GetMove().velocity = m_nVecToTarget * m_accel;
	enemyTransform.position += enemy->GetMove().velocity * enemy->GetElapsedTime();
	enemyTransform.WorldUpdate();

	return false;
}

uint32_t EnemyFarAttack0Task::JudgePriority(const int id, const VECTOR3F playerPos) 
{
	if (m_isUsed) return minPriority;

	std::shared_ptr<CharacterAI> enemy = MESSENGER.CallEnemyInstance(id);

	uint32_t damageCount = enemy->GetJudgeElement().damageCount;
	if (damageCount >= kDamageRatio)
	{
		auto player = MESSENGER.CallPlayersInstance();

		VECTOR3F playerPosition = playerPos;
		VECTOR3F enemyPosition = enemy->GetWorldTransform().position;

		float direction = ToDistVec3(playerPosition - enemyPosition);
		if (direction < kMinDirection)
			return m_priority;
	}

	return minPriority;
}
