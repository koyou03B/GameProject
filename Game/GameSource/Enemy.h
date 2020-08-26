#pragma once
#include "CharacterAI.h"


class Enemy : public CharacterAI
{
public:
	Enemy() = default;
	~Enemy() = default;

	void Init() override;

	void Update(float& elapsedTime) override;

	void Render(ID3D11DeviceContext* immediateContext) override;

	void ImGui(ID3D11Device* device) override;

	template<class T>
	void serialize(T& archive, const std::uint32_t version)
	{
		if (version >= 0)
		{
			archive
			(
				m_collision
			);
		}
		else
		{
			archive
			(
				m_collision

			);
		}
	}
private:
	void SerialVersionUpdate(uint32_t version)
	{
		for (auto& coll : m_collision)
		{
			coll.serialVersion = version;
		}
	}
private:
	CharacterParameter::BlendAnimation		m_blendAnimation;
	CharacterParameter::DebugObjects		m_debugObjects;
	float m_elapsedTime;
};