#include <stdio.h>
#include <random>
#include "EnemyBehaviorNode.h"
#include "MessengTo.h"
void EnemyBehaviorNode::Release()
{
	if (!m_task.empty())
	{
		int taskCount = static_cast<int>(m_task.size());

		for (int i = 0; i < taskCount; ++i)
		{
			if (m_task[i].unique())
				m_task[i].reset();
		}
		m_task.clear();
	}

	if (!m_family.childs.empty())
	{
		int childCount = static_cast<int>(m_family.childs.size());

		for (int i = 0; i < childCount; ++i)
		{
			if (m_family.childs[i].unique())
				m_family.childs[i].reset();
		}
		m_family.childs.clear();
	}
}

std::shared_ptr<EnemyBehaviorNode> EnemyBehaviorNode::SelectOfActivedNode(const int id)
{
	switch (m_selectRule)
	{
	case NON:
		if (m_family.childs.size() == 1)
		{
			return m_family.childs[0];
		}
		break;
	case PRIORITY:
	{
		int childNodeCount = static_cast<int>(m_family.childs.size());
		int selectChild = 0;
		uint32_t priority = 0;
		for (int i = 0; i < childNodeCount; ++i)
		{
			uint32_t nodePriority = m_family.childs.at(i)->JudgePriority(id);
			if (priority < nodePriority)
			{
				selectChild = i;
				priority = nodePriority;
			}
		}
		//もしレストが連続するようなことが起きたら攻撃に
		if (priority == 0)
			selectChild = 2;

		return m_family.childs.at(selectChild);
	}
		break;
	case RANDOM:
	{
		int childCount = static_cast<int>(m_family.childs.size()) - 1;
		std::random_device rnd;
		std::mt19937 mt(rnd());
		std::uniform_int_distribution<> randChildCount(0, childCount);
		int selectNomber = randChildCount(mt);

		return m_family.childs[selectNomber];
	}
		break;
	default:
		break;
	}
	return nullptr;
}

std::pair<int, std::shared_ptr<EnemyBehaviorTask>> EnemyBehaviorNode::SelectOfActiveTask(const int id)
{
	switch (m_selectRule)
	{
	case NON:
		if (m_task.size() == 1)
			return std::make_pair(0,m_task[0]);
		break;
	case PRIORITY:
	{
		int childTaskCount = static_cast<int>(m_task.size());
		std::shared_ptr<EnemyBehaviorTask> selectTask;
		uint32_t priority = 0;
		EnemyType enemyType = static_cast<EnemyType>(id);
		CharacterAI* enemy = MESSENGER.CallEnemyInstance(enemyType);
		int targetID = enemy->GetJudgeElement().targetID;
		PlayerType playerType = static_cast<PlayerType>(targetID);
		CharacterAI* player = MESSENGER.CallPlayerInstance(playerType);

		VECTOR3F playerPosition = player->GetWorldTransform().position;
		for (int i = 0; i < childTaskCount; ++i)
		{
			uint32_t taskPriority = m_task.at(i)->JudgePriority(id, playerPosition);
			if (priority < taskPriority)
			{
				selectTask = m_task.at(i);
				priority = taskPriority;
			}
		}

	//	assert(selectTask);

		if (!selectTask)
		{
			int taskCount = static_cast<int>(m_task.size()) - 1;
			std::random_device rnd;
			std::mt19937 mt(rnd());
			std::uniform_int_distribution<> randTaskCount(0, taskCount);
			int selectNomber = randTaskCount(mt);
			selectTask = m_task[selectNomber];
		}
		return  std::make_pair(0,selectTask);
	}
	break;
	case RANDOM:
	{
		int taskCount = static_cast<int>(m_task.size()) - 1;
		std::random_device rnd;
		std::mt19937 mt(rnd());
		std::uniform_int_distribution<> randTaskCount(0, taskCount);
		int selectNomber = randTaskCount(mt);

		return  std::make_pair(0,m_task[selectNomber]);
	}
	break;
	case SEQUENCE:
	{
		int childTaskCount = static_cast<int>(m_task.size());
		for (int i = 0; i < childTaskCount; ++i)
		{
			if (!m_task[i]->GetIsUsed())
				return  std::make_pair(i,m_task[i]);
		}

		return  std::make_pair(0,nullptr);
	}
		break;
	default:
		break;
	}
	return std::make_pair(0, nullptr);
}

