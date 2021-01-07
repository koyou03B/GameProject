#include "EnemyFightNode.h"

uint32_t EnemyFightNode::JudgePriority(const int id)
{
	std::shared_ptr<EnemyBehaviorNode> node = SearchNode("UnSpecialAttack");
	auto task = node->SearchTask("FarAttack2Task");
	if(!task->GetIsUsed())
		return m_priority;

	return 1;
}