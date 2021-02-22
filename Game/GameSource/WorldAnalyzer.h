#pragma once
#include "KnowledgeMaterial.h"

//���L�ϐ��̐M�p�x��}��̂�m_creditLv

class WorldAnalyzer
{
public:
	WorldAnalyzer() = default;
	~WorldAnalyzer() = default;

	void Analyze();

	inline DistanceLv GetDinstanceLv()	{ return m_distanceLv; }
	inline hitPointLv GetHpLv()			{ return m_hpLv; }
	inline SkillLv	  GetSkillLv()		{ return m_skillLv; }
	inline int		  GetCreditLv()		{ return m_creditLv; }
	
private:
	DistanceLv	m_distanceLv;
	hitPointLv	m_hpLv;
	SkillLv		m_skillLv;
	int			m_creditLv;
};