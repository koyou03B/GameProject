#pragma once
#include "PlanList.h"

#pragma region Taskについて
//Taskは2つ存在しています
//PrimitiveTaskとCompoundTaskです
//ここでは、共通する処理だけ記述します
//またTaskの集まりをDomainとよびます
//templateを使わずWorldStateを使用すると
//個々のWorldStateにアクセスできないので
//templateを使用している
#pragma endregion
template<class WorldState>
class ITask
{
public:
	ITask() = default;
	~ITask() = default;

public:
	virtual bool TryPlanTask(WorldState& state, PlanList& planList) = 0;
};
