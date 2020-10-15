#include "EnemyNearSpecialAttack0Task.h"
#include "Enemy.h"

void EnemyNearSpecialAttack0Task::Run(Enemy* enemy)
{
	auto& animation = enemy->GetBlendAnimation();

	switch (m_moveState)
	{
	case 0:
		m_taskState = TASK_STATE::START;
		animation.animationBlend.AddSampler(15, enemy->GetModel());
		animation.animationBlend.ResetAnimationFrame();
		++m_moveState;
		enemy->GetStatus().isAttack = false;
		break;
	case 1:
		m_taskState = TASK_STATE::RUN;
		if (JudgeBlendRatio(animation))
			++m_moveState;
		break;
	case 2:
	{
		if (JudgeAnimationRatio(enemy, 7, 1))
		{
			++m_moveState;
			enemy->GetStatus().isAttack = false;
		}
		uint32_t currentAnimationTime = enemy->GetBlendAnimation().animationBlend.GetAnimationTime(0);

		bool isHitAttack = enemy->GetStatus().isAttack;
		if (!isHitAttack && currentAnimationTime >= 140 && currentAnimationTime <= 160)
		{
			auto& rangeAttack = enemy->GetCollision().at(3);
			int rangeAttackMesh = rangeAttack.GetCurrentMesh(0);
			int rangeAttackBone = rangeAttack.GetCurrentBone(0);
			FLOAT4X4 getAttackBone = animation.animationBlend._blendLocals[rangeAttackMesh].at(rangeAttackBone);
			FLOAT4X4 modelAxisTransform = enemy->GetModel()->_resource->axisSystemTransform;
			FLOAT4X4 worldTransform = enemy->GetWorldTransform().world;
			FLOAT4X4 AttackTransform = getAttackBone * modelAxisTransform * worldTransform;

			rangeAttack.position[0] = { AttackTransform._41,AttackTransform._42,AttackTransform._43 };
			enemy->GetStatus().attackPoint = enemy->GetAttack(5).attackPoint;

			MESSENGER.EnemyAttackingMessage(enemy->GetID(), rangeAttack);
		}
		break;
	}
	case 3:
		if (JudgeBlendRatio(animation))
		{
			animation.animationBlend.ResetAnimationSampler(0);
			m_moveState = 0;
			m_taskState = TASK_STATE::END;
		}
		break;
	}
}

bool EnemyNearSpecialAttack0Task::JudgeBlendRatio(CharacterParameter::BlendAnimation& animation)
{
	m_taskState = TASK_STATE::RUN;
	animation.animationBlend._blendRatio += 0.045f;//magicNumber
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


uint32_t EnemyNearSpecialAttack0Task::JudgePriority(const int id)
{
	std::shared_ptr<CharacterAI> enemy = MESSENGER.CallEnemyInstance(id);
	bool isWrath = enemy->GetEmotion().wrathParm.isWrath;
	if (!isWrath)
		return minPriority;

	return m_priority;
}
