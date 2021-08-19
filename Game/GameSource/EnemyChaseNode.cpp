#include <stdlib.h>
#include "EnemyChaseNode.h"

//**************************************************
// JudgePriorityRule
//	1.The player with more hit attacks
//	2.Distance to the player
//**************************************************
uint32_t EnemyChaseNode::JudgePriority(const int id)
{
	EnemyType enemyType = static_cast<EnemyType>(id);
	CharacterAI* enemy = MESSENGER.CallEnemyInstance(enemyType);
	CharacterAI* fighter = MESSENGER.CallPlayerInstance(PlayerType::Fighter);
	CharacterAI* archer = MESSENGER.CallPlayerInstance(PlayerType::Archer);

	if (m_isUse)
	{
		++m_coolCount;
		if (m_coolCount >= m_maxCoolCount)
		{
			m_isUse = false;
			m_coolCount = 0;
			m_maxCoolCount = 0;
		}
		else
			return minPriority;
	}

	float fighterAttackHitCount = 0;
	float archerAttackHitCount = 0;

	if (fighter)
		fighterAttackHitCount = static_cast<float>(fighter->GetJudgeElement().attackHitCount);
	if (archer)
	{
		archerAttackHitCount = static_cast<float>(archer->GetJudgeElement().attackHitCount);
		//攻撃力が高くないからHitしてもそこまで気にするものじゃない
		//なので少し減らす(が、ハエは近くにいてもうっとおしいのでいる必要性はある)
		archerAttackHitCount *= 0.6f;
	}
	fighter->GetJudgeElement().attackHitCount = 0;
	archer->GetJudgeElement().attackHitCount = 0;

	//ヘイト管理
	//----------------------------------------------------------------------
	//1.プレイヤーとアーチャーの距離を求める
	//2.それを0から1の間の数字に収める(正規化)
	//3.その数字を1で引く(遠い->数字が小さい：近い->数字がでかい)
	//4.より攻撃を当てているほうにHitCountPointを足す
	//5.Pointがでかいほうを次のターゲット
	//6.同じならどちらを狙ってもいいのでランダム
	//----------------------------------------------------------------------
	VECTOR3F distanceToFighter = fighter->GetWorldTransform().position - enemy->GetWorldTransform().position;
	VECTOR3F distanceToArcher  = archer->GetWorldTransform().position - enemy->GetWorldTransform().position;

	float lengthToFighter = ToDistVec3(distanceToFighter);
	float lengthToArcher = ToDistVec3(distanceToArcher);

	float normalizeToFighter = lengthToFighter / m_maxDirection;
	float normalizeToArcher  = lengthToArcher / m_maxDirection;

	m_fighterPoint = 1.0f - normalizeToFighter;
	m_archerPoint  = 1.0f - normalizeToArcher;

	if (fighterAttackHitCount > archerAttackHitCount)
		m_fighterPoint += HitCountPoint;
	else
		m_archerPoint += HitCountPoint;

	if (m_fighterPoint > m_archerPoint)
	{
		enemy->GetJudgeElement().targetID = static_cast<int>(PlayerType::Fighter);
		m_isUse = true;
		srand((unsigned)time(NULL));
		m_maxCoolCount= rand() % 2 +2;
		return m_priority;
	}
	else if (m_archerPoint > m_fighterPoint)
	{
		enemy->GetJudgeElement().targetID = static_cast<int>(PlayerType::Archer);
		m_isUse = true;
		srand((unsigned)time(NULL));
		m_maxCoolCount = rand() % 2 + 2;
		return m_priority;
	}
	else
	{
		m_isUse = true;
		srand((unsigned)time(NULL));
		int judge = rand() % 2;
		m_maxCoolCount = judge + 2;
		if (judge == static_cast<int>(PlayerType::Fighter))
		{
			enemy->GetJudgeElement().targetID = static_cast<int>(PlayerType::Fighter);
			return m_priority;
		}
		else
		{
			enemy->GetJudgeElement().targetID = static_cast<int>(PlayerType::Archer);
			return m_priority;
		}
	}

	return minPriority;
}