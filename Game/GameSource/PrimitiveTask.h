#pragma once
#include <vector>
#include <string>
#include <memory>
#include "ITask.h"
#include "ArcherWorldState.h"
#include "PreCondition.h"
#include "Effect.h"
#include ".\LibrarySource\Function.h"
#include ".\LibrarySource\vectorCombo.h"
#pragma region PrimitiveTaskについて
//PrimitiveTaskは
//世界に影響を与える行動を表すもの
//次の3つの要素でできている
//PreCondition	タスクを実行するために必要な条件
//Operator		オペレータは実際に行う行動
//Effect		オペレータの行動でワールドステートがどのように変わるか
#pragma endregion

template <class State>
class PrimitiveTask : public ITask<State>
{
public:
	PrimitiveTask() = default;
	~PrimitiveTask() = default;

	bool TryPlanTask(State& state, PlanList& planList) override
	{
		size_t count = m_preconditions.size();
		for (size_t i = 0; i < count; ++i)
		{
			if (m_preconditions[i]->CheckPreCondition(state))
				return false;
		}

		count = m_effects.size();
		for (size_t i = 0; i < count; ++i)
		{
			m_effects[i]->ApplyTo(state);
		}

		planList.AddOperator(m_operatorID);

		return true;
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
				m_preconditions,
				m_effects,
				m_operatorID
			);
		}
		else
		{
			archive
			(
				m_taskName,
				m_preconditions,
				m_effects,
				m_operatorID
			);
		}
	}
private:
	inline void ToSave(std::string name, int value)
	{
		std::ofstream ofs;
		ofs.open((std::string("../Asset/Binary/HTN/PrimitiveTask/")+ name + ".bin").c_str(), std::ios::binary);
		cereal::BinaryOutputArchive o_archive(ofs);
		o_archive(*this);
	}

	inline void ToLoad(std::string name)
	{
		if (PathFileExistsA((std::string("../Asset/Binary/HTN/PrimitiveTask/") + name).c_str()))
		{
			std::ifstream ifs;
			ifs.open((std::string("../Asset/Binary/HTN/PrimitiveTask/") + name).c_str(), std::ios::binary);
			cereal::BinaryInputArchive i_archive(ifs);
			i_archive(*this);
		}
	}
private:
	std::string m_taskName;
	std::vector<std::shared_ptr<PreCondition<State>>>	m_preconditions;
	std::vector<std::shared_ptr<Effect<State>>>			m_effects;
	uint32_t m_operatorID = 0;
	std::vector<std::string> m_fileNames;
};

template<class State>
inline void PrimitiveTask<State>::ImGui()
{
	ImGui::BeginChild("PrimitiveTask", ImVec2(500, 200), true);

	#pragma region Save/Load
		static int select = 0;
		if (ImGui::Button("Task Save"))
			ToSave(m_taskName, 0);
		ImGui::SameLine();
		if (ImGui::Button("Task Load"))
			ToLoad(m_fileNames[select]);

		if (ImGui::Button("File Name Get"))
		{
			bool isError = Source::Path::GetFileNames("../Asset/Binary/HTN/PrimitiveTask/", m_fileNames);
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
