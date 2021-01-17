#pragma once
#include <vector>
#include <string>
#include <memory>
#include "ITask.h"
#include "ArcherWorldState.h"
#include "Precondition.h"
#include "Effect.h"
#include ".\LibrarySource\Function.h"
#include ".\LibrarySource\vectorCombo.h"
#pragma region PrimitiveTaskについて
//PrimitiveTaskは
//世界に影響を与える行動を表すもの
//次の3つの要素でできている
//Precondition	タスクを実行するために必要な条件
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
			if (m_preconditions[i]->CheckPrecondition(state))
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

	inline void SetPrecondition(std::shared_ptr<Precondition<State>>& precondition)
	{
		m_preconditions.push_back(precondition);
	}

	inline void SetEffect(std::shared_ptr<Effect<State>>& effect)
	{
		m_effects.push_back(effect);
	}
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
	std::vector<std::shared_ptr<Precondition<State>>>	m_preconditions;
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
	
	#pragma region Precondition
		int pcSize = static_cast<int>(m_preconditions.size());
		ImGui::DragInt("Precondition Size", &pcSize, 0, 10);

		if (!m_preconditions.empty())
		{
			static int selectPC = 0;
			ImGui::DragInt("Precondition Select", &selectPC, 0, 10);
			auto preCondition = m_preconditions.at(selectPC);
			std::string pcName = preCondition->GetPreconditionName();
			ImGui::Text("PreconditionName : %s", pcName.c_str());


		}

	#pragma endregion

	#pragma region Effect
		int effectSize = static_cast<int>(m_effects.size());
		ImGui::DragInt("Effect Size", &effectSize, 0, 10);

		if (!m_effects.empty())
		{
			static int selectEffect = 0;
			ImGui::DragInt("Effect Select", &selectEffect, 0, 10);
			auto effect = m_effects.at(selectEffect);
			std::string effectName = effect->GetEffectName();
			ImGui::Text("EffectName : %s", effectName.c_str());

			if (ImGui::Button("Delete"))
				m_effects.erase(m_effects.begin());
		}

	#pragma endregion

	ImGui::EndChild();
}
