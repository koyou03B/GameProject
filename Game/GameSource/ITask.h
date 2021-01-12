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

#pragma region Task�ɂ���
//Task��2���݂��Ă��܂�
//PrimitiveTask��CompoundTask�ł�
//�����ł́A���ʂ��鏈�������L�q���܂�
//�܂�Task�̏W�܂��Domain�Ƃ�т܂�
//template���g�킸WorldState���g�p�����
//�X��WorldState�ɃA�N�Z�X�ł��Ȃ��̂�
//template���g�p���Ă���
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
