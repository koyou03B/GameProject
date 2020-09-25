#pragma once
#include <memory>
#include <vector>
#include "EnemyBehaviorTask.h"

enum  SELECT_RULE
{
	NON,
	PRIORITY,
	RANDOM,
	SEQUENCE
};

class EnemyBehaviorNode;

struct FamilyNode
{
	EnemyBehaviorNode* parent;
	std::vector<std::shared_ptr<EnemyBehaviorNode>> childs;
};

class EnemyBehaviorNode
{
public:
	EnemyBehaviorNode() = default;
	~EnemyBehaviorNode() = default;

	void Release();

	std::shared_ptr<EnemyBehaviorNode> SelectOfActivedNode();
	std::pair<int,std::shared_ptr<EnemyBehaviorTask>> SelectOfActiveTask();

	virtual uint32_t JudgePriority() = 0;

	std::shared_ptr<EnemyBehaviorNode> SearchNode(std::string nodeName)
	{
		int childCount = static_cast<int>(m_family.childs.size());
		for (int i = 0; i < childCount; ++i)
		{
			if (m_family.childs[i]->GetNodeName() == nodeName)
				return m_family.childs[i];
		}

		return nullptr;
	}

	inline std::string& GetNodeName() { return m_nodeName; }
	inline SELECT_RULE& GetSelectRule() { return m_selectRule; }
	inline EnemyBehaviorNode& GetParent() { return *m_family.parent; }
	inline std::vector<std::shared_ptr<EnemyBehaviorNode>>& GetChild() { return m_family.childs; }
	inline std::vector<std::shared_ptr<EnemyBehaviorTask>>& GetTask() { return m_task; }
	inline uint32_t& GetPriority() { return m_priority; }


	inline void SetNodeName(std::string& nodeName) { m_nodeName = nodeName; }
	inline void SetSelectRule(SELECT_RULE& rule) { m_selectRule = rule;}
	inline void SetChildNode(std::shared_ptr<EnemyBehaviorNode> childNode) { m_family.childs.push_back(childNode); }
	inline void SetParentNode(EnemyBehaviorNode* parent) { m_family.parent = parent; }
	inline void SetTask(std::shared_ptr<EnemyBehaviorTask> task) { m_task.push_back(task); }

protected:
	std::string m_nodeName;
	SELECT_RULE m_selectRule;
	FamilyNode m_family;
	std::vector<std::shared_ptr<EnemyBehaviorTask>> m_task;
	uint32_t m_priority;
};