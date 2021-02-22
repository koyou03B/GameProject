#include "EnemyTargetTurnTask.h"
#include "MessengTo.h"
#include "Enemy.h"

void EnemyTargetTurnTask::Run(Enemy* enemy)
{


}

bool EnemyTargetTurnTask::JudgeBlendRatio(CharacterParameter::BlendAnimation& animation)
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

uint32_t EnemyTargetTurnTask::JudgePriority(const int id, const VECTOR3F playerPos) 
{
	EnemyType type = static_cast<EnemyType>(id);
	CharacterAI* enemy = MESSENGER.CallEnemyInstance(type);
	int targetID = enemy->GetJudgeElement().targetID;
	if(targetID != m_targetID)
		return m_priority;

	return minPriority;
}
