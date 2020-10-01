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

	//m_nodes->Release();
}

void EnemyBehaviorTree::UpdateUseTasks()
{
	for (auto& task : m_useTasks)
	{
		task->UpdateCoolTimer();
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
	while (!selectNode->GetTask().empty())
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
	std::shared_ptr<EnemyWaitNode>		waitNode		 = m_nodeDatas.GetWaitNode();
	std::shared_ptr<EnemyChaseNode>		chaseNode		 = m_nodeDatas.GetChaseNode();
	std::shared_ptr<EnemyFightNode>		fightNode		 = m_nodeDatas.GetFightNode();
	std::shared_ptr<EnemyFightNearNode>	fightNearNode	 = m_nodeDatas.GetFightNearNode();
	std::shared_ptr<EnemyFightFarNode>  fightFarNode	 = m_nodeDatas.GetFightFarNode();

	AddNode(waitNode->GetParentName(),		waitNode);
	AddNode(chaseNode->GetParentName(),		chaseNode);
	AddNode(fightNode->GetParentName(),		fightNode);
	AddNode(fightNearNode->GetParentName(), fightNearNode);
	AddNode(fightFarNode->GetParentName(),	fightFarNode);

}
