#pragma once
#include "HTN\Method.h"

template<class TWorldState, class TChara>
class AttackMethod : public Method<TWorldState, TChara>
{
public:
	AttackMethod() = default;
	~AttackMethod() = default;
	AttackMethod(std::vector<std::shared_ptr<PreconditionBase<TWorldState>>> preconditions,
		std::vector<std::shared_ptr<TaskBase<TWorldState, TChara>>> subTasks)
	{
		this->SetPreconditions(preconditions);
		this->SetSubTasks(subTasks);
	}
};