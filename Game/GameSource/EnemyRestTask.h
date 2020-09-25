#pragma once
#include "EnemyBehaviorTask.h"

class EnemyRestTask : public EnemyBehaviorTask
{
public:
	EnemyRestTask() = default;
	~EnemyRestTask() = default;
	void Run();
	uint32_t JudgePriority();

private:
	int m_restTimer;
	int m_maxRestTimer;
};