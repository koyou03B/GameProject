#pragma once
#include "CharacterAI.h"
#include "CharacterParameter.h"
#include ".\LibrarySource\Vector.h"
#include "ArcherWorldState.h"
#include "PrimitiveTask.h"
#include "CompoundTask.h"
#include "HTNPlanner.h"
struct AimMode
{
	CharacterParameter::Camera	aimCameraParm;
	CharacterParameter::Move	aimMoveParm;

	VECTOR3F arrowAngle = {};
	uint32_t meshNomber = 0;
	uint32_t boneNomber = 0;
	uint32_t frameCount = 0;

	float aimIdleBlendRatio = 0.0f;
	float aimMoveBlendRatio = 0.0f;
	float aimShotBlendRatio = 0.0f;
	float aimStepBlendRatio = 0.0f;

	float collsionVector = 0.0f;
	bool isAim = false;
	bool isStep = false;
	bool isShot = false;
	uint32_t serialVersion = 0;
	template<class T>
	void serialize(T& archive, const std::uint32_t version)
	{
		if (serialVersion <= version)
		{
			archive
			(
				aimCameraParm,
				aimMoveParm,
				arrowAngle,
				meshNomber,
				boneNomber,
				aimIdleBlendRatio,
				aimMoveBlendRatio,
				aimShotBlendRatio,
				aimStepBlendRatio,
				collsionVector,
				frameCount
			);
		}
		else
		{
			archive
			(
				aimCameraParm,
				aimMoveParm,
				arrowAngle,
				meshNomber,
				boneNomber,
				aimIdleBlendRatio,
				aimMoveBlendRatio,
				aimShotBlendRatio,
				aimStepBlendRatio,
				collsionVector,
				frameCount
			);
		}
	}
};

CEREAL_CLASS_VERSION(AimMode, 12);

class Archer : public CharacterAI
{
public:

	Archer() = default;
	~Archer() = default;

	void Init() override;

	void Update(float& elapsedTime) override;

	void Render(ID3D11DeviceContext* immediateContext) override;

	void ImGui(ID3D11Device* device) override;

	void Impact() override;

	void RestAnimationIdle();

	template<class T>
	void serialize(T& archive, const std::uint32_t version)
	{
		if (version >= 6)
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
				m_aimMode
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
				m_collision,
				m_aimMode
			);
		}
	}
private:

	void ChangeCharacter();

	void Move(float& elapsedTime);
	void Step(float& elapsedTime);
	void Stepping(float& elapsedTime);
	bool KnockBack();

	void Aim();
	void Aiming();
	void AimMove(float& elapsedTime);
	void AimStep(float& elapsedTime);
	void AimStepping(float& elapsedTime);
	void Shot();


	void SerialVersionUpdate(uint32_t version)
	{
		m_blendAnimation.serialVersion = version;
		m_statusParm.serialVersion = version;
		m_moveParm.serialVersion = version;
		m_cameraParm.serialVersion = version;
		m_stepParm.serialVersion = version;
		m_aimMode.serialVersion = version;
		m_aimMode.aimCameraParm.serialVersion = version;
		m_aimMode.aimMoveParm.serialVersion = version;
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


	PrimitiveTask<ArcherWorldState>* m_primitiveTask;
	CompoundTask<ArcherWorldState> m_compoundTask;
	HTNPlanner m_planner;
};
