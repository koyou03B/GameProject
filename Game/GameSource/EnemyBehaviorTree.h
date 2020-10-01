#pragma once
#include <queue>
#include "EnemyBehaviorNode.h"
#include "EnemyNodeAll.h"

struct NodeData
{
	std::shared_ptr<EnemyWaitNode>			waitNode;
	std::shared_ptr<EnemyChaseNode>			chaseNode;
	std::shared_ptr<EnemyFightNode>			fightNode;
	std::shared_ptr<EnemyFightNearNode>		fightNearNode;
	std::shared_ptr<EnemyFightFarNode>		fightFarNode;

	const int MAX_NODE_COUNT = 5;

	void CreateNodeData(ENTRY_NODE node)
	{
		switch (node)
		{
		case WAIT_NODE:
			if (waitNode == nullptr)
				waitNode = std::make_shared<EnemyWaitNode>();
			break;
		case CHASE_NODE:
			if (chaseNode == nullptr)
				chaseNode = std::make_shared<EnemyChaseNode>();
			break;
		case FIGHT_NODE:
			if (fightNode == nullptr)
				fightNode = std::make_shared<EnemyFightNode>();
			break;
		case FIGHT_NEAR_NODE:
			if (fightNearNode == nullptr)
				fightNearNode = std::make_shared<EnemyFightNearNode>();
			break;
		case FIGHT_FAR_NODE:
			if (fightFarNode == nullptr)
				fightFarNode = std::make_shared<EnemyFightFarNode>();
			break;
		default:
			break;
		}
	}

	inline std::shared_ptr<EnemyWaitNode> GetWaitNode() { return waitNode; };
	inline std::shared_ptr<EnemyChaseNode> GetChaseNode() { return chaseNode; };
	inline std::shared_ptr<EnemyFightNode> GetFightNode() { return fightNode; };
	inline std::shared_ptr<EnemyFightNearNode> GetFightNearNode() { return fightNearNode; };
	inline std::shared_ptr<EnemyFightFarNode> GetFightFarNode() { return fightFarNode; };


	template<class T>
	void serialize(T& archive, const std::uint32_t version)
	{
		if (serialVersion <= version)
		{
			archive
			(
				waitNode,
				chaseNode,
				fightNode,
				fightNearNode,
				fightFarNode
			);
		}
		else
		{
			archive
			(
				waitNode,
				chaseNode,
				fightNode,
				fightNearNode,
				fightFarNode
			);
		}
	}

private:
	uint32_t serialVersion;
};

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

	void CreateRootNode()
	{
		m_nodes = std::make_unique<EnemyRootNode>();
	}

	std::shared_ptr<EnemyBehaviorTask> SearchOfActiveTask(const int id);

	inline NodeData& GetNodeData() { return m_nodeDatas; }
	inline std::unique_ptr<EnemyRootNode>& GetRootNode() { return m_nodes; }

	void SetRootNodeChild();


	template<class T>
	void serialize(T& archive, const std::uint32_t version)
	{
		if (m_serialVersion <= version)
		{
			archive
			(
				m_nodes,
				m_nodeDatas
			);
		}
		else
		{
			archive
			(
				m_nodes,
				m_nodeDatas
			);
		}
	}

private:
	std::unique_ptr<EnemyRootNode> m_nodes;
	std::shared_ptr<EnemyBehaviorNode> m_sequenceNode;
	std::vector<std::shared_ptr<EnemyBehaviorTask>> m_useTasks;
	NodeData m_nodeDatas;
	uint32_t m_serialVersion = 1;
};

CEREAL_CLASS_VERSION(EnemyBehaviorTree, 1);
