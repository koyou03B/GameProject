#pragma once
#include "CharacterAI.h"
#include "CharacterParameter.h"
#include ".\LibrarySource\Vector.h"

struct AimMode
{
	CharacterParameter::Camera	aimCameraParm;
	CharacterParameter::Move	aimMoveParm;
	std::shared_ptr<Source::Sprite::SpriteBatch> scopeTexture;
	VECTOR2F texturePosition;
	VECTOR2F textureScale;
	VECTOR2F textureSize;
	VECTOR4F textureColor;
	int serialVersion = 0;
	template<class T>
	void serialize(T& archive, const std::uint32_t version)
	{
		if (serialVersion <= version)
		{
			archive
			(
				aimCameraParm,
				aimMoveParm,
				texturePosition,
				textureScale,
				textureSize,
				textureColor
			);
		}
		else
		{
			archive
			(
				aimCameraParm,
				aimMoveParm,
				texturePosition,
				textureScale,
				textureSize,
				textureColor
			);
		}
	}
};

class Archer : public CharacterAI
{
public:

	Archer() = default;
	~Archer() = default;

	void Init() override;

	void Update(float& elapsedTime) override;

	void Render(ID3D11DeviceContext* immediateContext) override;

	void ImGui(ID3D11Device* device) override;

	template<class T>
	void serialize(T& archive, const std::uint32_t version)
	{
		if (version >= 1)
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

	void RestAnimationIdle();

	void Aim();
	void Aiming();

	void Move(float& elapsedTime);
	void AimMove(float& elapsedTime) {};

	void Step(float& elapsedTime);
	void Stepping(float& elapsedTime);

	void Shot();

	void ChangeCharacter();

	void SerialVersionUpdate(uint32_t version)
	{
		m_blendAnimation.serialVersion = version;
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
		WALK,
		RUN,
		DIVE,
		AIMRECOIL,
		AIM,
		AIMWALKUNDER,
		ARROWSHOT,
		IMPACT,
		DEATH,
		SPELL,
		AIMWALK
	} m_animationType = IDLE;

	enum class Mode
	{
		Moving,
		Aiming
	};

	Mode m_mode = Mode::Moving;

	float m_padDeadLine;
	float m_elapsedTime;

	AimMode									m_aimMode;
	Source::Input::Input*					m_input;

	CharacterParameter::Step				m_stepParm;
	CharacterParameter::DebugObjects		m_debugObjects;
	CharacterParameter::BlendAnimation		m_blendAnimation;
	std::vector<CharacterParameter::Effect> m_effect;
	std::vector<CharacterParameter::Attack>	m_attackParm;

};
