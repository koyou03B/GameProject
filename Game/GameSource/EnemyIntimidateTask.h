#pragma once
#include "EnemyBehaviorTask.h"

class EnemyIntimidateTask : public EnemyBehaviorTask
{
public:
	EnemyIntimidateTask() = default;
	~EnemyIntimidateTask() = default;
	void Run();
	uint32_t JudgePriority();

};