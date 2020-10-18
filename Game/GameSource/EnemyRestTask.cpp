#include "EnemyRestTask.h"
#include "MessengTo.h"
#include "Enemy.h"

void EnemyRestTask::Run(Enemy* enemy)
{
	auto& animation = enemy->GetBlendAnimation();

	switch (m_moveState)
	{
	case 0:
		m_taskState = TASK_STATE::START;
		animation.animationBlend.AddSampler(1, enemy->GetModel());
		animation.animationBlend.ResetAnimationFrame();
		++m_moveState;

		break;
	case 1:
		m_taskState = TASK_STATE::RUN;
		if (JudgeBlendRatio(animation))
			++m_moveState;
		break;
	case 2:
	{
		++m_restTime;
		if (m_restTime >= 300)
		{
			AIParameter::Emotion emotion = enemy->GetEmotion();
			AIParameter::JudgeElement judgeElement = enemy->GetJudgeElement();
			judgeElement.moveCount = 0;
			judgeElement.attackCount = 0;
			judgeElement.damageCount = 0;
			emotion.exhaustionParm.exhaustionCost = 0;

			m_taskState = TASK_STATE::END;
		}
	}
	break;
	}
}

bool EnemyRestTask::JudgeBlendRatio(CharacterParameter::BlendAnimation& animation)
{
	animation.animationBlend._blendRatio += 0.045f;//magicNumber
	if (animation.animationBlend._blendRatio >= animation.blendRatioMax)//magicNumber
	{
		animation.animationBlend._blendRatio = 0.0f;
		animation.animationBlend.ResetAnimationSampler(0);
		animation.animationBlend.ReleaseSampler(0);
		return true;
	}

	return false;
}

uint32_t EnemyRestTask::JudgePriority(const int id)
{
	std::shared_ptr<CharacterAI> enemy = MESSENGER.CallEnemyInstance(id);
	AIParameter::Emotion emotion = enemy->GetEmotion();

	uint32_t exhaustionCost = emotion.exhaustionParm.exhaustionCost;
	
	if (exhaustionCost >= emotion.exhaustionParm.maxExhaustionCost)
		return m_priority;
	
	return minPriority;
}
