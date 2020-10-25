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
		TPOSE,
		IDLE,
		RUN,
		LEFT_TURN,
		RIGHT_TURN,
		WRATH,
		DIE,
		IMPACT,
		LEFT_PUNCH,
		RIGHT_PUNCH,
		RIGHT_LOWER,
		RIGHT_UPPER,
		TURN_ATTACK_KARI,
		TURN_ATTACK,
		JUMP_ATTACK,
		WRATH_ATTACK,
		RUN_SIGNAL,
		LEFT_WALK, 
		RIGHT_WALK,

	};

	enum class AttackType
	{
		RIGHT_PUNCH,
		LEFT_PUNCH,
		RIGHT_UPPER,
		RIGHT_LOWER,
		TURN_ATTACK_KARI,
		TURN_ATTACK,
		JUMP_ATTACK,
		WRATH_ATTACK,
		RUN_ATTACK,
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