#pragma once
#include "HTN\Method.h"

template<class TWorldState, class TChara>
class HealMethod : public Method<TWorldState, TChara>
{
public:
	HealMethod() = default;
	~HealMethod() = default;
	HealMethod(std::vector<std::shared_ptr<PreconditionBase<TWorldState>>> preconditions,
		std::vector<std::shared_ptr<TaskBase<TWorldState, TChara>>> subTasks)
	{
		this->SetPreconditions(preconditions);
		this->SetSubTasks(subTasks);
	}
};