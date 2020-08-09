#pragma once
#include "PlayCharacters.h"
#include "CharacterParameter.h"
#include ".\LibrarySource\Vector.h"


class Saber : public PlayCharacter
{
public:

	Saber() = default;
	~Saber() = default;

	void Init() override;

	void Update(float& elapsedTime) override;

	void Render(ID3D11DeviceContext* immediateContext) override;

	void ImGui(ID3D11Device* device) override;
private:
	CharacterParameter::WorldTransform m_transformParm;
	CharacterParameter::BlendAnimation m_blendAnimation;
	CharacterParameter::DebugObjects   m_debugObjects;
};
