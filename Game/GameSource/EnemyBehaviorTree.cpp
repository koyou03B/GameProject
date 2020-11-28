#include "EnemyBehaviorTree.h"

void EnemyBehaviorTree::Release()
{
	int useTaskCount = static_cast<int>(m_useTasks.size());

	for (int i = 0; i < useTaskCount; ++i)
	{
		if (m_useTasks[i].unique())
			m_useTasks[i].reset();
	}

	m_useTasks.clear();

	m_taskData.Release();
	m_nodeData.Release();
	m_nodes->Release();
}

void EnemyBehaviorTree::UpdateUseTasks(const float elapsedTime)
{
	if (m_useTasks.empty()) return;
	for (auto& task : m_useTasks)
	{
		task->UpdateCoolTimer(elapsedTime);
	}

	bool isUsed = m_useTasks[0]->GetIsUsed();

	if (!isUsed)
		m_useTasks.erase(m_useTasks.begin());
}

void EnemyBehaviorTree::AddNode(std::string parentName, std::shared_ptr<EnemyBehaviorNode> node)
{
	if (m_nodes->GetNodeName() == parentName)
	{
		node->SetParentName(parentName);
		m_nodes->SetChildNode(node);
	}
	else
	{
		std::shared_ptr<EnemyBehaviorNode> selectNode = m_nodes->SearchNode(parentName);
		node->SetParentName(selectNode->GetNodeName());
		selectNode->SetChildNode(node);
	}
}

std::shared_ptr<EnemyBehaviorTask> EnemyBehaviorTree::SearchOfActiveTask(const int id)
{
	std::shared_ptr<EnemyBehaviorNode> selectNode;
	selectNode = m_nodes->SelectOfActivedNode(id);
	while (selectNode->GetTask().empty())
	{
		if (selectNode->GetSelectRule() != SELECT_RULE::SEQUENCE)
			selectNode = selectNode->SelectOfActivedNode(id);
		else
		{
			m_sequenceNode = selectNode;
			break;
		}
	}

	if (m_sequenceNode)
	{
		int taskCount = static_cast<int>(m_sequenceNode->GetTask().size());
		std::pair<int, std::shared_ptr<EnemyBehaviorTask>> selectTask = m_sequenceNode->SelectOfActiveTask(id);
		if (selectTask.first == taskCount)
		{
			m_sequenceNode.reset();
			return selectTask.second;
		}
	}
	else
	{
		std::pair<int, std::shared_ptr<EnemyBehaviorTask>> selectTask = selectNode->SelectOfActiveTask(id);
		return selectTask.second;
	}
#if 0 
	if (!m_sequenceNode)
	{
		selectNode = m_nodes->SelectOfActivedNode(enemy);

		if (selectNode->GetSelectRule() == SELECT_RULE::SEQUENCE)
			m_sequenceNode = selectNode;
	}

	if (m_sequenceNode)
	{
		int taskCount = static_cast<int>(m_sequenceNode->GetTask().size());
		std::pair<int,std::shared_ptr<EnemyBehaviorTask>> selectTask = m_sequenceNode->SelectOfActiveTask(enemy);
		if (selectTask.first == taskCount)
		{
			m_sequenceNode.reset();
			return selectTask.second;
		}

	}
	else
	{
		std::pair<int, std::shared_ptr<EnemyBehaviorTask>> selectTask = selectNode->SelectOfActiveTask(enemy);
		return selectTask.second;
	}
#endif
	return nullptr;
}

void EnemyBehaviorTree::SetRootNodeChild()
{
	std::shared_ptr<EnemyWaitNode>				waitNode	 = m_nodeData.waitNode;
	std::shared_ptr<EnemyChaseNode>				chaseNode	 = m_nodeData.chaseNode;
	std::shared_ptr<EnemyFightNode>				fightNode	 = m_nodeData.fightNode;
	std::shared_ptr<EnemySpecialAttackNode>		specialAttackNode = m_nodeData.specialAttackNode;
	std::shared_ptr<EnemyUnSpecialAttackNode>	unSpecialAttackNode = m_nodeData.unSpecialAttackNode;

	AddNode(waitNode->GetParentName(),		waitNode);
	AddNode(chaseNode->GetParentName(),		chaseNode);
	AddNode(fightNode->GetParentName(),		fightNode);
	AddNode(specialAttackNode->GetParentName(), specialAttackNode);
	AddNode(unSpecialAttackNode->GetParentName(), unSpecialAttackNode);
}

void EnemyBehaviorTree::SetTaskToNode()
{
	std::shared_ptr<EnemyRestTask>				 restTask = m_taskData.restTask;
	std::shared_ptr<EnemyIntimidateTask>		 intimidateTask = m_taskData.intimidateTask;
	std::shared_ptr<EnemyTargetTurnTask>		 chaseTask = m_taskData.targetTurnTask;
	std::shared_ptr<EnemyWalkTask>				 walkTask = m_taskData.walkTask;
	std::shared_ptr<EnemyNearAttack0Task>		 fightNearTask0 = m_taskData.fightNearTask0;
	std::shared_ptr<EnemyNearAttack1Task>		 fightNearTask1 = m_taskData.fightNearTask1;
	std::shared_ptr<EnemyNearAttack2Task>		 fightNearTask2 = m_taskData.fightNearTask2;
	std::shared_ptr<EnemyNearAttack3Task>		 fightNearTask3 = m_taskData.fightNearTask3;
	std::shared_ptr<EnemyFarAttack0Task>		 fightFarTask0 = m_taskData.fightFarTask0;
	std::shared_ptr<EnemyFarAttack1Task>		 fightFarTask1 = m_taskData.fightFarTask1;
	std::shared_ptr<EnemyFarAttack2Task>		 fightFarTask2 = m_taskData.fightFarTask2;
	std::shared_ptr<EnemyNearSpecialAttack0Task> fightNearSpecialTask0 = m_taskData.fightNearSpecialTask0;
	std::shared_ptr<EnemyFarSpecialAttack0Task>	 fightFarSpecialTask0 = m_taskData.fightFarSpecialTask0;

	std::shared_ptr<EnemyBehaviorNode> selectNode = m_nodes->SearchNode(restTask->GetParentNodeName());
	selectNode->SetTask(restTask);
	selectNode->SetTask(intimidateTask);

	selectNode = m_nodes->SearchNode(chaseTask->GetParentNodeName());
	selectNode->SetTask(chaseTask);
	selectNode->SetTask(walkTask);
	float direction = dynamic_cast<EnemyChaseNode*>(selectNode.get())->GetMaxDirection();
	dynamic_cast<EnemyWalkTask*>(selectNode->GetTask().at(1).get())->SetMaxDirection(direction);
	
	selectNode = m_nodes->SearchNode(fightNearTask0->GetParentNodeName());
	selectNode->SetTask(fightNearTask0);
	selectNode->SetTask(fightNearTask1);
	selectNode->SetTask(fightNearTask2);
	selectNode->SetTask(fightNearTask3);
	selectNode->SetTask(fightFarTask0);
	selectNode->SetTask(fightFarTask1);
	selectNode->SetTask(fightFarTask2);

	selectNode = m_nodes->SearchNode(fightNearSpecialTask0->GetParentNodeName());
	selectNode->SetTask(fightNearSpecialTask0);
	selectNode->SetTask(fightFarSpecialTask0);

}
