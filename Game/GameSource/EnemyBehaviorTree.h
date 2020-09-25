#pragma once
#include <queue>
#include "EnemyBehaviorNode.h"

class OperatCoolTime
{
public:
	bool operator() (std::shared_ptr<EnemyBehaviorTask> taskFirst, std::shared_ptr<EnemyBehaviorTask> taskSecond)
	{
		int coolTime1 = taskFirst->GetCoolTimer();
		int coolTime2 = taskSecond->GetCoolTimer();

		return (coolTime1 < coolTime2) ? true : false;
	}
};

class EnemyBehaviorTree
{
public:
	EnemyBehaviorTree() = default;
	~EnemyBehaviorTree() = default;

	void Release();

	void UpdateUseTasks();

	void AddNode(std::string parentName, std::shared_ptr<EnemyBehaviorNode> node);

	void AddUseTask(std::shared_ptr<EnemyBehaviorTask> task)
	{
		m_useTasks.push_back(task);
		if (m_useTasks.size() >= 2)
		{
			OperatCoolTime operatTime;
			std::sort(m_useTasks.begin(), m_useTasks.end(), operatTime);
		}
	}

	std::shared_ptr<EnemyBehaviorTask> SearchOfActiveTask();


private:
	std::unique_ptr<EnemyBehaviorNode> m_nodes;
	std::shared_ptr<EnemyBehaviorNode> m_sequenceNode;
	std::vector<std::shared_ptr<EnemyBehaviorTask>> m_useTasks;
};