#pragma once
#include "EnemyBehaviorNode.h"
#include "EnemyNodeAll.h"
#include "EnemyTaskAll.h"

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
	uint32_t serialVersion = 2;
};

struct TaskData
{
	std::shared_ptr<EnemyRestTask>					restTask;
	std::shared_ptr<EnemyIntimidateTask>			intimidateTask;
	std::shared_ptr<EnemyChaseTask>					chaseTask;
	std::shared_ptr<EnemyNearAttack0Task>			fightNearTask0;
	std::shared_ptr<EnemyNearAttack1Task>			fightNearTask1;
	std::shared_ptr<EnemyNearAttack2Task>			fightNearTask2;
	std::shared_ptr<EnemyNearSpecialAttack0Task>	fightNearSpecialTask0;
	std::shared_ptr<EnemyFarAttack0Task>			fightFarTask0;
	std::shared_ptr<EnemyFarAttack1Task>			fightFarTask1;
	std::shared_ptr<EnemyFarSpecialAttack0Task>		fightFarSpecialTask0;

	const int MAX_TASK_COUNT = 10;

	void CreateTaskData(ENTRY_TASK task)
	{
		switch (task)
		{
		case REST_TASK:
			if (restTask == nullptr)
				restTask = std::make_shared<EnemyRestTask>();
			break;
		case INTIMIDATE_TASK:
			if (intimidateTask == nullptr)
				intimidateTask = std::make_shared<EnemyIntimidateTask>();
			break;
		case CHASE_TASK:
			if (chaseTask == nullptr)
				chaseTask = std::make_shared<EnemyChaseTask>();
			break;
		case NEAR_ATTACK0_TASK:
			if (fightNearTask0 == nullptr)
				fightNearTask0 = std::make_shared<EnemyNearAttack0Task>();
			break;
		case NEAR_ATTACK1_TASK:
			if (fightNearTask1 == nullptr)
				fightNearTask1 = std::make_shared<EnemyNearAttack1Task>();
			break;
		case NEAR_ATTACK2_TASK:
			if (fightNearTask2 == nullptr)
				fightNearTask2 = std::make_shared<EnemyNearAttack2Task>();
			break;
		case Near_Special_TASK:
			if (fightNearSpecialTask0 == nullptr)
				fightNearSpecialTask0 = std::make_shared<EnemyNearSpecialAttack0Task>();
			break;
		case FAR_ATTACK0_TASK:
			if (fightFarTask0 == nullptr)
				fightFarTask0 = std::make_shared<EnemyFarAttack0Task>();
			break;
		case FAR_ATTACK1_TASK:
			if (fightFarTask0 == nullptr)
				fightFarTask1 = std::make_shared<EnemyFarAttack1Task>();
			break;
		case FAR_SPECIAL_TASK:
			if (fightFarSpecialTask0 == nullptr)
				fightFarSpecialTask0 = std::make_shared<EnemyFarSpecialAttack0Task>();
			break;
		default:
			break;
		}
	}

	template<class T>
	void serialize(T& archive, const std::uint32_t version)
	{
		if (serialVersion <= version)
		{
			archive
			(
				restTask,
				intimidateTask,
				chaseTask,
				fightNearTask0,
				fightNearTask1,
				fightNearTask2,
				fightNearSpecialTask0,
				fightFarTask0,
				fightFarTask1,
				fightFarSpecialTask0
			);
		}
		else
		{
			archive
			(
				restTask,
				intimidateTask,
				chaseTask,
				fightNearTask0,
				fightNearTask1,
				fightNearTask2,
				fightNearSpecialTask0,
				fightFarTask0,
				fightFarTask1,
				fightFarSpecialTask0
			);
		}
	}

private:
	uint32_t serialVersion = 2;
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

	inline NodeData& GetNodeData() { return m_nodeData; }
	inline TaskData& GetTaskData() { return m_taskData; }
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
				m_nodeData,
				m_taskData
			);
		}
		else
		{
			archive
			(
				m_nodes,
				m_nodeData
			);
		}
	}

private:
	std::unique_ptr<EnemyRootNode> m_nodes;
	std::shared_ptr<EnemyBehaviorNode> m_sequenceNode;
	std::vector<std::shared_ptr<EnemyBehaviorTask>> m_useTasks;
	NodeData m_nodeData;
	TaskData m_taskData;
	uint32_t m_serialVersion = 2;
};

CEREAL_CLASS_VERSION(EnemyBehaviorTree, 2);
CEREAL_CLASS_VERSION(NodeData, 2);
CEREAL_CLASS_VERSION(TaskData, 2);
