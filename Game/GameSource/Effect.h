#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <Shlwapi.h>
#include "AnimationData.h"

#undef max
#undef min
#include<cereal/types/memory.hpp>
#include<cereal/types/vector.hpp>

class BaseEffect
{
public:
	BaseEffect() = default;
	virtual ~BaseEffect() = default;

	virtual void Init() = 0;
	virtual void Update(float& elapsedTime) = 0;
	virtual void Render(ID3D11DeviceContext* immediateContext, const FLOAT4X4& projection,const FLOAT4X4& view);
	virtual void ImGui() = 0;

	void AnimationUpdate(float& elapsedTime);

	inline VECTOR3F& GetPosition() { return m_position; }
	inline VECTOR3F& GetAngle() { return m_angle; }
	inline VECTOR4F& GetColor() { return m_color; }
	inline float& GetScale() { return m_scale; }
	inline float& GetAnimFrame() { return m_animFrame; }
	inline bool& GetIsEnd() { return m_isEnd; }
	inline bool& GetIsLooping() { return m_isLooping; }

	void SetPosition(const VECTOR3F& position) { m_position = position; }
	void SetAngle(const VECTOR3F& angle) { m_angle = angle; }
	void SetColor(const VECTOR4F& color) { m_color = color; }
	void SetScale(const float& scale) { m_scale = scale; }
	void SetAnimFrame(const float& animFrame) { m_animFrame = animFrame; }
	void SetIsEnd(const bool& isEnd) { m_isEnd = isEnd; }
	void SetIsLooping(const bool& isLooping) { m_isLooping = isLooping; }

	template<class T>
	void serialize(T& archive, const std::uint32_t version)
	{
		if (version >= 0)
		{
			archive
			(
				m_animData
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

protected:
	AnimationData m_animData;
	UVECTOR2 m_tileCount;
	VECTOR3F m_position;
	VECTOR3F m_angle;
	VECTOR4F m_color;
	float m_scale;
	float m_animFrame;
	bool m_isEnd;
	bool m_isLooping;
};

enum EffectType
{
	FighterAttack,
	ArrowAura,
	ArrowAttack,
	EnemyAttack,
	EffectEND
};

class EffectManager
{
public:
	EffectManager() = default;
	~EffectManager() = default;

	void Init();
	void Update(float& elapsedTime);
	void Render(ID3D11DeviceContext* immediateContext, const FLOAT4X4& projection, const FLOAT4X4& view);
	void Clear();
	void ImGui();

	void AddEffect(BaseEffect& effect);
	void SelectEffect(const EffectType& type, const int count);

	template<class T>
	void serialize(T& archive, const std::uint32_t version)
	{
		if (version >= 0)
		{
			archive
			(
				m_sampleEffect
			);
		}
		else
		{
			archive
			(
				m_sampleEffect
			);
		}
	}
private:
	std::vector<std::shared_ptr<BaseEffect>> m_sampleEffect;
	std::vector<std::shared_ptr<BaseEffect>> m_selectedEffect;
};