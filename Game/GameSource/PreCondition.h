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

#pragma region Precondition�ɂ���
//�^�X�N�����s���邽�߂̏���
//���N���X
#pragma endregion
template<class State>
class Precondition
{
public:
	Precondition() = default;
	~Precondition() = default;

	inline bool CheckPrecondition(State& state)
	{
		if (m_status != WorldState::Status::MaxStatus)
			return state.GetStatus(m_status) == m_value;

		return  state.GetStatus(m_statusForTemp) == m_value;
	}

	void ImGui();

	inline std::string& GetPreconditionName() { return m_preconditionName; }
	inline void SetPreconditionName(const std::string name) { m_preconditionName = name; }


	template<class T>
	void serialize(T& archive, const std::uint32_t version)
	{

		if (1 <= version)
		{
			archive
			(
				m_status,
				m_statusForTemp,
				m_value,
				m_preconditionName
			);
		}
		else
		{
			archive
			(
				m_status,
				m_statusForTemp,
				m_value,
				m_preconditionName
			);
		}
	}
private:
	inline void ToSave(std::string name)
	{
		std::ofstream ofs;
		ofs.open((std::string("../Asset/Binary/HTN/Precondition/") + name + ".bin").c_str(), std::ios::binary);
		cereal::BinaryOutputArchive o_archive(ofs);
		o_archive(*this);
	}

	inline void ToLoad(std::string name)
	{
		if (PathFileExistsA((std::string("../Asset/Binary/HTN/Precondition/") + name).c_str()))
		{
			std::ifstream ifs;
			ifs.open((std::string("../Asset/Binary/HTN/Precondition/") + name).c_str(), std::ios::binary);
			cereal::BinaryInputArchive i_archive(ifs);
			i_archive(*this);
		}
	}
private:
	WorldState::Status m_status;
	int m_statusForTemp;
	bool m_value;

	std::string m_preconditionName;
	std::vector<std::string> m_fileNames;
};

template<class State>
inline void Precondition<State>::ImGui()
{
	ImGui::BeginChild("Precondition", ImVec2(500, 200), true);

	#pragma region Save/Load
		static int select = 0;
		if (ImGui::Button("Precondition Save"))
			ToSave(m_preconditionName);
		ImGui::SameLine();
		if (ImGui::Button("Precondition Load"))
			ToLoad(m_fileNames[select]);
	
		if (ImGui::Button("File Name Get"))
		{
			bool isError = Source::Path::GetFileNames("../Asset/Binary/HTN/Precondition/", m_fileNames);
		}
	
		ImGui::Combo("SelectFileNames",
			&select,
			vectorGetter,
			static_cast<void*>(&m_fileNames),
			static_cast<int>(m_fileNames.size())
		);
	#pragma endregion

	#pragma region PreconditionName
		static char preconditionName[256] = "";
		std::string currentPreconditionName = m_preconditionName.c_str();
		if (currentPreconditionName.size() == 0)
			currentPreconditionName = "EmptyPreconditionName";
		ImGui::Text("PreconditionName : %s", currentPreconditionName.c_str());
		ImGui::InputText("PreconditionName", preconditionName, 256);
		std::string nameDecided = preconditionName;
		if (ImGui::Button("Set PreconditionName"))
			m_preconditionName = nameDecided;
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
			"AlwaysTrue",
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
				};

				int selectStatus = m_statusForTemp;
				ImGui::Combo("ArcherStatusName",
					&selectStatus,
					vectorGetter,
					static_cast<void*>(&statusName),
					static_cast<int>(statusName.size())
				);
				m_status = WorldState::MaxStatus;
				m_statusForTemp = static_cast<ArcherWorldState::Status>(selectStatus);

			}

		}
	#pragma endregion

	#pragma region True/False
		ImGui::Checkbox(u8"WorldState�͗L����",&m_value);
	#pragma endregion


	ImGui::EndChild();

}
