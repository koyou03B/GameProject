#pragma once
#include "EnemyBehaviorNode.h"
#include "MessengTo.h"

class EnemyChaseNode : public EnemyBehaviorNode
{
public:
	EnemyChaseNode() = default;
	~EnemyChaseNode() = default;
	uint32_t JudgePriority();
};

uint32_t EnemyChaseNode::JudgePriority()
{

}