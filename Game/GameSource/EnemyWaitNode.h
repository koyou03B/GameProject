#pragma once
#include "EnemyBehaviorNode.h"
#include "MessengTo.h"

class EnemyWaitNode : public EnemyBehaviorNode
{
public:
	EnemyWaitNode() = default;
	~EnemyWaitNode() = default;
	uint32_t JudgePriority();
};

uint32_t EnemyWaitNode::JudgePriority()
{

}