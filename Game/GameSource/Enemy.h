#pragma once
#include "CharacterAI.h"
#include "EnemyBehaviorTree.h"
class Enemy : public CharacterAI
{
public:
	Enemy() = default;
	~Enemy() { m_behaviorTree.Release(); };

	void Init() override;

	void Update(float& elapsedTime) override;

	void Render(ID3D11DeviceContext* immediateContext) override;

	void ImGui(ID3D11Device* device) override;

	void Impact() override {};

	inline AIParameter::Emotion& GetEmotion() { return m_emotionParm; }
	inline CharacterParameter::Attack& GetAttack(const int num) { return m_attackParm.at(num); }
	inline CharacterParameter::BlendAnimation& GetBlendAnimation() { return m_blendAnimation; }


	inline float& GetElapsedTime() { return m_elapsedTime; }
	template<class T>
	void serialize(T& archive, const std::uint32_t version)
	{
		if (version >= 6)
		{
			archive
			(
				m_collision,
				m_behaviorTree,
				m_emotionParm,
				m_standardValuePram,
				m_attackParm,
				m_moveParm
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
				m_attackParm

			);
		}
	}

public:
	enum  Animation
	{
		TPOSE0,
		Idle,
		Damage,
		TPOSE1,
		Knockdown,
		Die,
		Wrath,
		LeftTurn,
		RightTurn,
		Run,
		BackFlip,
		StandUp,
		MuscleSignal,
		RelaxSignal,
		RoaringSignal,
		RunSignal,
		CrossPunch,
		TurnAttackLower,
		Hook,
		RightPunchLower,
		TurnAttackHeight,
		FallFlat_edit,
		RightPunchUpper,
		WrathNearAttack,
		WrathFarAttack,


	};

	enum class AttackType
	{
		CrossPunch,
		TurnAttackLower,
		Hook,
		RightPunchLower,
		TurnAttackHeight,
		FallFlat_edit,
		RightPunchUpper,
		WrathNearAttack,
		WrathFarAttack,
	};
	
private:
	void SerialVersionUpdate(uint32_t version)
	{
	}
private:

	EnemyBehaviorTree m_behaviorTree;
	std::shared_ptr<EnemyBehaviorTask> m_selectTask;

	CharacterParameter::BlendAnimation		m_blendAnimation;
	CharacterParameter::DebugObjects		m_debugObjects;
	std::vector<CharacterParameter::Attack>	m_attackParm;
	int m_moveState;
	float m_elapsedTime;
	bool m_isAction;
};