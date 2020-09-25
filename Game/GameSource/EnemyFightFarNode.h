#pragma once
#include "EnemyBehaviorNode.h"
#include "MessengTo.h"

class EnemyFightFarNode : public EnemyBehaviorNode
{
public:
	EnemyFightFarNode() = default;
	~EnemyFightFarNode() = default;
	uint32_t JudgePriority();
};

uint32_t EnemyFightFarNode::JudgePriority()
{

}