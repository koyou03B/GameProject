#pragma once
#include <string>


enum TaskType
{
	Primitive,
	Compound,
	Non
};

#pragma region TaskBase�ɂ���
//TaskBase��PrimitiveTask��CompoundTask�Ƃ�����
//Task�̊��ɂ�����N���X�B���̃N���X��ʂ���
//Planner�Ƀv�����j���O���Ă��炤�B
//Precondition,Effect,Operator�̊e�@�\�������Ă���B

//Precondition��Task���s���ɒl�����Ƃ������Ƃ����O������̖���
//Effect��Task���s�����ۂɂǂ��������e�����o�邩�Ƃ�������
//Operator�͎��ۂɍs���s����S���Ă���

//TWorldState�Ƃ����͎̂����̏�Ԃ��ӂ��߂��Q�[���̐��E�S�̂̂���
#pragma endregion
template<class TWorldState, class TChara>
class TaskBase
{
public:
	TaskBase() = default;
	virtual ~TaskBase() = default;
	
	virtual inline TaskType GetTaskType() = 0;
	virtual inline void SetTaskType(TaskType& type) = 0;

	virtual inline std::string GetTaskName() = 0;
	virtual inline void SetTaskName(std::string taskName) = 0;

	virtual bool CheckPreCondition(const TWorldState& state) { return true; }
	virtual void ApplyEffects(TWorldState& state) { return ; }
	virtual bool ExecuteOperator(TChara* chara) { return true; }
};