#pragma once
#include "EnemyBehaviorNode.h"
#include "EnemyNodeAll.h"
#include "EnemyTaskAll.h"

struct NodeData
{
	std::shared_ptr<EnemyWaitNode>				waitNode;
	std::shared_ptr<EnemyChaseNode>				chaseNode;
	std::shared_ptr<EnemyFightNode>				fightNode;
	std::shared_ptr<EnemyFightNearNode>			fightNearNode;
	std::shared_ptr<EnemyFightFarNode>			fightFarNode;
	std::shared_ptr<EnemySpecialAttackNode>		specialAttackNode;
	std::shared_ptr<EnemyUnSpecialAttackNode>	unSpecialAttackNode;
	const int MAX_NODE_COUNT = 7;

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
		case SPECIAL_ATTACK:
			if (specialAttackNode == nullptr)
				specialAttackNode = std::make_shared<EnemySpecialAttackNode>();
			break;
		case UNSPECIAL_ATTACK:
			if (unSpecialAttackNode == nullptr)
				unSpecialAttackNode = std::make_shared<EnemyUnSpecialAttackNode>();
			break;
		default:
			break;
		}
	}

	void Release()
	{
		waitNode.reset();
		chaseNode.reset();
		fightNode.reset();
		fightNearNode.reset();
		fightFarNode.reset();
		specialAttackNode.reset();
		unSpecialAttackNode.reset();
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
				fightFarNode,
				specialAttackNode,
				unSpecialAttackNode
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
	uint32_t serialVersion = 3;
};

struct TaskData
{
	std::shared_ptr<EnemyRestTask>					restTask;
	std::shared_ptr<EnemyIntimidateTask>			intimidateTask;
	std::shared_ptr<EnemyWalkTask>					walkTask;
	std::shared_ptr<EnemyTargetTurnTask>			targetTurnTask;
	std::shared_ptr<EnemyNearAttack0Task>			fightNearTask0;
	std::shared_ptr<EnemyNearAttack1Task>			fightNearTask1;
	std::shared_ptr<EnemyNearAttack2Task>			fightNearTask2;
	std::shared_ptr<EnemyNearAttack3Task>			fightNearTask3;
	std::shared_ptr<EnemyNearSpecialAttack0Task>	fightNearSpecialTask0;
	std::shared_ptr<EnemyFarAttack0Task>			fightFarTask0;
	std::shared_ptr<EnemyFarAttack1Task>			fightFarTask1;
	std::shared_ptr<EnemyFarAttack2Task>			fightFarTask2;
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
		case WALK_TASK:
			if (walkTask == nullptr)
				walkTask = std::make_shared<EnemyWalkTask>();
			break;
		case TARGET_TURN_TASK:
			if (targetTurnTask == nullptr)
				targetTurnTask = std::make_shared<EnemyTargetTurnTask>();
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
		case NEAR_ATTACK3_TASK:
			if (fightNearTask3 == nullptr)
				fightNearTask3 = std::make_shared<EnemyNearAttack3Task>();
			break;
		case NEAR_SPECIAL_TASK:
			if (fightNearSpecialTask0 == nullptr)
				fightNearSpecialTask0 = std::make_shared<EnemyNearSpecialAttack0Task>();
			break;
		case FAR_ATTACK0_TASK:
			if (fightFarTask0 == nullptr)
				fightFarTask0 = std::make_shared<EnemyFarAttack0Task>();
			break;
		case FAR_ATTACK1_TASK:
			if (fightFarTask1 == nullptr)
				fightFarTask1 = std::make_shared<EnemyFarAttack1Task>();
			break;
		case FAR_ATTACK2_TASK:
			if (fightFarTask2 == nullptr)
				fightFarTask2 = std::make_shared<EnemyFarAttack2Task>();
			break;
		case FAR_SPECIAL_TASK:
			if (fightFarSpecialTask0 == nullptr)
				fightFarSpecialTask0 = std::make_shared<EnemyFarSpecialAttack0Task>();
			break;
		default:
			break;
		}
	}

	void Release()
	{
		restTask.reset();
		intimidateTask.reset();
		walkTask.reset();
		targetTurnTask.reset();
		fightNearTask0.reset();
		fightNearTask1.reset();
		fightNearTask2.reset();
		fightNearTask3.reset();
		fightNearSpecialTask0.reset();
		fightFarTask0.reset();
		fightFarTask1.reset();
		fightFarTask2.reset();
		fightFarSpecialTask0.reset();
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
				targetTurnTask,
				fightNearTask0,
				fightNearTask1,
				fightNearTask2,
				fightNearSpecialTask0,
				fightFarTask0,
				fightFarTask1,
				fightFarSpecialTask0,
				walkTask,
				fightNearTask3,
				fightFarTask2
			);
		}
		else
		{
			archive
			(
				restTask,
				intimidateTask,
				targetTurnTask,
				fightNearTask0,
				fightNearTask1,
				fightNearTask2,
				fightNearSpecialTask0,
				fightFarTask0,
				fightFarTask1,
				fightFarSpecialTask0,
				walkTask
			);
		}
	}

private:
	uint32_t serialVersion = 4;
};

class OperatCoolTime
{
public:
	bool operator() (std::shared_ptr<EnemyBehaviorTask> taskFirst, std::shared_ptr<EnemyBehaviorTask> taskSecond)
	{
		float coolTime1 = taskFirst->GetCoolTimer();
		float coolTime2 = taskSecond->GetCoolTimer();

		return (coolTime1 < coolTime2) ? true : false;
	}
};

class EnemyBehaviorTree
{
public:
	EnemyBehaviorTree() = default;
	~EnemyBehaviorTree() = default;

	void Release();

	void UpdateUseTasks(const float elapsedTime);

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
	inline std::vector<std::shared_ptr<EnemyBehaviorTask>>& GetUseTask() { return m_useTasks; }
	void SetRootNodeChild();
	void SetTaskToNode();

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
				m_nodeData,
				m_taskData
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
CEREAL_CLASS_VERSION(NodeData, 3);
CEREAL_CLASS_VERSION(TaskData, 4);
