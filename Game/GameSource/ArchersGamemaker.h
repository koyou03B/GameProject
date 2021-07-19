#pragma once
#include "Domain.h"
#include "GameMaker.h"
#include "HTN/PlanRunner.h"
#include "DomainConverter.h"
#include "ArcherWorldState.h"

#undef max
#undef min
#include<cereal/cereal.hpp>
#include<cereal/archives/binary.hpp>

class Archer;
class ArchersGameMaker : public GameMaker
{
public:
	ArchersGameMaker() = default;
	~ArchersGameMaker() = default;

	void Init();
	void Release();

	void SetRootTask(const CompoundTaskType& type);
	const CompoundTaskType& GetRootTaskType() { return m_taskType; }
	std::vector<TaskBase<ArcherWorldState, Archer>*> SearchBehaviorTask(ArcherWorldState& worldState);
	void ImGui(Archer* arccher);

	template<class T>
	void serialize(T& archive, const std::uint32_t version)
	{
		if (version >= 0)
		{
			archive
			(
				m_domainConverter
			);
		}
		else
		{
			archive
			(
				m_domainConverter
			);
		}
	}

private:
	DomainConverter							m_domainConverter;
	Domain<ArcherWorldState, Archer>		m_domain;
	PlanRunner<ArcherWorldState, Archer>	m_planRunner;
	CompoundTaskType									m_taskType;
};

