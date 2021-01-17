#pragma once
#include <string>
#include "WorldState.h"

#pragma region Effectについて
//Effectはタスクが実行された際に
//WorldStateに与える影響のこと
//基底クラス
#pragma endregion
template<class State>
class Effect
{
public:
	Effect() = default;
	~Effect() = default;

	inline void ApplyTo(State& state)
	{
		if (m_status != WorldState::Status::MaxStatus)
			state.SetStatus(m_status, m_value);
		else
			state.SetStatus(m_statusForTemp, m_value);
	}

	inline std::string& GetEffectName() { return m_effectName; }
	inline void SetEffectName(const std::string name) { m_effectName = name; }

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
				m_effectName
			);
		}
		else
		{
			archive
			(
				m_status,
				m_statusForTemp,
				m_value,
				m_effectName
			);
		}
	}
private:
	inline void ToSave(std::string name)
	{
		std::ofstream ofs;
		ofs.open((std::string("../Asset/Binary/HTN/Effect/") + name + ".bin").c_str(), std::ios::binary);
		cereal::BinaryOutputArchive o_archive(ofs);
		o_archive(*this);
	}

	inline void ToLoad(std::string name)
	{
		if (PathFileExistsA((std::string("../Asset/Binary/HTN/Effect/") + name).c_str()))
		{
			std::ifstream ifs;
			ifs.open((std::string("../Asset/Binary/HTN/Effect/") + name).c_str(), std::ios::binary);
			cereal::BinaryInputArchive i_archive(ifs);
			i_archive(*this);
		}
	}
private:
	WorldState::Status m_status;
	int m_statusForTemp;
	bool m_value;

	std::string m_effectName;
	std::vector<std::string> m_fileNames;
};

template<class State>
inline void Effect<State>::ImGui()
{
	ImGui::BeginChild("Effect", ImVec2(500, 200), true);

	#pragma region Save/Load
		static int select = 0;
		if (ImGui::Button("Effect Save"))
			ToSave(m_effectName);
		ImGui::SameLine();
		if (ImGui::Button("Effect Load"))
			ToLoad(m_fileNames[select]);

		if (ImGui::Button("File Name Get"))
		{
			bool isError = Source::Path::GetFileNames("../Asset/Binary/HTN/Effect/", m_fileNames);
		}
	
		ImGui::Combo("SelectFileNames",
			&select,
			vectorGetter,
			static_cast<void*>(&m_fileNames),
			static_cast<int>(m_fileNames.size())
		);
	#pragma endregion

	#pragma region EffectName
		static char effectName[256] = "";
		std::string currentEffectName = m_effectName.c_str();
		if (currentEffectName.size() == 0)
			currentEffectName = "EmptyEffectName";
		ImGui::Text("EffectName : %s", currentEffectName.c_str());
		ImGui::InputText("EffectName", effectName, 256);
		std::string nameDecided = effectName;
		if (ImGui::Button("Set EffectName"))
			m_effectName = nameDecided;
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
		ImGui::Checkbox(u8"WorldStateは有効か", &m_value);
	#pragma endregion


	ImGui::EndChild();


}
