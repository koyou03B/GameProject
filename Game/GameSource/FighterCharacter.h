#pragma once
#include "CharacterAI.h"
#include "CharacterParameter.h"
#include ".\LibrarySource\Vector.h"


class Fighter : public CharacterAI
{
public:
	Fighter() { Init(); };
	~Fighter() = default;

	void Init() override;

	void Update(float& elapsedTime) override;

	void Render(ID3D11DeviceContext* immediateContext) override;

	void Release() override;

	void ImGui(ID3D11Device* device) override;

	void Impact() override;

	VECTOR3F GetInputDirection();
	VECTOR3F GetRotationAfterAngle(VECTOR2F vector,float turnSpeed);

	template<class T>
	void serialize(T& archive, const std::uint32_t version)
	{
		if (version >= 12)
		{
			archive
			(
				m_blendAnimation,
				m_attackParm,
				m_statusParm,
				m_moveParm,
				m_cameraParm,
				m_stepParm,
				m_collision,
				m_damageParm
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
		RUN,
		DIVE,
		LEFT_KICK,
		LEFT_DUSH_KICK,
		RIGHT_KICK,
		LEFT_ROLL_KICK,
		RIGHT_ROLL_KICK,
		RIGHT_PUNCH,
		RIGHT_FLY_KICK,
		HIT_REACTION,
		HIT_BIG_REACTION,
		DEATH
	} m_animationType = IDLE;

	enum AttackType
	{
		LeftKick,
		LeftDushKick,
		RightKick,
		LeftRollKick,
		RightRollKick,
		RightPunch,
		RightFlyKick,
		NON
	} m_attackType = NON;

	void Move(float& elapsedTime);

	void Step(float& elapsedTime);

	bool KnockBack();

	void Attack(float& elapsedTime);

	void RestAnimationIdle();

	void ChangeCharacter();

	void Attacking(Animation currentAnimation, Animation nextAnimations,
		CharacterParameter::Attack& attack,CharacterParameter::Collision& collision);

	void Stepping(float& elapsedTime);

	void SerialVersionUpdate(uint32_t version)
	{
		m_blendAnimation.serialVersion = 13;
		m_statusParm.serialVersion = version;
		m_moveParm.serialVersion = version;
		m_cameraParm.serialVersion = version;
		m_stepParm.serialVersion = version;
		m_damageParm.serialVersion = version;
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
