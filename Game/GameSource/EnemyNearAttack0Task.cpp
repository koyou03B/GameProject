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
		if (animation.animationBlend.GetSampler().size() != 2)
		{
			animation.animationBlend.AddSampler(Enemy::Animation::LEFT_PUNCH, enemy->GetModel());
			animation.animationBlend.ResetAnimationFrame();
		}
		else
		{
			if (JudgeBlendRatio(animation))
			{
				++m_moveState;
				AIParameter::JudgeElement judgeElement = enemy->GetJudgeElement();
				++judgeElement.attackCount;
			}
		}

		break;
	case Action::LEFT_PUNCH:
		m_attackNo = static_cast<int>(Enemy::AttackType::LEFT_PUNCH);
		if (JudgeAnimationRatio(enemy, m_attackNo, Enemy::Animation::RIGHT_PUNCH))
			++m_moveState;

		JudgeAttack(enemy, m_attackNo);
		break;
	case Action::ANIM_CHANGE:
		if (JudgeBlendRatio(animation))
		{
			++m_moveState;
			AIParameter::JudgeElement judgeElement = enemy->GetJudgeElement();
			++judgeElement.attackCount;
		}
		break;
	case Action::RIGHT_PUNCH:
		m_attackNo = static_cast<int>(Enemy::AttackType::RIGHT_PUNCH);
		if (JudgeAnimationRatio(enemy, m_attackNo, Enemy::Animation::IDLE))
			++m_moveState;

		JudgeAttack(enemy, m_attackNo);
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

bool EnemyNearAttack0Task::JudgeBlendRatio(CharacterParameter::BlendAnimation& animation)
{
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

bool EnemyNearAttack0Task::JudgeAnimationRatio(Enemy* enemy, const int attackNo, const int nextAnimNo)
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

void EnemyNearAttack0Task::JudgeAttack(Enemy* enemy, const int attackNo)
{
	uint32_t currentAnimationTime = enemy->GetBlendAnimation().animationBlend.GetAnimationTime(0);

	if (currentAnimationTime < kAttackTimer)
	{
		int collisionNo = attackNo + kCollisionNo;
		auto& attackParm = enemy->GetCollision().at(collisionNo);
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

uint32_t EnemyNearAttack0Task::JudgePriority(const int id)
{
	auto player = MESSENGER.CallPlayersInstance();
	std::shared_ptr<CharacterAI> enemy = MESSENGER.CallEnemyInstance(id);
	int targetID = enemy->GetJudgeElement().targetID;

	VECTOR3F playerPosition = player.at(targetID)->GetWorldTransform().position;
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
# if 1
		uint32_t attackHitCount = enemy->GetJudgeElement().attackHitCount;
		uint32_t attackCount = enemy->GetJudgeElement().attackCount;
		float attackRatio = enemy->GetStandardValue().attackRatio;
		float ratio = static_cast<float>(attackHitCount) / static_cast<float>(attackCount);
		if (ratio <= attackRatio)
			return m_priority;
#else
		return m_priority;
#endif
	}

	return minPriority;
}
