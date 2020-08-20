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

	void ImGui(ID3D11Device* device) override;

	template<class T>
	void serialize(T& archive, const std::uint32_t version)
	{
		if (version >= 1)
		{
			archive
			(
				m_status,
				m_cameraParm,
				m_stepParm

			);
		}
		else
		{
			archive
			(
				m_status,
				m_cameraParm,
				m_stepParm

			);
		}
	}
private:
	enum Animation
	{
		IDLE=1,
		MOVE = 3,
		DIVE = 4,
		ATTACK,
		IMPACT = 8,
		DEATH,
		IDLE2,
		ARMSET,
		FIGHTIDLE,
		HEAL
	} m_animationType = IDLE;
private:
	CharacterParameter::BlendAnimation m_blendAnimation;
	CharacterParameter::DebugObjects   m_debugObjects;
	CharacterParameter::Step		   m_stepParm;
	Source::Input::Input* m_input;
	float m_padDeadLine;
	
};
