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
		if (version >= 2)
		{
			archive
			(
				m_animData,
				m_color,
				m_scaleOffset
			);
		}
		else
		{
			archive
			(
				m_animData,
				m_color,
				m_scale
			);
		}
	}
private:
	int m_state;
	float m_timer;
	float m_scaleAdd;
	float m_scaleOffset;
	bool m_isActive;
};

class ArrowHitEffect : public BaseEffect
{
public:
	ArrowHitEffect() { Init(); };
	~ArrowHitEffect() = default;

	void Init() override;
	void Update(float& elapsedTime) override;
	void ImGui() override;

	template<class T>
	void serialize(T& archive, const std::uint32_t version)
	{
		if (version >= 2)
		{
			archive
			(
				m_animData,
				m_color,
				m_scaleOffset
			);
		}
		else
		{
			archive
			(
				m_animData,
				m_color,
				m_scaleOffset
			);
		}
	}
private:
	int m_state;
	float m_timer;
	float m_scaleAdd;
	float m_scaleOffset;
	bool m_isActive;
};

class HealEffect : public BaseEffect
{
public:
	HealEffect() { Init(); };
	~HealEffect() = default;

	void Init() override;
	void Update(float& elapsedTime) override;
	void ImGui() override;

	void SetTargetID(const int id) { m_targetID = id; }

	template<class T>
	void serialize(T& archive, const std::uint32_t version)
	{
		if (version >= 4)
		{
			archive
			(
				m_animData,
				m_color,
				m_scaleOffset,
				m_angleOffset,
				m_maxTime
			);
		}
		else
		{
			archive
			(
				m_animData,
				m_color,
				m_scaleOffset,
				m_angleOffset
			);
		}
	}
private:
	int m_state;
	int m_targetID;
	float m_timer;
	float m_angleOffset;
	float m_scaleOffset;
	float m_maxTime;
	bool m_isActive;
};