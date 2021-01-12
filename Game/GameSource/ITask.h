#pragma once
#include "PlanList.h"

#ifdef _DEBUG
#include "..\External_libraries\imgui\imgui.h"
#include "..\External_libraries\imgui\imgui_impl_dx11.h"
#include "..\External_libraries\imgui\imgui_impl_win32.h"
#include "..\External_libraries\imgui\imgui_internal.h"
#endif

#undef max
#undef min
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/string.hpp>

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
