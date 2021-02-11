#pragma once
#include "HTN\Method.h"

template<class TWorldState, class TChara>
class FindDAMethod : public Method<TWorldState, TChara>
{
public:
	FindDAMethod() = default;
	~FindDAMethod() = default;
	FindDAMethod(std::vector<std::shared_ptr<PreconditionBase<TWorldState>>> preconditions,
		std::vector<std::shared_ptr<TaskBase<TWorldState, TChara>>> subTasks)
	{
		this->SetPreconditions(preconditions);
		this->SetSubTasks(subTasks);
	}
};