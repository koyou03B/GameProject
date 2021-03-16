#pragma once
#include "CharacterAI.h"
#include "KnowledgeMaterial.h"
//下記変数の信用度を図るのがm_creditLv

class WorldAnalyzer
{
public:
	WorldAnalyzer() = default;
	~WorldAnalyzer() = default;
	WorldAnalyzer(const WorldAnalyzer& it);
	WorldAnalyzer& operator=(const WorldAnalyzer& it);

	void Analyze(CharacterAI* mySelf,CharacterAI* target);

	inline DistanceLv GetDinstanceLv()	{ return m_distanceLv; }
	inline HitPointLv GetHpLv()			{ return m_hpLv; }
	inline SkillLv	  GetSkillLv()		{ return m_skillLv; }
	inline float	  GetCreditLv()		{ return m_creditLv; }
	inline VECTOR3F   GetPosition()		{ return m_position; }
	
	inline void SetCreditLv(const float& credit) { this->m_creditLv = credit; }
private:
	const float	kSafeAreaRadius = 20.0f;

	DistanceLv	m_distanceLv;
	HitPointLv	m_hpLv;
	SkillLv		m_skillLv;
	float		m_creditLv;
	VECTOR3F	m_position;
};