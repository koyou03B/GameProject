#pragma once
#include "HTN\Method.h"

template<class TWorldState, class TChara>
class PrepareAttackMethod : public Method<TWorldState, TChara>
{
public:
	PrepareAttackMethod() = default;
	~PrepareAttackMethod() = default;
	PrepareAttackMethod(std::vector<std::shared_ptr<PreconditionBase<TWorldState>>> preconditions,
		std::vector<std::shared_ptr<TaskBase<TWorldState, TChara>>> subTasks)
	{
		this->SetPreconditions(preconditions);
		this->SetSubTasks(subTasks);
	}
};