#pragma once
#include "EnemyBehaviorNode.h"
#include "MessengTo.h"

class EnemyFightNode : public EnemyBehaviorNode
{
public:
	EnemyFightNode() = default;
	~EnemyFightNode() = default;
	uint32_t JudgePriority();
};

uint32_t EnemyFightNode::JudgePriority()
{

}