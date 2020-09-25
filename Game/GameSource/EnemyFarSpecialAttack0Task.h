#pragma once
#include "EnemyBehaviorTask.h"

class EnemyFarSpecialAttack0Task : public EnemyBehaviorTask
{
public:
	EnemyFarSpecialAttack0Task() = default;
	~EnemyFarSpecialAttack0Task() = default;
	void Run();
	uint32_t JudgePriority();
};