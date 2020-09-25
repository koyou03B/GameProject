#pragma once
#include <string>

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

	virtual void Run() = 0;

	virtual uint32_t JudgePriority() = 0;

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
private:
	std::string m_taskName;
	TaskState m_taskState;
	uint32_t m_moveState;
	uint32_t m_coolTimer;
	bool m_isUsed;
};