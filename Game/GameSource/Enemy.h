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

	inline AIParameter::Emotion& GetEmotion() { return m_emotionParm; }
	inline CharacterParameter::Attack& GetAttack() { return m_attackParm; }

	template<class T>
	void serialize(T& archive, const std::uint32_t version)
	{
		if (version >= 3)
		{
			archive
			(
				m_collision,
				m_behaviorTree
			);
		}
		else
		{
			archive
			(
				m_collision,
				m_behaviorTree

			);
		}
	}
private:
	void SerialVersionUpdate(uint32_t version)
	{
	}
private:
	EnemyBehaviorTree m_behaviorTree;
	CharacterParameter::Attack m_attackParm;
	CharacterParameter::BlendAnimation		m_blendAnimation;
	CharacterParameter::DebugObjects		m_debugObjects;
	float m_elapsedTime;

};