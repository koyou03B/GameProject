#pragma once
#include <vector>
#include <memory>
#include "ITask.h"
#include "ArcherWorldState.h"
#include "PreCondition.h"
#include "PrimitiveTask.h"

#pragma region Methodについて
//Methodは自分が選ばれるための
//コンディションと選ばれた時の
//Taskのリストを持つ
//CompoundTaskがMethodを持つ
#pragma endregion
template<class State>
class Method
{
public:
	Method() = default;
	~Method() = default;

	void AddTask(ITask<State>* state)
	{
		std::shared_ptr<ITask<State>> uState(state);
		m_subTasks.push_back(uState);
	}

	bool CheckPreCondition(State state)
	{
		size_t count = m_preconditions.size();
		for (size_t i = 0; i < count; ++i)
		{
			if (!m_preconditions[i]->CheckPreCondition(state))
				return  false;
		}
		return true;
	}

	bool TryPlanSubTasks(State& state, PlanList& planList)
	{
		size_t count = m_subTasks.size();
		for (size_t i = 0; i < count; ++i)
		{
			if (!m_subTasks[i]->TryPlanTask(state, planList))
			{
				return false;
			}
		}

		return true;
	}

	inline void ToSave(std::string name)
	{
		std::ofstream ofs;
		ofs.open((std::string("../Asset/Binary/HTN/Method/") + name + ".bin").c_str(), std::ios::binary);
		cereal::BinaryOutputArchive o_archive(ofs);
		o_archive(*this);
	}

	inline void ToLoad(std::string name, int value)
	{
		if (PathFileExistsA((std::string("../Asset/Binary/HTN/Method/") + name + ".bin").c_str()))
		{
			std::ofstream ifs;
			ifs.open((std::string("../Asset/Binary/HTN/Method/") + name + ".bin").c_str(), std::ios::binary);
			cereal::BinaryInputArchive i_archive(ifs);
			i_archive(*this);
		}
	}

	template<class T>
	void serialize(T& archive, const std::uint32_t version)
	{

		if (0 <= version)
		{
			archive
			(
				m_preconditions,
				m_subTasks
			);
		}
		else
		{
			archive
			(
				m_preconditions,
				m_subTasks
			);
		}
	}
public:
	std::vector<std::shared_ptr<PreCondition<State>>> m_preconditions;
	std::vector<std::shared_ptr<ITask<State>>> m_subTasks;
};