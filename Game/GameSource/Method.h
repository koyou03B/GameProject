#pragma once
#include <vector>
#include <memory>
#include <unordered_map>
#include "ITask.h"
#include "ArcherWorldState.h"
#include "PreCondition.h"
#include "PrimitiveTask.h"

#include <cereal/types/map.hpp>

#pragma region Methodについて
//Methodは自分が選ばれるための
//コンディションと選ばれた時の
//Taskのリストを持つ
//CompoundTaskがMethodを持つ
#pragma endregion

enum class TaskType
{
	Primitive,
	Compound
};


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

	inline void ToLoad(std::string name)
	{
		if (PathFileExistsA((std::string("../Asset/Binary/HTN/Method/") + name).c_str()))
		{
			std::ifstream ifs;
			ifs.open((std::string("../Asset/Binary/HTN/Method/") + name).c_str(), std::ios::binary);
			cereal::BinaryInputArchive i_archive(ifs);
			i_archive(*this);
		}
	}

	void ImGui();

	template<class T>
	void serialize(T& archive, const std::uint32_t version)
	{

		if (0 <= version)
		{
			archive
			(
				m_preconditions,
				m_subTasksID,
				m_methodName
			);
		}
		else
		{
			archive
			(
				m_preconditions,
				m_subTasksID,
				m_methodName
			);
		}
	}
public:
	std::vector<std::shared_ptr<PreCondition<State>>> m_preconditions;
	std::multimap <TaskType, int>	m_subTasksID;
	std::vector<std::shared_ptr<ITask<State>>> m_subTasks;

	std::string m_methodName;
	std::vector<std::string> m_fileNames;
};

template<class State>
inline void Method<State>::ImGui()
{
	ImGui::BeginChild("Method", ImVec2(500, 200), true);

	static int select = 0;

	if (ImGui::Button("Method Save"))
		ToSave(m_methodName);
	ImGui::SameLine();
	if (ImGui::Button("Method Load"))
		ToLoad(m_fileNames[select]);

	if (ImGui::Button("File Name Get"))
	{
		bool isError = Source::Path::GetFileNames("../Asset/Binary/HTN/Method/", m_fileNames);
	}

	ImGui::Combo("SelectFileNames",
		&select,
		vectorGetter,
		static_cast<void*>(&m_fileNames),
		static_cast<int>(m_fileNames.size())
	);

	static char methodName[256] = "";
	std::string currentMethodName = m_methodName.c_str();
	if (currentMethodName.size() == 0)currentMethodName = "EmptyMehodName";
	ImGui::Text("MethodName : %s", currentMethodName.c_str());
	ImGui::InputText("MethodName", methodName, 256);
	std::string nameDecided = methodName;
	if (ImGui::Button("Set MethodName"))
		m_methodName = nameDecided;


	static int value = 0;
	ImGui::RadioButton("Primitive", &value, 0); ImGui::SameLine();
	ImGui::RadioButton("Compound", &value, 1);
	TaskType taskType = static_cast<TaskType>(value);

	static int taskID = 0;
	ImGui::DragInt("TaskID", &taskID, 0, 20);

	if (ImGui::Button("SubTaskID Add"))
		m_subTasksID.insert(std::make_pair(taskType, taskID));
	int subTaskCount = static_cast<int>(m_subTasksID.size());
	ImGui::DragInt("SubTaskCount", &subTaskCount, 0, 20);

	ImGui::EndChild();

}
