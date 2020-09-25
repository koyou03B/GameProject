#pragma once
#include "EnemyBehaviorTask.h"

class EnemyNearSpecialAttack0Task : public EnemyBehaviorTask
{
public:
	EnemyNearSpecialAttack0Task() = default;
	~EnemyNearSpecialAttack0Task() = default;
	void Run();
	uint32_t JudgePriority();
};