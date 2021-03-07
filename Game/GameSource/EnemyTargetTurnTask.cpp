#include "EnemyTargetTurnTask.h"
#include "MessengTo.h"
#include "Enemy.h"

void EnemyTargetTurnTask::Run(Enemy* enemy)
{
	CharacterAI* fighter = MESSENGER.CallPlayerInstance(PlayerType::Fighter);
	CharacterAI* archer = MESSENGER.CallPlayerInstance(PlayerType::Archer);
	fighter->GetJudgeElement().attackHitCount = 0;
	fighter->GetJudgeElement().attackHitCount = 0;
	//uint32_t fighterAttackHitCount = 0;
	//uint32_t archerAttackHitCount = 0;
	//VECTOR3F playerPosition = {};
	//int targetID = enemy->GetJudgeElement().targetID;
	//PlayerType playerType = static_cast<PlayerType>(targetID);

	//if (fighter)
	//{
	//	fighterAttackHitCount = fighter->GetJudgeElement().attackHitCount;
	//	if (playerType == PlayerType::Fighter)
	//		playerPosition = fighter->GetWorldTransform().position;
	//}
	//if (archer)
	//{
	//	archerAttackHitCount = archer->GetJudgeElement().attackHitCount;
	//	if (playerType == PlayerType::Archer)
	//		playerPosition = archer->GetWorldTransform().position;
	//}

	//if (fighterAttackHitCount > archerAttackHitCount)
	//{
	//	if (playerType != PlayerType::Fighter)
	//	{
	//		enemy->GetJudgeElement().targetID = static_cast<int>(PlayerType::Fighter);
	//	}
	//}
	//else
	//{
	//	if (playerType != PlayerType::Archer)
	//	{
	//		enemy->GetJudgeElement().targetID = static_cast<int>(PlayerType::Archer);
	//	}
	//}


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
	if (targetID != m_targetID)
	{
		m_targetID = targetID;
		return m_priority;

	}

	return minPriority;
}
