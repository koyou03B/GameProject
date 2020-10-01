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

class Enemy;
class EnemyBehaviorTask
{
public:
	EnemyBehaviorTask() = default;
	~EnemyBehaviorTask() = default;

	enum TaskState
	{
		START,
		RUN,
		END
	};

	void UpdateCoolTimer()
	{
		--m_coolTimer;
		if (m_coolTimer == 0)
			m_isUsed = false;
	}

	virtual void Run(Enemy* enemy) = 0;

	virtual uint32_t JudgePriority(const int id) = 0;

	virtual void LoadOfBinaryFile(std::string taskName) = 0;
	virtual void SaveOfBinaryFile() = 0;

	inline std::string& GetTaskName() { return m_taskName;}
	inline TaskState& GetTaskState() { return m_taskState; }
	inline uint32_t& GetMoveState() { return m_moveState; }
	inline uint32_t& GetCoolTimer() { return m_coolTimer; }
	inline bool& GetIsUsed() { return m_isUsed; }

	inline void SetTaskName(std::string& taskName) { m_taskName = taskName; }
	inline void SetTaskState(TaskState& taskState) { m_taskState = taskState; }
	inline void SetMoveState(uint32_t& moveState) { m_moveState = moveState; }
	inline void SetCoolTimer(uint32_t& coolTime) { m_coolTimer = coolTime; }
	inline void SetIsUsed(bool& isUse) { m_isUsed = isUse; }

protected:
	const uint32_t maxPriority = 1;
	const uint32_t minPriority = 0;

	std::string m_taskName;
	TaskState m_taskState;
	uint32_t m_moveState;
	uint32_t m_coolTimer;
	uint32_t m_priority;
	bool m_isUsed;
	uint32_t m_serialVersion;
};