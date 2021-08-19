#pragma once
#include <vector>
#include <memory>
#include "CompoundTask.h"
#include "TaskBase.h"

#pragma region HTNPlanner�ɂ���
//�Ă߂�(HTNPlanner)�͉������点��
//�������S�傾�����B����N���X�̋����B���Ԃ��������B
//�����ł�TWorldState��rootTask����v���������グ��
//�悤�ɍ���Ă���B

//�ڂ��������͒���...
#pragma endregion

#pragma region PlannerState�ɂ���
//HTNPlanner�ł����g�p���Ȃ����ߒ��ɂ���Ă܂�
//�����ɂ̓v�����j���O�K�v�Ȃ��̂�Z�߂Ă��܂�
//TWorldState:�u���݁v��TWorldState
//finalPlanList:�v�l�̖����ǂ蒅�������ʂ�Task
//taskListToProcess:�v�l����Task�̈ꗗ
//nextMethodNumber:����method�͌��Ȃ��Č��\�ł�
#pragma endregion

template<class TWorldState,class TChara>
class HTNPlanner
{
public:
	HTNPlanner() = default;
	~HTNPlanner() = default;

	//���O������ŕύX
	using PlanList = std::vector<TaskBase<TWorldState, TChara>*>;
	
	struct PlannerState
	{
		TWorldState worldState;
		PlanList finalPlanList;
		PlanList taskListToProcess;
		int nextMethodNumber = 0;

		void Clear()
		{
			finalPlanList.clear();
			taskListToProcess.clear();
			nextMethodNumber = 0;
		}
	};

	PlanList& Planning(TWorldState& currentState, TaskBase<TWorldState, TChara>* rootTask)
	{
		//�����ݒ蕔���ł�
		{
			m_historyState.clear();
			m_plannerState.Clear();
			m_plannerState.worldState = currentState;
			m_plannerState.taskListToProcess.push_back(rootTask);
			m_plannerState.nextMethodNumber = 0;
		}

		//�v�l����^�X�N���X�g�������Ȃ莟��I���ł�
		while (m_plannerState.taskListToProcess.size() > 0)
		{
			TaskBase<TWorldState, TChara>* currentTask = m_plannerState.taskListToProcess[0];
			//�^�X�N��PrimitiveTask�Ȃ̂�CompoundTask�Ȃ̂���
			//�������ς��̂ŁA�܂��͂ǂ��炩���m���߂܂��B
			if (currentTask->GetTaskType() == TaskType::Compound)
			{
				//������Method�����邩�Ȃ����ŏ������ς��܂�
				Method<TWorldState, TChara>* satisfiedMethod = FindSatisfiedMethod(currentTask);
				if (satisfiedMethod)
				{
					//�����ł͎O�̍�Ƃ����܂�
					//1.historyState�ɋL�^������
					//2.taskListToProcess��0�Ԗ�(�����Ă���Task)������
					//3.subTask��taskListToProcess�ɒǉ�
					RecordDecompositionOfTask(currentTask);
					m_plannerState.taskListToProcess.erase(m_plannerState.taskListToProcess.begin());
					AddToTaskLiast(satisfiedMethod);
				}
				else
				{
					RestoreToLastDecomposedTask();
				}
			}
			else
			{
				if (currentTask->CheckPreCondition(m_plannerState.worldState))
				{
					m_plannerState.taskListToProcess.erase(m_plannerState.taskListToProcess.begin());
					m_plannerState.finalPlanList.push_back(currentTask);
					currentTask->ApplyEffects(m_plannerState.worldState);
				}
			}
		}

		return m_plannerState.finalPlanList;
	}

	//�g�p�\��Method��T���܂�
	Method<TWorldState, TChara>* FindSatisfiedMethod(
		TaskBase<TWorldState, TChara>* currentCompoundTask);

	//PlannerState��i�K�I�ɋL�^������
	void RecordDecompositionOfTask(
		TaskBase<TWorldState, TChara>* nextCompoundTask);

	//subTask��ǉ����Ă���
	void AddToTaskLiast(Method<TWorldState, TChara>* currentMethod);

	//�A�N�V�f���g�����B�ŐV�̋L�^�����o��
	void RestoreToLastDecomposedTask();

private:
	PlannerState m_plannerState;
	std::vector<PlannerState> m_historyState;
};

template<class TWorldState, class TChara>
inline Method<TWorldState, TChara>* HTNPlanner<TWorldState, TChara>::FindSatisfiedMethod(
	TaskBase<TWorldState, TChara>* currentCompoundTask)
{
	//�͂��������|�C���g! ��΂ɂ�����Č��܂��Ă��鎞��static_cast�ł��_�E���L���X�g�\�Ȃ�
	//�����ł�Method�����N���X�Ȃ��CompoundTask�����Ȃ����炱��ł����̂��悧
	auto& methods = static_cast<CompoundTask<TWorldState, TChara>*>(currentCompoundTask)->GetMethod();

	//���̒��Ŏg�p�ł���Method��T���o���Ă���̂��悧
	while (m_plannerState.nextMethodNumber < static_cast<int>(methods.size()))
	{
		auto& method = methods[m_plannerState.nextMethodNumber];
		++m_plannerState.nextMethodNumber;
		if (method->CheckPreCondition(m_plannerState.worldState))
		{
			return method.get();
		}
	}

	return nullptr;
}

template<class TWorldState, class TChara>
inline void HTNPlanner<TWorldState, TChara>::RecordDecompositionOfTask(
	TaskBase<TWorldState, TChara>* nextCompoundTask)
{
	//���݂̃v�����j���O���(PlannerState)���L�^�����Ă���
	//����ɂ��A�N�V�f���g�������Ă���O�̋L�^�����낵�Ă���
	//�ăv�����j���O���\�ɂȂ�
	m_historyState.push_back(m_plannerState);
	m_plannerState.nextMethodNumber = 0;
	//m_plannerState.taskListToProcess.push_back(nextCompoundTask);
}

template<class TWorldState, class TChara>
inline void HTNPlanner<TWorldState, TChara>::AddToTaskLiast(
	Method<TWorldState, TChara>* currentMethod)
{
	//�͂��������|�C���g! taskListToProcess�ɂ�subTask�̌�납��(2->1->0)
	//taskListToProcess�̑O�ɑ}�����Ă����킟�B�������邱�Ƃ�taskListToProcess�ɂ�
	//0->1->2�̏��œ��邩��悧�B���Ԃ��ƂĂ��厖�Ȃ̂悱���B
	//subTask��O����(0->1->2)�̏�����2->1->0�̏���taskListToProcess�ɓ��邩��
	//�u����->������������߂�->��������v�̏����ƕς��Ƃ������Ƃ��킩����˂�
	auto& subTasks = currentMethod->GetSubTask();
	int  select = static_cast<int>(subTasks.size());
	for (int i = select; i > 0; --i)
	{
		std::shared_ptr<TaskBase<TWorldState, TChara>> ptr = subTasks[i - 1].lock();
		m_plannerState.taskListToProcess.insert(
			m_plannerState.taskListToProcess.begin(), ptr.get());
	}
}

template<class TWorldState, class TChara>
inline void HTNPlanner<TWorldState, TChara>::RestoreToLastDecomposedTask()
{
	//�L�^���Ȃ��Ȃ���߂悤�I
	size_t size = m_historyState.size();
	if (size == 0)
	{
		m_plannerState.Clear();
		return;
	}

	//�ŐV�̋L�^��n���B�����ď���
	m_plannerState = m_historyState.at(size - 1);
	m_historyState.pop_back();
}
