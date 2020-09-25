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

	m_nodes->Release();
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
		node->SetParentNode(m_nodes.get());
		m_nodes->SetChildNode(node);
	}
	else
	{
		std::shared_ptr<EnemyBehaviorNode> selectNode = m_nodes->SearchNode(parentName);
		node->SetParentNode(&(*selectNode));
		selectNode->SetChildNode(node);
	}
}

std::shared_ptr<EnemyBehaviorTask> EnemyBehaviorTree::SearchOfActiveTask()
{
	std::shared_ptr<EnemyBehaviorNode> selectNode;

	if (!m_sequenceNode)
	{
		selectNode = m_nodes->SelectOfActivedNode();

		if (selectNode->GetSelectRule() == SELECT_RULE::SEQUENCE)
			m_sequenceNode = selectNode;
	}

	if (m_sequenceNode)
	{
		int taskCount = static_cast<int>(m_sequenceNode->GetTask().size());
		std::pair<int,std::shared_ptr<EnemyBehaviorTask>> selectTask = m_sequenceNode->SelectOfActiveTask();
		if (selectTask.first == taskCount)
		{
			m_sequenceNode.reset();
			return selectTask.second;
		}

	}
	else
	{
		std::pair<int, std::shared_ptr<EnemyBehaviorTask>> selectTask = selectNode->SelectOfActiveTask();
		return selectTask.second;
	}

	return nullptr;
}
