#pragma once
#include <vector>
#include "Method.h"
#include "PrimitiveTask.h"
#include "ArcherWorldState.h"

#pragma region CompoundTaskについて
//タスクが達成すべき結果に
//至る方法を記述したMethodを複数持つ
#pragma endregion
template<class State>
class CompoundTask : public ITask<State>
{
public:
	CompoundTask() = default;
	~CompoundTask() = default;

	bool TryPlanTask(State& state, PlanList& planList) override
	{
		size_t count = m_methods.size();
		for (size_t i = 0; i < count; ++i)
		{
			if (!m_methods[i]->CheckPreCondition(state))
				continue;

			PlanList currentPlanList = planList;
			State currentState = state;
			if (!m_methods[i]->TryPlanSubTasks(state, planList))
			{
				planList = currentPlanList;
				state = currentState;
				continue;
			}

			return true;
		}

		return false;
	}

	inline std::string& GetTaskName() { return m_taskName; }
	inline void SetTaskName(const std::string name) { m_taskName = name; }

	void ImGui();

	template<class T>
	void serialize(T& archive, const std::uint32_t version)
	{

		if (0 <= version)
		{
			archive
			(
				m_taskName,
				m_methods
			);
		}
		else
		{
			archive
			(
				m_taskName,
				m_methods
			);
		}
	}

private:
	inline void ToSave(std::string name, int value)
	{
		std::ofstream ofs;
		ofs.open((std::string("../Asset/Binary/HTN/CompoundTask/") + name + ".bin").c_str(), std::ios::binary);
		cereal::BinaryOutputArchive o_archive(ofs);
		o_archive(*this);
	}

	inline void ToLoad(std::string name)
	{
		if (PathFileExistsA((std::string("../Asset/Binary/HTN/CompoundTask/") + name).c_str()))
		{
			std::ifstream ifs;
			ifs.open((std::string("../Asset/Binary/HTN/CompoundTask/") + name).c_str(), std::ios::binary);
			cereal::BinaryInputArchive i_archive(ifs);
			i_archive(*this);
		}
	}
private:
	std::string m_taskName;
	std::vector<std::shared_ptr<Method<State>>> m_methods;
	std::vector<std::string> m_fileNames;
};

template<class State>
inline void CompoundTask<State>::ImGui()
{
	ImGui::BeginChild("CompoundTask", ImVec2(500, 200), true);

	#pragma region  Save/Load
		static int select = 0;
		if (ImGui::Button("Task Save"))
			ToSave(m_taskName, 0);
		ImGui::SameLine();
		if (ImGui::Button("Task Load"))
			ToLoad(m_fileNames[select]);
	
		if (ImGui::Button("File Name Get"))
		{
			bool isError = Source::Path::GetFileNames("../Asset/Binary/HTN/CompoundTask/", m_fileNames);
		}
	
		ImGui::Combo("SelectFileNames",
			&select,
			vectorGetter,
			static_cast<void*>(&m_fileNames),
			static_cast<int>(m_fileNames.size())
		);
	#pragma endregion

	#pragma region TaskName
		static char taskName[256] = "";
		std::string currentTaskName = m_taskName.c_str();
		if (currentTaskName.size() == 0)currentTaskName = "EmptyTaskName";
		ImGui::Text("TaskName : %s", currentTaskName.c_str());
		ImGui::InputText("TaskName", taskName, 256);
		std::string nameDecided = taskName;
		if (ImGui::Button("Set TaskName"))
			m_taskName = nameDecided;
	#pragma endregion

	ImGui::EndChild();
}
