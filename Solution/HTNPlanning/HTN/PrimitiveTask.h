#pragma once
#include "TaskBase.h"

#pragma region PrimitiveTask�ɂ���
//PrimitiveTask��TaskBase�ɋL�ڂ��Ă���
//Precondition,Effect,Operator�ō\������Ă���

//TWorldState��TWorldState���p�������N���X���w�肵�Ă�������
//�������邱�ƂŊe�L�����N�^�[���Ƃ�TWorldState�����邱�Ƃ��ł��܂�

//TChara�ł̓L�����N�^�[�̃N���X���w�肵�Ă�������
//�������邱�ƂŊe�L�����N�^�[�̍s�������s�ł��܂�
#pragma endregion
template<class TWorldState,class TChara>
class PrimitiveTask : public TaskBase<TWorldState,TChara>
{
public:
	PrimitiveTask() = default;
	virtual ~PrimitiveTask() = default;

	//�^�X�N�����s�ł��邩
	bool CheckPreCondition(const TWorldState& state) override	
	{ return IsSatisfiedPreConditions(state);}

	//�^�X�N�����s�����ꍇ�̉e����
	void ApplyEffects(TWorldState& state) override
	{ ApplyEffectsToWorldState(state); }

	//�^�X�N�����s����
	bool ExecuteOperator(TChara* chara) override
	{ return Execute(chara); }

	inline TaskType GetTaskType() override { return m_taskType; }
	inline void SetTaskType(TaskType& type) override { m_taskType = type; }

	inline std::string GetTaskName() override { return m_taskName; }
	inline void SetTaskName(std::string taskName) override { m_taskName = taskName; }

	//�^��ActFunc�ň���TWorldState�̊֐��|�C���^
	typedef bool (TChara::* ActFunc)();
	inline void SetOperator(ActFunc func) { m_func = func; }

protected:
	//���̃N���X���p�������N���X�ŏڂ����O��������L�ڂ��Ă��炢�܂�
	//�܂��AEffect�����l�ł��B
	virtual bool IsSatisfiedPreConditions(const TWorldState& currentState) { return true; }
	virtual void ApplyEffectsToWorldState(TWorldState& previousState) { return; };

	std::string m_taskName;
	TaskType m_taskType = TaskType::Non;
private:
	bool Execute(TChara* chara)
	{
		return (chara->*m_func)();
	}
	ActFunc m_func;
};


