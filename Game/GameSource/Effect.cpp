#include "Effect.h"

void BaseEffect::Render(ID3D11DeviceContext* immediateContext, const FLOAT4X4& projection, const FLOAT4X4& view)
{
	VECTOR3F position = {};
	position.x = m_position.x - m_animData.tileSize.x * 0.5f;
	position.y = m_position.y - m_animData.tileSize.y * 0.5f;
	position.z = m_position.z;
	m_animData.Render(immediateContext, projection,view,position,m_scale,m_angle,m_color);
}

void BaseEffect::AnimationUpdate(float& elapsedTime)
{
	float duration = m_animData.endTime / m_animData.tileMaxCount;
	
	m_animFrame += elapsedTime;
	if (m_animFrame > m_animData.endTime)
	{
		m_animFrame = 0.0f;
		m_tileCount = {};
		m_animData.tilePosition = {};
		m_isEnd = true;
		return;
	}

	if (m_animFrame > duration)
	{
		uint32_t xtileSize = static_cast<uint32_t>(m_animData.tileCount.x) - 1;
		if (m_tileCount.x > xtileSize)
		{
			m_tileCount.x = 0;
			++m_tileCount.y;
		}
		else
			++m_tileCount.x;
	}

	VECTOR2F tilePosition = {};
	tilePosition.x = m_tileCount.x * m_animData.tileSize.x;
	tilePosition.y = m_tileCount.y * m_animData.tileSize.y;
	m_animData.tilePosition = tilePosition;

}

void EffectManager::Init()
{
	m_sampleEffect.clear();
	m_selectedEffect.clear();

	if (PathFileExistsA(std::string("../Asset/Binary/Effect/SampleEffect.bin").c_str()))
	{
		std::ifstream ifs;
		ifs.open(std::string("../Asset/Binary/Effect/SampleEffect.bin").c_str(), std::ios::binary);
		cereal::BinaryInputArchive i_archive(ifs);
		i_archive(*this);
	}
}

void EffectManager::Update(float& elapsedTime)
{
	for (auto& effect : m_selectedEffect)
	{
		effect->Update(elapsedTime);
	}
}

void EffectManager::Render(ID3D11DeviceContext* immediateContext, const FLOAT4X4& projection, const FLOAT4X4& view)
{
	for (auto& effect : m_selectedEffect)
	{
		effect->Render(immediateContext, projection, view);
	}
}

void EffectManager::Clear()
{
	m_sampleEffect.clear();
	m_selectedEffect.clear();
}

void EffectManager::ImGui()
{
}

void EffectManager::AddEffect(BaseEffect& effect)
{
	m_sampleEffect.push_back(std::make_shared<BaseEffect>(effect));
}

void EffectManager::SelectEffect(const EffectType& type, const int count)
{
	m_selectedEffect.push_back(m_sampleEffect.at(type));
}
