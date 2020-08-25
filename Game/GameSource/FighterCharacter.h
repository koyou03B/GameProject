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

	void Move(float& elapsedTime);

	void Step(float& elapsedTime);

	void Attack(float& elapsedTime);

	void ImGui(ID3D11Device* device) override;

	template<class T>
	void serialize(T& archive, const std::uint32_t version)
	{
		if (version >= 8)
		{
			archive
			(
				m_blendAnimation,
				m_attackParm,
				m_statusParm,
				m_moveParm,
				m_cameraParm,
				m_stepParm
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
				m_stepParm

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
		RIGHTKICK,
		LEFTKICKING,
		IMPACT,
		DEATH,
		IDLE2,
		ARMSET,
		FIGHTIDLE,
		HEAL
	} m_animationType = IDLE;

	void Attacking(Animation currentAnimation, Animation nextAnimations[2],
		CharacterParameter::Attack& attack,CharacterParameter::Collision collision);

	void Stepping(float& elapsedTime);

private:
	CharacterParameter::BlendAnimation		m_blendAnimation;
	CharacterParameter::DebugObjects		m_debugObjects;
	CharacterParameter::Step				m_stepParm;
	std::vector<CharacterParameter::Attack>	m_attackParm;
	Source::Input::Input* m_input;
	float radian;
	float m_padDeadLine;
	float m_elapsedTime;
};
