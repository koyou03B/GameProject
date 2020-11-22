#include "EnemyRestTask.h"
#include "MessengTo.h"
#include "Enemy.h"

void EnemyRestTask::Run(Enemy* enemy)
{
	auto& animation = enemy->GetBlendAnimation();

	switch (m_moveState)
	{
	case 0:
	{
		m_taskState = TASK_STATE::START;
		animation.animationBlend.AddSampler(1, enemy->GetModel());
		animation.animationBlend.ResetAnimationFrame();
		enemy->GetJudgeElement().moveCount = 0;
		enemy->GetJudgeElement().attackCount = 0;
		enemy->GetJudgeElement().damageCount = 0;
		enemy->GetEmotion().exhaustionParm.exhaustionCost = 0;
		animation.animationBlend.SetAnimationSpeed(1.5f);
		m_restTime = 0;
		++m_moveState;
	}
		break;
	case 1:
		m_taskState = TASK_STATE::RUN;
		if (JudgeBlendRatio(animation))
			++m_moveState;
		break;
	case 2:
	{
		m_restTime += enemy->GetElapsedTime();
		if (m_restTime >= 5)
		{
			m_taskState = TASK_STATE::END;
		}
		animation.animationBlend.SetAnimationSpeed(1.0f);
	}
	break;
	}
}

bool EnemyRestTask::JudgeBlendRatio(CharacterParameter::BlendAnimation& animation)
{
	animation.animationBlend._blendRatio += 0.08f;//magicNumber
	if (animation.animationBlend._blendRatio >= animation.blendRatioMax)//magicNumber
	{
		animation.animationBlend._blendRatio = 0.0f;
		animation.animationBlend.ResetAnimationSampler(0);
		animation.animationBlend.ReleaseSampler(0);
		return true;
	}

	return false;
}

uint32_t EnemyRestTask::JudgePriority(const int id, const VECTOR3F playerPos) 
{
	std::shared_ptr<CharacterAI> enemy = MESSENGER.CallEnemyInstance(id);
	if(enemy->GetEmotion().exhaustionParm.isExhaustion)
		return m_priority;
	
	return minPriority;
}
