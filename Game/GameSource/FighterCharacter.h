#pragma once
#include "CharacterAI.h"
#include "CharacterParameter.h"
#include ".\LibrarySource\Vector.h"


class Fighter : public CharacterAI
{
public:

	Fighter() = default;
	~Fighter() = default;

	void Init() override;

	void Update(float& elapsedTime) override;

	void Render(ID3D11DeviceContext* immediateContext) override;

	void ImGui(ID3D11Device* device) override;

	void Impact() override;

	template<class T>
	void serialize(T& archive, const std::uint32_t version)
	{
		if (version >= 11)
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
	enum Animation
	{
		IDLE=1,
		WALK,
		RUN,
		DIVE,
		RIGHTPUNCHING,
		LEFTPUNCHING,
		LASTTPUNCHING,
		LEFTFIRSTKICK,
		RIGHTKICK,
		LEFTKICKING,
		IMPACT,
		DEATH,
		IDLE2,
		ARMSET,
		FIGHTIDLE,
		HEAL
	} m_animationType = IDLE;

	void Move(float& elapsedTime);

	void Step(float& elapsedTime);

	bool KnockBack();

	void Attack(float& elapsedTime);

	void RestAnimationIdle();

	void ChangeCharacter();

	void Attacking(Animation currentAnimation, Animation nextAnimations[2],
		CharacterParameter::Attack& attack,CharacterParameter::Collision& collision);

	void Stepping(float& elapsedTime);

	void SerialVersionUpdate(uint32_t version)
	{
		m_blendAnimation.serialVersion = version;
		m_statusParm.serialVersion = version;
		m_moveParm.serialVersion = version;
		m_cameraParm.serialVersion = version;
		m_stepParm.serialVersion = version;

		for (auto& atk : m_attackParm)
		{
			atk.serialVersion= version;
		}

		for (auto& coll : m_collision)
		{
			coll.serialVersion = version;
		}
	}
private:
	CharacterParameter::BlendAnimation		m_blendAnimation;
	CharacterParameter::DebugObjects		m_debugObjects;
	CharacterParameter::Step				m_stepParm;
	std::vector<CharacterParameter::Effect> m_effect;
	std::vector<CharacterParameter::Attack>	m_attackParm;
	Source::Input::Input* m_input;
	float m_padDeadLine;
	float m_elapsedTime;
	bool m_adjustAnimation;
};
