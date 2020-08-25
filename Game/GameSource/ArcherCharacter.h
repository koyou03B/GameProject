#pragma once
#include "CharacterAI.h"
#include "CharacterParameter.h"
#include ".\LibrarySource\Vector.h"


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
	void serialize(T& archive)
	{
		archive
		(
			m_statusParm,
			m_cameraParm

		);
	}
private:
	CharacterParameter::BlendAnimation m_blendAnimation;
	CharacterParameter::DebugObjects   m_debugObjects;
};
