#include "EnemyNearAttack1Task.h"
#include "MessengTo.h"
#include "Enemy.h"

//magicNumber
void EnemyNearAttack1Task::Run(Enemy* enemy)
{
#if 0
	auto& animation = enemy->GetBlendAnimation();

	switch (m_moveState)
	{
	case Action::START:
		m_taskState = TASK_STATE::RUN;
		if (animation.animationBlend.GetSampler().size() != 2)
		{
			m_animNo = Enemy::Animation::TURN_ATTACK;
			animation.animationBlend.AddSampler(m_animNo, enemy->GetModel());
			animation.animationBlend.ResetAnimationFrame();
		}
		else
		{
			if (JudgeBlendRatio(animation))
			{
				++m_moveState;
				++enemy->GetJudgeElement().attackCount;
				++enemy->GetJudgeElement().moveCount;
				m_animNo = Enemy::Animation::IDLE;
			}
		}
		break;
	case Action::TURN_ATTACK:
		m_attackNo = static_cast<int>(Enemy::AttackType::TURN_ATTACK);
		if (JudgeAnimationRatio(enemy, m_attackNo, m_animNo))
		{
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
		//‚±‚±‚Å’x‰„?

		JudgeAttack(enemy, m_attackNo);
		AttackMove(enemy);
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
		m_animNo = Enemy::Animation::HOOK;
		if (animation.animationBlend.SearchSampler(m_animNo))
		{
			if (JudgeBlendRatio(animation))
			{
				m_animNo = Enemy::Animation::RIGHT_PUNCH_LOWER;
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
	case Action::HOOK:
	{
		static bool isAChange = false;
		if (m_isHit && !isAChange)
		{
			isAChange = true;
			m_animNo = GetRundumSignal();
		}

		m_attackNo = Enemy::AttackType::Hook;
		AttackMove(enemy);
		JudgeAttack(enemy, m_attackNo);

		if (JudgeAnimationRatio(enemy, m_attackNo, m_animNo))
		{
			isAChange = false;
			m_isHit = false;
			animation.animationBlend.SetAnimationSpeed(1.0f);
			if (m_animNo == Enemy::Animation::RIGHT_PUNCH_LOWER)
			{
				m_animNo = GetRundumSignal();
				++enemy->GetJudgeElement().attackCount;
				++m_moveState;
			}
			else
			{
				m_coolTimer = 8.0f;
				m_animNo = m_animNo % Enemy::Animation::MUSCLE_SIGNAL;
				m_moveState = Action::SIGNAL;
			}
		}
		break;
	}	
	case Action::RPUNCHI_LOWER:
	{
		static bool blendFinish = false;
		if (!blendFinish)
			blendFinish = JudgeBlendRatio(animation);
		else
		{
			m_attackNo = Enemy::AttackType::RightPunchLower;
			JudgeAttack(enemy, m_attackNo);
			if (JudgeAnimationRatio(enemy, m_attackNo, m_animNo))
			{
				blendFinish = false;
				m_isHit = false;
				m_animNo = m_animNo % Enemy::Animation::MUSCLE_SIGNAL;
				animation.animationBlend.SetAnimationSpeed(1.0f);
				m_coolTimer = 9.0f;
				++m_moveState;
			}
		}
		break;
	}
	case Action::SIGNAL:
	{
		static bool blendFinish = false;
		if (!blendFinish)
			blendFinish = JudgeBlendRatio(animation);
		else
		{
			uint32_t currentAnimationTime = enemy->GetBlendAnimation().animationBlend.GetAnimationTime(0);
			uint32_t signalAnimationTime = enemy->GetSignalAnimFrame(m_animNo);
			if (currentAnimationTime >= signalAnimationTime)
			{
				blendFinish = false;
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
			m_moveState = Action::START;
			m_taskState = TASK_STATE::END;
		}
		break;
	}
}

bool EnemyNearAttack1Task::JudgeBlendRatio(CharacterParameter::BlendAnimation& animation, const bool isLoop)
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

void EnemyNearAttack1Task::JudgeAttack(Enemy* enemy, const int attackNo)
{
	uint32_t currentAnimationTime = enemy->GetBlendAnimation().animationBlend.GetAnimationTime(0);

	uint32_t attackTimer[2] = {};
	int collID = 0;
	if (m_moveState == Action::HOOK)
	{
		if (currentAnimationTime < kAnimationSpeed[0])
			enemy->GetBlendAnimation().animationBlend.SetAnimationSpeed(1.0f);
		else if (currentAnimationTime >= kAnimationSpeed[0] && currentAnimationTime < kAnimationSpeed[1])
			enemy->GetBlendAnimation().animationBlend.SetAnimationSpeed(1.2f);
		else
			enemy->GetBlendAnimation().animationBlend.SetAnimationSpeed(1.0f);

		attackTimer[0] = kAnimationSpeed[0];
		attackTimer[1] = kAnimationSpeed[1];
		collID = 2;
	}
	else if (m_moveState == Action::RPUNCHI_LOWER)
	{
		if (currentAnimationTime < kAnimationSpeed[2])
			enemy->GetBlendAnimation().animationBlend.SetAnimationSpeed(0.8f);
		else if (currentAnimationTime >= kAnimationSpeed[2] && currentAnimationTime < kAnimationSpeed[3])
			enemy->GetBlendAnimation().animationBlend.SetAnimationSpeed(1.2f);
		else
			enemy->GetBlendAnimation().animationBlend.SetAnimationSpeed(0.8f);

		attackTimer[0] = kAnimationSpeed[2];
		attackTimer[1] = kAnimationSpeed[3];
		collID = 1;
	}

	if (currentAnimationTime > attackTimer[0] && currentAnimationTime < attackTimer[1])
	{
		auto& attackParm = enemy->GetCollision().at(collID);
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

bool EnemyNearAttack1Task::JudgeAnimationRatio(Enemy* enemy, const int attackNo, const int nextAnimNo)
{
	uint32_t currentAnimationTime = enemy->GetBlendAnimation().animationBlend.GetAnimationTime(0);
	uint32_t attackFrameCount = enemy->GetAttack(attackNo).frameCount;
	if (m_moveState == Action::HOOK && !m_isHit) attackFrameCount = enemy->GetAttack(attackNo).frameCount - 30;
	if (currentAnimationTime >= attackFrameCount)
	{
		enemy->GetBlendAnimation().animationBlend.AddSampler(nextAnimNo, enemy->GetModel());
		return true;
	}

	return false;
}

int EnemyNearAttack1Task::JudgeTurnChace(Enemy* enemy)
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

int EnemyNearAttack1Task::GetRundumSignal()
{
	srand((unsigned)time(NULL));
	return rand() % 3 + Enemy::Animation::MUSCLE_SIGNAL;
}

bool EnemyNearAttack1Task::IsTurnChase(Enemy* enemy)
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

void EnemyNearAttack1Task::AttackMove(Enemy* enemy)
{
	uint32_t currentAnimationTime = enemy->GetBlendAnimation().animationBlend.GetAnimationTime(0);
	if (currentAnimationTime >= kMoveTimer[0] && currentAnimationTime < kMoveTimer[1])
	{
		auto& enemyTransform = enemy->GetWorldTransform();

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



		VECTOR3F velocity = targetNormal * kAccel;

		enemyTransform.position += velocity * enemy->GetElapsedTime();
		enemyTransform.WorldUpdate();
	}
}

uint32_t EnemyNearAttack1Task::JudgePriority(const int id, const VECTOR3F playerPos) 
{

	if (m_isUsed) return minPriority;
	auto player = MESSENGER.CallPlayersInstance();
	std::shared_ptr<CharacterAI> enemy = MESSENGER.CallEnemyInstance(id);

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

	if (cosTheta <= frontValue)
	{
		if (direction <= kAttackLength)
			return m_priority;
		else
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
