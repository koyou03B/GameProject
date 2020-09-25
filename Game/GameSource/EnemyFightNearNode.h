#pragma once
#include "EnemyBehaviorNode.h"
#include "MessengTo.h"

class EnemyFightNearNode : public EnemyBehaviorNode
{
public:
	EnemyFightNearNode() = default;
	~EnemyFightNearNode() = default;
	uint32_t JudgePriority();
};

uint32_t EnemyFightNearNode::JudgePriority()
{

}