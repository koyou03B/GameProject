#pragma once
#include <string>
#include <memory>
#include <iostream>
#include <fstream>
#include <shlwapi.h>
#undef max
#undef min
#include<cereal/cereal.hpp>
#include<cereal/archives/binary.hpp>
#include<cereal/types/memory.hpp>
#include<cereal/types/vector.hpp>
#include <cereal/types/string.hpp>


#include ".\LibrarySource\Vector.h"
enum ENTRY_TASK
{
	REST_TASK,
	INTIMIDATE_TASK,
	WALK_TASK,
	TARGET_TURN_TASK,
	NEAR_ATTACK0_TASK,
	NEAR_ATTACK1_TASK,
	NEAR_ATTACK2_TASK,
	NEAR_ATTACK3_TASK,
	NEAR_SPECIAL_TASK,
	FAR_ATTACK0_TASK,
	FAR_ATTACK1_TASK,
	FAR_ATTACK2_TASK,
	FAR_SPECIAL_TASK
};

class Enemy;
class EnemyBehaviorTask
{
public:
	EnemyBehaviorTask() = default;
	~EnemyBehaviorTask() = default;

	enum TASK_STATE
	{
		START,
		RUN,
		END
	};

	void UpdateCoolTimer(const float elapsedTime)
	{
		m_coolTimer-=elapsedTime;
		if (m_coolTimer <= 0)
			m_isUsed = false;
	}

	virtual void Run(Enemy* enemy) = 0;

	virtual uint32_t JudgePriority(const int id, const VECTOR3F playerPos = {}) = 0;

	virtual void LoadOfBinaryFile(std::string taskName) = 0;
	virtual void SaveOfBinaryFile() = 0;

	inline std::string& GetParentNodeName() { return m_parentNodeName; }
	inline std::string& GetTaskName() { return m_taskName;}
	inline TASK_STATE& GetTaskState() { return m_taskState; }
	inline uint32_t& GetMoveState() { return m_moveState; }
	inline uint32_t& GetPriority() { return m_priority; }
	inline float& GetCoolTimer() { return m_coolTimer; }
	inline bool& GetIsUsed() { return m_isUsed; }


	inline void SetParentNodeName(const std::string& parentNodeName) { m_parentNodeName = parentNodeName; }
	inline void SetTaskName(const std::string& taskName) { m_taskName = taskName; }
	inline void SetTaskState(const TASK_STATE& taskState) { m_taskState = taskState; }
	inline void SetMoveState(const uint32_t& moveState) { m_moveState = moveState; }
	//inline void SetCoolTimer(const float& coolTime) { m_coolTimer = coolTime; }
	inline void SetPriority(const uint32_t& priority) { m_priority = priority; }
	inline void SetIsUsed(bool& isUse) { m_isUsed = isUse; }

protected:
	const uint32_t minPriority = 0;

	std::string m_parentNodeName;
	std::string m_taskName;
	TASK_STATE m_taskState;
	uint32_t m_moveState = 0;
	float m_coolTimer;
	uint32_t m_priority;
	int m_animNo;
	bool m_isUsed = false;
	uint32_t m_serialVersion = 3;
};