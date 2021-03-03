#include "WorldAnalyzer.h"

WorldAnalyzer::WorldAnalyzer(const WorldAnalyzer& it)
{
	m_creditLv = it.m_creditLv;
	m_distanceLv = it.m_distanceLv;
	m_hpLv = it.m_hpLv;
	m_skillLv = it.m_skillLv;
	m_position = it.m_position;
}

WorldAnalyzer& WorldAnalyzer::operator=(const WorldAnalyzer& it)
{
	this->m_creditLv = it.m_creditLv;
	this->m_distanceLv = it.m_distanceLv;
	this->m_hpLv = it.m_hpLv;
	this->m_skillLv = it.m_skillLv;
	this->m_position = it.m_position;

	return *this;
}

void WorldAnalyzer::Analyze(CharacterAI* mySelf, CharacterAI* target)
{
	if (mySelf == nullptr && target == nullptr) return;

	VECTOR3F tPosition = target->GetWorldTransform().position;
	float tHp = target->GetStatus().life;
	float tMaxHp = target->GetStatus().maxLife;

	VECTOR3F mPosition = mySelf->GetWorldTransform().position;
	VECTOR3F mAngle = mySelf->GetWorldTransform().angle;

	float distance = ToDistVec3(mPosition - tPosition);
	m_distanceLv = distance > kSafeAreaRadius ? DistanceLv::FAR_WAY : DistanceLv::NEAR_AWAY;
	m_position = tPosition;

	float hpRatio = tHp / tMaxHp;
	int maxHpLv = static_cast<int>(HitPointLv::END);
	const float divisionHp = 1.0f / maxHpLv;
	float minHp = 0.0f;
	for (int i = 0; i < maxHpLv; ++i)
	{
		if (minHp < hpRatio && hpRatio <= minHp + divisionHp)
			m_hpLv = static_cast<HitPointLv>(i);
		
		minHp += divisionHp;
	}

	VECTOR3F normalizeDist = NormalizeVec3(mPosition - tPosition);
	VECTOR3F mFront = VECTOR3F(sinf(mAngle.y), 0.0f, cosf(mAngle.y));
	mFront = NormalizeVec3(mFront);
	float dot = DotVec3(mFront, normalizeDist);
	float cosTheta = acosf(dot);
	float frontValue = mySelf->GetStandardValue().viewFrontValue;

	if (cosTheta <= frontValue)
		m_creditLv = 1.0f; //Œ©‚¦‚Ä‚¢‚éî•ñ‚È‚Ì‚ÅM—Š«MAX
	else
		m_creditLv = 0.8f; //Œ©‚¦‚Ä‚È‚­•·‚©‚³‚ê‚½î•ñ‚È‚Ì‚ÅM—Š«0.8

}
