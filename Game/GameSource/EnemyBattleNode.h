#pragma once
#include "EnemyBehaviorNode.h"
#include "MessengTo.h"

class EnemyBattleNode : public EnemyBehaviorNode
{
public:
	EnemyBattleNode() = default;
	~EnemyBattleNode() = default;
	uint32_t JudgePriority();
};

uint32_t EnemyBattleNode::JudgePriority()
{

}