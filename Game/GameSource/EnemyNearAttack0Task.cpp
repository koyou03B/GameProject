#include "EnemyNearAttack0Task.h"
#include "MessengTo.h"
#include "Enemy.h"
#include ".\LibrarySource\Vector.h"


void EnemyNearAttack0Task::Run(Enemy* enemy)
{
	auto& animation = enemy->GetBlendAnimation();
	switch (m_moveState)
	{
	case Action::START:
		m_taskState = TASK_STATE::RUN;
		m_animNo = Enemy::Animation::CROSS_PUNCH;
		m_hasFinishedBlend = false;
		m_hasSentSignal = false;
		m_isHit = false;
		if (animation.animationBlend.SearchSampler(m_animNo))
		{
			if (JudgeBlendRatio(animation))
			{
				m_animNo = Enemy::Animation::TURN_ATTACK_LOWER;
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
	case Action::CROSS_PUNCH:	
		m_attackNo = Enemy::AttackType::CrossPunch;
		JudgeAttack(enemy, m_attackNo);

		if (JudgeAnimationRatio(enemy, m_attackNo, -1))
		{
			if (m_isHit)
			{
				m_coolTimer = 9.0f;
				m_animNo = JudgeTurnChace(enemy);
				if (m_animNo <= 1)
				{
					m_isHit = false;
					animation.animationBlend.SetAnimationSpeed(1.0f);
					animation.animationBlend.ResetAnimationFrame();
					m_moveState = Action::END;
					break;
				}
				animation.animationBlend.AddSampler(m_animNo, enemy->GetModel());
				animation.animationBlend.ResetAnimationFrame();
				m_moveState = Action::TURN_CHACE;
			}
			else
			{
				if (m_animNo == Enemy::Animation::TURN_ATTACK_LOWER)
				{	
					animation.animationBlend.AddSampler(m_animNo, enemy->GetModel());
					m_animNo = GetRundumSignal();
					++enemy->GetJudgeElement().attackCount;
					++enemy->GetJudgeElement().moveCount;
					++m_moveState;
				}
			}
			m_isHit = false;
			animation.animationBlend.SetAnimationSpeed(1.0f);
		}
		break;
	case Action::TURN_ATTACK:
		if (!m_hasFinishedBlend)
		{
			m_hasFinishedBlend = JudgeBlendRatio(animation);
			int targetID = enemy->GetJudgeElement().targetID;
			PlayerType type = static_cast<PlayerType>(targetID);
			CharacterAI* player = MESSENGER.CallPlayerInstance(type);

			m_targetPosition = player->GetWorldTransform().position;
		}
		else
		{
			m_attackNo = Enemy::AttackType::TurnAttackLower;
			AttackMove(enemy);
			JudgeAttack(enemy, m_attackNo);
			if (JudgeAnimationRatio(enemy, m_attackNo, -1))
			{
				if (m_isHit)
				{
					animation.animationBlend.AddSampler(m_animNo, enemy->GetModel());
					++m_moveState;
				}
				else
				{
					//animation.animationBlend.AddSampler(Enemy::Animation::IDLE, enemy->GetModel());
					m_moveState = Action::END;
				}
				m_hasFinishedBlend = false;
				m_isHit = false;
				m_animNo = m_animNo % Enemy::Animation::MUSCLE_SIGNAL;
				animation.animationBlend.SetAnimationSpeed(1.0f);
				m_coolTimer = 12.0f;
				//++m_moveState;
			}
		}
		break;
	case Action::SIGNAL:
		if (!m_hasFinishedBlend)
			m_hasFinishedBlend = JudgeBlendRatio(animation);
		else
		{
			uint32_t currentAnimationTime = enemy->GetBlendAnimation().animationBlend.GetAnimationTime(0);
			uint32_t signalAnimationTime = enemy->GetSignalAnimFrame(m_animNo);
			if (currentAnimationTime >= signalAnimationTime)
			{
				m_hasFinishedBlend = false;
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
		size_t samplerSize = animation.animationBlend.GetSampler().size();
		if (samplerSize != 1)
		{
			if (JudgeBlendRatio(animation))
			{
				m_isUsed = true;
				animation.animationBlend.ResetAnimationSampler(0);
				m_moveState = Action::START;
				m_taskState = TASK_STATE::END;
			}
		}
		else
		{
			m_isUsed = true;
			m_moveState = Action::START;
			m_taskState = TASK_STATE::END;
		}
		break;
	}

}

bool EnemyNearAttack0Task::JudgeBlendRatio(CharacterParameter::BlendAnimation& animation, const bool isLoop)
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
		if(!isLoop)
			animation.animationBlend.FalseAnimationLoop(0);
		return true;
	}

	return false;
}

void EnemyNearAttack0Task::JudgeAttack(Enemy* enemy, const int attackNo)
{
	uint32_t currentAnimationTime = enemy->GetBlendAnimation().animationBlend.GetAnimationTime(0);
	uint32_t attackTimer = 0;
	static float radius = enemy->GetCollision().at(1).radius;

	if (m_moveState == Action::CROSS_PUNCH)
	{
		if (currentAnimationTime < kAnimationSpeed[0])
			enemy->GetBlendAnimation().animationBlend.SetAnimationSpeed(0.8f);
		else if (currentAnimationTime >= kAnimationSpeed[0] && currentAnimationTime < kAnimationSpeed[1])
			enemy->GetBlendAnimation().animationBlend.SetAnimationSpeed(1.2f);
		else
			enemy->GetBlendAnimation().animationBlend.SetAnimationSpeed(0.8f);

		attackTimer = kAnimationSpeed[0];
	}
	else if (m_moveState == Action::TURN_ATTACK)
	{
		if (currentAnimationTime < kAnimationSpeed[2])
			enemy->GetBlendAnimation().animationBlend.SetAnimationSpeed(0.8f);
		else if (currentAnimationTime >= kAnimationSpeed[2] && currentAnimationTime < kAnimationSpeed[1])
			enemy->GetBlendAnimation().animationBlend.SetAnimationSpeed(1.2f);
		else
			enemy->GetBlendAnimation().animationBlend.SetAnimationSpeed(0.8f);

		attackTimer = kAnimationSpeed[2];
	}

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
		float radius = attackParm.radius;
		attackParm.radius = m_moveState == Action::CROSS_PUNCH ? 1.725f : radius;
		if (!m_isHit && MESSENGER.AttackingMessage(EnemyType::Boss, attackParm))
			m_isHit = true;
		attackParm.radius = radius;
	}
	enemy->GetCollision().at(1).radius = radius;
}

bool EnemyNearAttack0Task::JudgeAnimationRatio(Enemy* enemy, const int attackNo, const int nextAnimNo)
{
	uint32_t currentAnimationTime = enemy->GetBlendAnimation().animationBlend.GetAnimationTime(0);
	uint32_t attackFrameCount = enemy->GetAttack(attackNo).frameCount;
	if (currentAnimationTime >= attackFrameCount)
	{
		if(nextAnimNo >= 0)
			enemy->GetBlendAnimation().animationBlend.AddSampler(nextAnimNo, enemy->GetModel());
		return true;
	}

	return false;
}

int EnemyNearAttack0Task::JudgeTurnChace(Enemy* enemy)
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

int EnemyNearAttack0Task::GetRundumSignal()
{
	srand((unsigned)time(NULL));
	return rand() % 3 + Enemy::Animation::MUSCLE_SIGNAL;
}

bool EnemyNearAttack0Task::IsTurnChase(Enemy* enemy)
{
	auto& animation = enemy->GetBlendAnimation();
	if (!m_hasFinishedBlend)
		m_hasFinishedBlend = JudgeBlendRatio(animation,true);
	
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
		if(m_animNo == Enemy::Animation::LEFT_TURN)
			rot *= -1;

		enemyTransform.angle.y += rot;
		enemyTransform.WorldUpdate();
	}

	return false;
}

