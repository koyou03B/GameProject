#pragma once
#include "WorldAnalyzer.h"
#include "ArchersGamemaker.h"
#include "Blackboard.h"


class AgentAI
{
public:
	AgentAI() = default;
	~AgentAI() = default;

	std::vector<TaskBase<ArcherWorldState, Archer>*> Init(Archer* mySelf);
	std::vector<TaskBase<ArcherWorldState, Archer>*> OperationFlowStart(Archer* mySelf);
	void SavePerception(CharacterAI* mySelf, CharacterAI* target);
	void Release();
	void ImGui(Archer* archer);


	template<class T>
	void serialize(T& archive, const std::uint32_t version)
	{
		if (version >= 0)
		{
			archive
			(
				m_gameMaker
			);
		}
		else
		{
			archive
			(
				m_gameMaker
			);
		}
	}
private:
	void CreatePerception(Archer* mySelf);
	void PerceptionOfRecover(ArcherWorldState& worldState, const HitPointLv& hpLv);
	void PerceptionOfDistance(ArcherWorldState& worldState, const DistanceLv& hpLv,bool isCurrent = false);
private:
	WorldAnalyzer		m_worldAnalyser;
	ArchersGameMaker	m_gameMaker;
	Blackboard			m_blackboard;

	int m_recoverPoint[4] = {0,1,3,5};
};