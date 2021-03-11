#pragma once
#include "Effect.h"

class AttackEffect : public BaseEffect
{
public:
	AttackEffect() { Init(); };
	~AttackEffect() = default;

	void Init() override;
	void Update(float& elapsedTime) override;
	void ImGui() override;

	template<class T>
	void serialize(T& archive, const std::uint32_t version)
	{
		if (version >= 1)
		{
			archive
			(
				m_animData,
				m_color,
				m_scale
			);
		}
		else
		{
			archive
			(
				m_animData
			);
		}
	}
private:
	bool m_isActive;
};