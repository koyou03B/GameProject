#pragma once
#include <vector>
#include <memory>
#include "PreconditionBase.h"
#include "TaskBase.h"

#pragma region Method�ɂ���
//Method�Ƃ��͕̂�����Task��Precondition���������Ă��܂�
//����́u�����̏���(Precondition)�������Ă���
//����(Task)�ł��܂���?�v��CompoundTask�ɃA�s�[�����Ă���
//���čl�������C���[�W���Ă��ꂽ��킩��₷�����ȂƎv���Ă܂��B

//Task��PrimitiveTask�ł�CompoundTask�ł������ł�
//���̂Ƃ��Ɋ�ꂪ�������Ə������ł���`
#pragma endregion
template<class TWorldState,class TChara>
class Method
{
public:
	Method() = default;
	~Method() = default;

	//Precondition�𖞂����Ă��邩�𔻒f���܂�
	//�������Ă��Ȃ��̂ł���΂���Method�͎g�p�s��
	bool CheckPreCondition(TWorldState& state)
	{
		size_t count = m_preconditions.size();
		for (size_t i = 0; i < count; ++i)
		{
			if (m_preconditions.at(i)->CheckPreCondition(state))
				return true;
		}
		return false;
	}
	std::vector<std::weak_ptr<TaskBase<TWorldState, TChara>>>& GetSubTask() { return m_subTasks; }

	inline void AddSubTask(std::shared_ptr<TaskBase<TWorldState, TChara>> subTask) { m_subTasks.push_back(subTask); }
	inline void AddPrecondition(std::shared_ptr<PreconditionBase<TWorldState>> precondition) { m_preconditions.push_back(precondition); }

	inline void SetSubTasks(std::vector<std::shared_ptr<TaskBase<TWorldState, TChara>>> subTasks)
	{
		m_subTasks = subTasks;
	}

	inline void SetPreconditions(std::vector<std::shared_ptr<PreconditionBase<TWorldState>>> preconditions)
	{
		m_preconditions = preconditions;
	}

protected:
	std::vector<std::weak_ptr<TaskBase<TWorldState, TChara>>> m_subTasks;
	std::vector<std::shared_ptr<PreconditionBase<TWorldState>>> m_preconditions;
};