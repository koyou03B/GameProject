#pragma once
#include "HTN\Method.h"

template<class TWorldState, class TChara>
class FindAttackPointMethod : public Method<TWorldState, TChara>
{
public:
	FindAttackPointMethod() = default;
	~FindAttackPointMethod() = default;
	FindAttackPointMethod(std::vector<std::shared_ptr<PreconditionBase<TWorldState>>> preconditions,
		std::vector<std::shared_ptr<TaskBase<TWorldState, TChara>>> subTasks)
	{
		this->SetPreconditions(preconditions);
		this->SetSubTasks(subTasks);
	}
};