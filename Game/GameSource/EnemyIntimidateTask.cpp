#include "EnemyIntimidateTask.h"
#include "MessengTo.h"
#include "Enemy.h"

void EnemyIntimidateTask::Run(Enemy* enemy)
{
	auto& animation = enemy->GetBlendAnimation();

	switch (m_moveState)
	{
	case 0:
	{
		m_taskState = TASK_STATE::START;
		animation.animationBlend.AddSampler(5, enemy->GetModel());
		animation.animationBlend.ResetAnimationFrame();
		enemy->GetJudgeElement().moveCount = 0;
		enemy->GetJudgeElement().attackCount = 0;
		enemy->GetJudgeElement().damageCount= 0;
		enemy->GetJudgeElement().attackHitCount = 0;

		 enemy->GetEmotion().exhaustionParm.exhaustionCost = 0;
		 enemy->GetEmotion().wrathParm.wrathCost = 0;
		 enemy->GetEmotion().wrathParm.isWrath = true;

		++m_moveState;
	}
	break;
	case 1:
	{
		m_taskState = TASK_STATE::RUN;
		if (JudgeBlendRatio(animation))
			++m_moveState;
	}
	break;
	case 2:
	{
		m_taskState = TASK_STATE::RUN;
		uint32_t currentAnimationTime = enemy->GetBlendAnimation().animationBlend.GetAnimationTime(0);
		if (currentAnimationTime >= 170)
		{
			m_taskState = TASK_STATE::END;
		}
	}
	break;
	}
}

bool EnemyIntimidateTask::JudgeBlendRatio(CharacterParameter::BlendAnimation & animation)
{
	animation.animationBlend._blendRatio += 0.08f;//magicNumber
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

bool EnemyIntimidateTask::JudgeAnimationRatio(Enemy* enemy, const int attackNo, const int nextAnimNo)
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

uint32_t EnemyIntimidateTask::JudgePriority(const int id, const VECTOR3F playerPos) 
{
	EnemyType type = static_cast<EnemyType>(id);
	CharacterAI* enemy = MESSENGER.CallEnemyInstance(type);
	//**************************
	// MoveCost,AttackCost
	//**************************

	if (enemy->GetEmotion().wrathParm.isWrath)
		return m_priority;

	return minPriority;
}
