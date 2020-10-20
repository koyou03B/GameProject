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
		AIParameter::Emotion emotion = enemy->GetEmotion();
		AIParameter::JudgeElement judgeElement = enemy->GetJudgeElement();
		judgeElement.moveCount = 0;
		judgeElement.attackCount = 0;
		judgeElement.damageCount = 0;
		judgeElement.attackHitCount = 0;
		emotion.exhaustionParm.exhaustionCost = 0;
		emotion.wrathParm.wrathCost = 0;
		emotion.wrathParm.isWrath = true;

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

uint32_t EnemyIntimidateTask::JudgePriority(const int id)
{
	std::shared_ptr<CharacterAI> enemy = MESSENGER.CallEnemyInstance(id);
	//**************************
	// MoveCost,AttackCost
	//**************************
	AIParameter::Emotion emotion = enemy->GetEmotion();
	AIParameter::JudgeElement judgeElement = enemy->GetJudgeElement();

	if (emotion.wrathParm.isWrath)
		return minPriority;

	uint32_t attackHitCost = judgeElement.attackHitCount * emotion.wrathParm.attackWrathCost;
	uint32_t damageCost = judgeElement.damageCount * emotion.wrathParm.damageWrathCost;

	emotion.wrathParm.wrathCost = damageCost - attackHitCost;

	if (emotion.wrathParm.wrathCost >= emotion.wrathParm.maxWrathCost)
		return m_priority;

	return minPriority;
}
