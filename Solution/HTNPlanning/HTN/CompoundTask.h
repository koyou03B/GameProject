#pragma once
#include "TaskBase.h"
#include "Method.h"

#pragma region CompoundTask�ɂ���
//CompoundTask��Method�𕡐������܂��B
//Method�Ƃ����̂�Precondition��Task�������܂��B

//���z�֐�����K�v�Ȃ��̂����L�ڂ��܂��B
//Precondition,Effect,Operator��CompoundTask�ɂ͕K�v����܂���B
//�ł���Όp������Ȃƌ��������Ȃ�Ǝv���܂����A�v�����j���O�̂Ƃ���
//���X����̂Ōp�������܂��B

//CompoundTask�͕����̓����̒��(Method)��
//���N���X�Ƃ����΂킩��₷���ł��傤��
#pragma endregion

template<class TWorldState,class TChara>
class CompoundTask : public TaskBase<TWorldState, TChara>
{
public:
	CompoundTask() = default;
	virtual ~CompoundTask() = default;

	inline TaskType GetTaskType() override { return m_taskType; }
	inline void SetTaskType(TaskType& type) override { m_taskType = type; }

	inline std::string GetTaskName() override { return m_taskName; }
	inline void SetTaskName(std::string taskName) override { m_taskName = taskName; }

	std::vector<std::shared_ptr<Method<TWorldState, TChara>>>& GetMethod() { return m_methods; }
	void AddMethod(std::shared_ptr<Method<TWorldState, TChara>> method) { m_methods.push_back(method); }

protected:
	std::vector<std::shared_ptr<Method<TWorldState, TChara>>> m_methods;
	std::string m_taskName;
	TaskType m_taskType = TaskType::Non;
};