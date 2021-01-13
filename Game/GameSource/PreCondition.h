#pragma once
#include <string>
#include "ArcherWorldState.h"
#include ".\LibrarySource\vectorCombo.h"

#ifdef _DEBUG
#include "..\External_libraries\imgui\imgui.h"
#include "..\External_libraries\imgui\imgui_impl_dx11.h"
#include "..\External_libraries\imgui\imgui_impl_win32.h"
#include "..\External_libraries\imgui\imgui_internal.h"
#endif

#undef max
#undef min
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/utility.hpp>

#pragma region Preconditionについて
//タスクを実行するための条件
//基底クラス
#pragma endregion
template<class State>
class PreCondition
{
public:
	PreCondition() = default;
	~PreCondition() = default;

	inline bool CheckPreCondition(State& state)
	{
		if (m_status != WorldState::Status::MaxStatus)
			return state.GetStatus(m_status) == m_value;

		return  state.GetStatus(m_statusForTemp) == m_value;
	}

	inline void ToSave(std::string name)
	{
		std::ofstream ofs;
		ofs.open((std::string("../Asset/Binary/HTN/PreCondition/") + name + ".bin").c_str(), std::ios::binary);
		cereal::BinaryOutputArchive o_archive(ofs);
		o_archive(*this);
	}

	inline void ToLoad(std::string name, int value)
	{
		if (PathFileExistsA((std::string("../Asset/Binary/HTN/PreCondition/") + name).c_str()))
		{
			std::ifstream ifs;
			ifs.open((std::string("../Asset/Binary/HTN/PreCondition/") + name).c_str(), std::ios::binary);
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
				m_status,
				m_statusForTemp,
				m_value,
				m_methodName,
				m_fileNames
			);
		}
		else
		{
			archive
			(
				m_status,
				m_statusForTemp,
				m_value,
				m_methodName,
				m_fileNames
			);
		}
	}

private:
	WorldState::Status m_status;
	int m_statusForTemp;
	bool m_value;

	std::string m_methodName;
	std::vector<std::string> m_fileNames;
};

template<class State>
inline void PreCondition<State>::ImGui()
{
	ImGui::BeginChild("PreCondition", ImVec2(500, 200), true);

	#pragma region Save/Load
		static int select = 0;
		if (ImGui::Button("PreCondition Save"))
			ToSave(m_taskName, 0);
		ImGui::SameLine();
		if (ImGui::Button("PreCondition Load"))
			ToLoad(m_fileNames[select]);
	
		if (ImGui::Button("File Name Get"))
		{
			bool isError = Source::Path::GetFileNames("../Asset/Binary/HTN/PreCondition/", m_fileNames);
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

	#pragma region Status/StatusToTemplate
		static int value = 0;
		ImGui::RadioButton("Status", &value, 0); ImGui::SameLine();
		ImGui::RadioButton("StatusToTemplate", &value, 1);

		if (value == 0)
		{
			std::vector<std::string> statusName =
			{
			"CanAttack",
			"CanMove",
			"CanAvoid",
			"CanRecover",
			"CanRevive",
			"MaxStatus"
			};

			int selectStatus = m_status;
			ImGui::Combo("StatusName",
				&selectStatus,
				vectorGetter,
				static_cast<void*>(&statusName),
				static_cast<int>(statusName.size())
			);
			m_status = static_cast<WorldState::Status>(selectStatus);
		}
		else
		{
			static int statusType = 0;
			ImGui::RadioButton("Archer", &statusType, 0);
			if (statusType == 0)
			{
				std::vector<std::string> statusName =
				{
				"HoldingArrow"
				//"CanMove",
				//"CanAvoid",
				//"CanRecover",
				//"CanRevive",
				//"MaxStatus"
				};

				int selectStatus = m_status;
				ImGui::Combo("ArcherStatusName",
					&selectStatus,
					vectorGetter,
					static_cast<void*>(&statusName),
					static_cast<int>(statusName.size())
				);
				m_status = static_cast<ArcherWorldState::Status>(selectStatus);

			}

		}
	#pragma endregion

	ImGui::EndChild();

}