void EnemyNearAttack0Task::AttackMove(Enemy* enemy)
{
	uint32_t currentAnimationTime = enemy->GetBlendAnimation().animationBlend.GetAnimationTime(0);
	if (currentAnimationTime >= kRowlingTimer[0] && currentAnimationTime <= kRowlingTimer[1])
	{
		auto& enemyTransform = enemy->GetWorldTransform();
		VECTOR3F targetDistance = m_targetPosition - enemyTransform.position;
		VECTOR3F targetNormal = NormalizeVec3(targetDistance);
		float targetDist = ToDistVec3(targetDistance);
		VECTOR3F angle = enemy->GetWorldTransform().angle;
		VECTOR3F front = VECTOR3F(sinf(angle.y), 0.0f, cosf(angle.y));
		front = NormalizeVec3(front);

		float dot = DotVec3(front, targetNormal);
		float rot = 1.0f - dot;
		float limit = 0.1f;
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

uint32_t EnemyNearAttack0Task::JudgePriority(const int id,const VECTOR3F playerPos)
{
	if (m_isUsed) return minPriority;

	EnemyType type = static_cast<EnemyType>(id);
	CharacterAI* enemy = MESSENGER.CallEnemyInstance(type);

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
		if(direction <= kAttackLength)
			return m_priority;
	}

	return minPriority;
}
