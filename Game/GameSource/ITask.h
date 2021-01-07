#pragma once
#include "PlanList.h"

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
