#pragma once
#include "Stone.h"
#include "CharacterAI.h"
#include "EnemyBehaviorTree.h"

class Enemy : public CharacterAI
{
public:
	Enemy() = default;
	~Enemy()
	{ 
		m_behaviorTree.Release();
		if (m_selectTask)
		{
			m_selectTask.reset();
		}
		m_stone->Release();
	}

	void Init() override;

	void Update(float& elapsedTime) override;

	void Render(ID3D11DeviceContext* immediateContext) override;

	void Release() override;

	bool JudgeBlendRatio();

	void ImGui(ID3D11Device* device) override;

	void Impact() override {};

	inline AIParameter::Emotion& GetEmotion() { return m_emotionParm; }
	inline CharacterParameter::Attack& GetAttack(const int num) { return m_attackParm.at(num); }
	inline CharacterParameter::BlendAnimation& GetBlendAnimation() { return m_blendAnimation; }
	inline std::unique_ptr<Stone>& GetStone() { return m_stone; }
	inline uint32_t& GetSignalAnimFrame(const int animNo) { return m_signalFrame[animNo]; }
	template<class T>
	void serialize(T& archive, const std::uint32_t version)
	{
		if (version >= 10)
		{
			archive
			(
				m_collision,
				m_behaviorTree,
				m_emotionParm,
				m_standardValuePram,
				m_attackParm,
				m_moveParm,
				m_signalFrame[0], m_signalFrame[1],
				m_signalFrame[2], m_signalFrame[3],
				m_stone
			);
		}
		else
		{
			archive
			(
				m_collision,
				m_behaviorTree,
				m_emotionParm,
				m_standardValuePram,
				m_attackParm,
				m_moveParm,
				m_signalFrame[0], m_signalFrame[1],
				m_signalFrame[2], m_signalFrame[3]
			);
		}
	}
	FLOAT4X4 boneSpine;
public:
	enum  Animation
	{
		TPOSE0,
		IDLE,
		DAMAGE,
		TPOSE1,
		KNOCKDOWN,
		DIE,
		WRATH,
		LEFT_TURN,
		RIGHT_TURN,
		RUN,
		STEP,
		BACK_FLIP,
		STAND_UP,
		MUSCLE_SIGNAL,
		RELAX_SIGNAL,
		ROARING_SIGNAL,
		RUN_SIGNAL,
		CROSS_PUNCH,
		TURN_ATTACK_LOWER,
		TPOSE2,
		HOOK,
		RIGHT_PUNCH_LOWER,
		TURN_ATTACK_HEIGHT,
		FALL_FLAT_EDIT,
		RIGHT_PUNCH_UPPER,
		RUN_ATTACK,
		WRATH_FAR_ATTACK,
	};

	enum  AttackType
	{
		CrossPunch,
		TurnAttackLower,
		Hook,
		RightPunchLower,
		TurnAttackHeight,
		FallFlat_edit,
		RightPunchUpper,
		Run_Attack,
		WrathFarAttack,
	};
	
private:
	void SerialVersionUpdate(uint32_t version)
	{
	}
private:
	EnemyBehaviorTree m_behaviorTree;
	std::shared_ptr<EnemyBehaviorTask> m_selectTask;
	std::unique_ptr<Stone> m_stone;

	CharacterParameter::BlendAnimation		m_blendAnimation;
	CharacterParameter::DebugObjects		m_debugObjects;
	std::vector<CharacterParameter::Attack>	m_attackParm;
	uint32_t m_signalFrame[4] = {};
	int m_moveState;
};