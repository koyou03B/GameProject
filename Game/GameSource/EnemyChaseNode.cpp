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
		//�U���͂������Ȃ�����Hit���Ă������܂ŋC�ɂ�����̂���Ȃ�
		//�Ȃ̂ŏ������炷(���A�n�G�͋߂��ɂ��Ă������Ƃ������̂ł���K�v���͂���)
		archerAttackHitCount *= 0.6f;
	}
	fighter->GetJudgeElement().attackHitCount = 0;
	archer->GetJudgeElement().attackHitCount = 0;

	//�w�C�g�Ǘ�
	//----------------------------------------------------------------------
	//1.�v���C���[�ƃA�[�`���[�̋��������߂�
	//2.�����0����1�̊Ԃ̐����Ɏ��߂�(���K��)
	//3.���̐�����1�ň���(����->�������������F�߂�->�������ł���)
	//4.���U���𓖂ĂĂ���ق���HitCountPoint�𑫂�
	//5.Point���ł����ق������̃^�[�Q�b�g
	//6.�����Ȃ�ǂ����_���Ă������̂Ń����_��
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