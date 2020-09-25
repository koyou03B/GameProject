#pragma once
#include "EnemyBehaviorTask.h"

class EnemyChaseTask : public EnemyBehaviorTask
{
public:
	EnemyChaseTask() = default;
	~EnemyChaseTask() = default;
	void Run();
	uint32_t JudgePriority();

};