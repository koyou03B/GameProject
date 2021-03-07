#pragma once
#include "Arrow.h"
#include "AgentAI.h"
#include "CharacterAI.h"
#include "HTN/TaskBase.h"
#include "ArcherWorldState.h"
#include "CharacterParameter.h"
#include ".\LibrarySource\Vector.h"

struct AttackArrow
{
	float m_useArrowTimer;
	float m_useArrowMaxTimer;
	float m_attackPoint[2];
	ArrowType m_arrowType;

	void UpdateTime(float& elapsedTime);

	template<class T>
	void serialize(T& archive, const std::uint32_t version)
	{
		if (version >= 0)
		{
			archive
			(
				m_useArrowMaxTimer,
				m_attackPoint[0], m_attackPoint[1]
			);
		}
		else
		{
			archive
			(
				m_useArrowMaxTimer,
				m_attackPoint[0], m_attackPoint[1]
			);
		}
	}
};

class Archer : public CharacterAI
{
public:

	Archer() { Init(); };
	~Archer() = default;

	void Init() override;
	void Update(float& elapsedTime) override;
	void Render(ID3D11DeviceContext* immediateContext) override;
	void Release() override;
	void WriteBlackboard(CharacterAI* target) override;
	void ImGui(ID3D11Device* device) override;

	void Impact() override;
	void RestAnimationIdle();

	bool FindAttackPoint();
	bool MoveRun();
	bool SetArrow();
	bool Shoot();
	bool FindDirectionToAvoid();
	bool Avoid();
	bool Heal();
	bool Revival();
	
	inline std::unique_ptr<Arrow>& GetArrow() { return m_arrow; }
	inline ArcherWorldState& GetWorldState() { return m_worldState; }
	inline CharacterParameter::Recover& GetRecoverParm() { return m_recoverParm; }
	inline std::vector<TaskBase<ArcherWorldState, Archer>*>& GetPlanList() { return m_currentPlanList; }
	inline float& GetPlayerCreditLv() { return m_playerCreditLv; }
	inline bool& GetCanRecover() { return m_canRecover; }
	template<class T>
	void serialize(T& archive, const std::uint32_t version)
	{
		if (version >= 8)
		{
			archive
			(
				m_attackParm,
				m_statusParm,
				m_moveParm,
				m_cameraParm,
				m_stepParm,
				m_collision,
				m_agentAI,
				m_recoverParm,
				m_attackArrow,
				m_blendAnimation,
				m_damageParm
			);
		}
		else
		{
			archive
			(
				m_attackParm,
				m_statusParm,
				m_moveParm,
				m_cameraParm,
				m_stepParm,
				m_collision,
				m_agentAI,
				m_recoverParm,
				m_attackArrow
			);
		}
	}
private:

	bool KnockBack();
	bool JudgeBlendRatio(const bool isLoop = true);
	bool Rotate(VECTOR3F& target,const float turnSpeed, bool isLookEnemy = false);
	void ActiveWriteTimer();
	void ActiveRecoverTimer();
	void SerialVersionUpdate(uint32_t version)
	{
		m_statusParm.serialVersion = version;
		m_moveParm.serialVersion = version;
		m_cameraParm.serialVersion = version;
		m_stepParm.serialVersion = version;
		m_recoverParm.serialVersion = version;

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
	int m_currentTask;
	float m_elapsedTime;
	float m_writeTimer;
	float m_recoverTimer;
	float m_recoverMaxTimer;
	float m_writeMaxTimer;
	float m_playerCreditLv;
	bool m_hasBlendAnim;
	bool m_hasRotated;
	bool m_canRun;
	bool m_isSetArrow;
	bool m_hasShoot;
	bool m_canRecover;
	std::vector<std::pair<bool,VECTOR3F>> m_controlPoint;

	VECTOR3F m_attackPoint;
	VECTOR3F m_avoidPoint;
	VECTOR3F m_safeAreaSclae;
	
	Source::Input::Input*					m_input;
	CharacterParameter::Step				m_stepParm;
	CharacterParameter::Recover				m_recoverParm;
	CharacterParameter::DebugObjects		m_debugObjects;
	CharacterParameter::BlendAnimation		m_blendAnimation;
	std::vector<CharacterParameter::Attack>	m_attackParm;
	std::unique_ptr<Arrow>					m_arrow;
	AttackArrow                             m_attackArrow;

	AgentAI									m_agentAI;
	ArcherWorldState						m_worldState;
	std::vector<TaskBase<ArcherWorldState, Archer>*> m_currentPlanList;
};
