#pragma once
#include <memory>
#include <vector>
#include "EnemyBehaviorTask.h"
#include "CharacterAI.h"



enum  SELECT_RULE
{
	NON,
	PRIORITY,
	RANDOM,
	SEQUENCE
};

enum ENTRY_NODE
{
	WAIT_NODE,
	CHASE_NODE,
	FIGHT_NODE,
	FIGHT_NEAR_NODE,
	FIGHT_FAR_NODE,
	SPECIAL_ATTACK,
	UNSPECIAL_ATTACK
};

class EnemyBehaviorNode;

struct FamilyNode
{
	std::string parentName;
	std::vector<std::shared_ptr<EnemyBehaviorNode>> childs;

	template<class T>
	void serialize(T& archive, const std::uint32_t version)
	{
		if (m_serialVersion <= version)
		{
			archive
			(
				parentName
			);
		}
		else
		{
			archive
			(
				parentName
			);
		}
	}

private:
	uint32_t m_serialVersion = 0;

};

class EnemyBehaviorNode
{
public:
	EnemyBehaviorNode() = default;
	~EnemyBehaviorNode() = default;

	void Release();

	virtual uint32_t JudgePriority(const int id) = 0;

	std::shared_ptr<EnemyBehaviorNode> SelectOfActivedNode(const int id);
	std::pair<int,std::shared_ptr<EnemyBehaviorTask>> SelectOfActiveTask(const int id);
	
	std::shared_ptr<EnemyBehaviorNode> SearchNode(std::string nodeName)
	{
		int childCount = static_cast<int>(m_family.childs.size());
		for (int i = 0; i < childCount; ++i)
		{
			if (!m_family.childs[i]->GetChild().empty())
			{
				std::shared_ptr<EnemyBehaviorNode> selectNode = m_family.childs[i]->SearchNode(nodeName);
				if (selectNode != nullptr)
					return selectNode;
			}
			if (m_family.childs[i]->GetNodeName() == nodeName)
				return m_family.childs[i];
		}

		return nullptr;
	}

	std::shared_ptr<EnemyBehaviorTask> SearchTask(std::string taskName)
	{
		int taskCount = static_cast<int>(m_task.size());
		for (int i = 0; i < taskCount; ++i)
		{
			if (m_task[i]->GetTaskName() == taskName)
				return m_task[i];
		}

		return nullptr;
	}

	virtual void LoadOfBinaryFile(std::string nodeName) = 0;
	virtual void SaveOfBinaryFile() = 0;

	inline std::string& GetNodeName() { return m_nodeName; }
	inline SELECT_RULE& GetSelectRule() { return m_selectRule; }
	inline std::string& GetParentName() { return m_family.parentName; }
	inline std::vector<std::shared_ptr<EnemyBehaviorNode>>& GetChild() { return m_family.childs; }
	inline std::vector<std::shared_ptr<EnemyBehaviorTask>>& GetTask() { return m_task; }
	inline uint32_t& GetPriority() { return m_priority; }

	inline void SetNodeName(const std::string& nodeName) { m_nodeName = nodeName; }
	inline void SetSelectRule(const SELECT_RULE& rule) { m_selectRule = rule;}
	inline void SetChildNode(const std::shared_ptr<EnemyBehaviorNode> childNode) { m_family.childs.push_back(childNode); }
	inline void SetParentName(const std::string& parentName) { m_family.parentName = parentName; }
	inline void SetTask(const std::shared_ptr<EnemyBehaviorTask> task) { m_task.push_back(task); }
	inline void SetPriority(const uint32_t& priority) { m_priority = priority; }

protected:
	const uint32_t minPriority = 0;

	std::string m_nodeName = "";
	SELECT_RULE m_selectRule = SELECT_RULE::NON;
	FamilyNode m_family;
	std::vector<std::shared_ptr<EnemyBehaviorTask>> m_task;
	uint32_t m_priority =0;
	uint32_t m_serialVersion = 3;

};