#pragma once
#include "CharacterAI.h"
#include "CharacterParameter.h"
#include "Domain.h"
#include "ArcherWorldState.h"
#include "DomainConverter.h"
#include "HTN/PlanRunner.h"

#include ".\LibrarySource\Vector.h"


class Archer : public CharacterAI
{
public:

	Archer() = default;
	~Archer()
	{
		m_domain.Release();
	};

	void Init() override;
	void Update(float& elapsedTime) override;
	void Render(ID3D11DeviceContext* immediateContext) override;
	void ImGui(ID3D11Device* device) override;

	void Impact() override;
	void RestAnimationIdle();

	bool FindAttackPoint();
	bool MoveRun();
	bool SetArrow();
	bool Shoot();
	bool SearchAttackDirection();
	bool Avoid();
	bool Heal();
	bool Revival();

	template<class T>
	void serialize(T& archive, const std::uint32_t version)
	{
		if (version >= 1)
		{
			archive
			(
				m_attackParm,
				m_statusParm,
				m_moveParm,
				m_cameraParm,
				m_stepParm,
				m_collision,
				m_domainConverter
			);
		}
		else
		{
			archive
			(
				m_blendAnimation,
				m_attackParm,
				m_statusParm,
				m_moveParm,
				m_cameraParm,
				m_stepParm,
				m_collision
			);
		}
	}
private:
	void Move(float& elapsedTime);

	void ChangeCharacter();
	bool KnockBack();
	bool JudgeBlendRatio(const bool isLoop = true);
	bool Rotate(VECTOR3F& target,const float turnSpeed, bool isLookEnemy = false);
	void SerialVersionUpdate(uint32_t version)
	{
		m_statusParm.serialVersion = version;
		m_moveParm.serialVersion = version;
		m_cameraParm.serialVersion = version;
		m_stepParm.serialVersion = version;

		for (auto& atk : m_attackParm)
		{
			atk.serialVersion = version;
		}

		for (auto& coll : m_collision)
		{
			coll.serialVersion = version;
		}
	}

private:
	enum Animation
	{
		IDLE,
		RUN,
		RUNL,
		RUNR,
		NOUSE,
		DIVE,
		SET,
		AIM,
		SHOOT,
		HIT,
		HITBIG,
		HEAL,
		REVIVAL,
		DEATH
	} m_animationType = IDLE;

	enum class Mode
	{
		Moving,
		Aiming
	};

	Mode m_mode = Mode::Moving;

	const float	kSafeAreaRadius = 30.0f;
	const float	kBlendValue = 0.1f;
	const float	kViewRange = 0.15f;
	const float	kAimRange = 0.05f;
	const float	kPredictionViewRangeE = 0.8f;

	int m_state;
	float m_padDeadLine;
	float m_elapsedTime;
	float m_MoveOffset;
	bool m_hasBlendAnim;
	bool m_hasRotated;
	std::vector<std::pair<bool,VECTOR3F>> m_controlPoint;

	VECTOR3F m_attackPoint;
	VECTOR3F m_safeAreaSclae;
	
	Source::Input::Input*					m_input;
	CharacterParameter::Step				m_stepParm;
	CharacterParameter::DebugObjects		m_debugObjects;
	CharacterParameter::BlendAnimation		m_blendAnimation;
	std::vector<CharacterParameter::Attack>	m_attackParm;

	ArcherWorldState						m_worldState;
	Domain<ArcherWorldState, Archer>		m_domain;
	DomainConverter							m_domainConverter;
	PlanRunner<ArcherWorldState, Archer>	m_planRunner;
	std::vector<TaskBase<ArcherWorldState, Archer>*> m_currentPlanList;
};
