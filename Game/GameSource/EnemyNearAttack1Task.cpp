#include "EnemyNearAttack1Task.h"
#include "MessengTo.h"
#include "Enemy.h"

//magicNumber
void EnemyNearAttack1Task::Run(Enemy* enemy)
{
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


}

bool EnemyNearAttack1Task::JudgeBlendRatio(CharacterParameter::BlendAnimation& animation)
{
	float blendRatio = m_moveState == Action::END ? kBlendValue[1] : kBlendValue[0];
	animation.animationBlend._blendRatio += blendRatio;//magicNumber
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

bool EnemyNearAttack1Task::IsTurnChase(Enemy* enemy)
{
	auto& animation = enemy->GetBlendAnimation();

	if (animation.animationBlend.GetSampler().size() == 2)
	{
		animation.animationBlend._blendRatio += kBlendValue[1];//magicNumber
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

void EnemyNearAttack1Task::JudgeAttack(Enemy* enemy, const int attackNo)
{
	uint32_t currentAnimationTime = enemy->GetBlendAnimation().animationBlend.GetAnimationTime(0);

	if (currentAnimationTime > kAttackTimer[0] &&currentAnimationTime < kAttackTimer[1])
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

void EnemyNearAttack1Task::AttackMove(Enemy* enemy)
{
	uint32_t currentAnimationTime = enemy->GetBlendAnimation().animationBlend.GetAnimationTime(0);
	if (currentAnimationTime >= kRowlingTimer[0] && currentAnimationTime <= kRowlingTimer[1])
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
		dot = acosf(dot);
		dot /= kOneSecond;

		if (!std::isfinite(dot))
			dot = 0.0f;

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

	if (currentAnimationTime >= kMoveTimer[0]&&currentAnimationTime <= kMoveTimer[1])
	{
		auto& enemyTransform = enemy->GetWorldTransform();

		auto& player = MESSENGER.CallPlayerInstance(enemy->GetJudgeElement().targetID);
		VECTOR3F targetPosition = player->GetWorldTransform().position;
		VECTOR3F targetDistance = targetPosition - enemyTransform.position;
		VECTOR3F targetNormal = NormalizeVec3(targetDistance);
		float targetDist = ToDistVec3(targetDistance);

		enemy->GetMove().velocity = targetNormal * (targetDist / kOneSecond/3.0f);
		enemyTransform.position += enemy->GetMove().velocity;

		enemyTransform.WorldUpdate();
	}
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
	
	////‹ß‚¢‚¯‚Ç‹ß‚­‚È‚¢‚È‚ç‚±‚±
	//if(direction > kMinDirection)
	//	return m_priority;

	return minPriority;
}
