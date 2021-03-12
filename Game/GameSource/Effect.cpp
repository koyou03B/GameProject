#include <iostream>
#include <random>
#include <iomanip>
#include "Effect.h"
#include "SampleEffects.h"

void BaseEffect::Render(ID3D11DeviceContext* immediateContext, const FLOAT4X4& projection, const FLOAT4X4& view)
{
	VECTOR3F position = {};
	position.x = m_position.x - (m_animData.tileSize.x * m_scale) * 0.5f;
	position.y = m_position.y - (m_animData.tileSize.y * m_scale) * 0.5f;
	position.z = m_position.z;
	m_animData.Render(immediateContext, projection,view, m_position,m_scale,m_angle,m_color);
}

void BaseEffect::Reset()
{
	SecureZeroMemory(&m_animData, sizeof(m_animData));
	m_tileCount = {};
	m_position = {};
	m_angle = {};
	m_color = {};
	m_animRota = 0;
	m_scale = 0.0f;
	m_animFrame = 0.0f;
	m_isEnd = false;
	m_isLooping = false;

}

void BaseEffect::AnimationUpdate(float& elapsedTime)
{
	float duration = m_animData.endTime / m_animData.tileMaxCount;
	
	m_animFrame += elapsedTime;
	if (m_animRota >= m_animData.tileMaxCount)
	{
		m_animRota = 0;
		m_animFrame = 0.0f;
		m_tileCount = {};
		m_animData.tilePosition = {};
		m_isEnd = true;
		return;
	}

	if (m_animFrame > duration)
	{
		uint32_t xtileSize = static_cast<uint32_t>(m_animData.tileCount.x) - 1;
		if (m_tileCount.x >= xtileSize)
		{
			m_tileCount.x = 0;
			++m_tileCount.y;
		}
		else
			++m_tileCount.x;
		m_animFrame = 0.0f;
		++m_animRota;
	}

	VECTOR2F tilePosition = {};
	tilePosition.x = m_tileCount.x * m_animData.tileSize.x;
	tilePosition.y = m_tileCount.y * m_animData.tileSize.y;
	m_animData.tilePosition = tilePosition;

}

void BaseEffect::ImGuiOfAnimData()
{
	int tileCount[] = { static_cast<int>(m_animData.tileCount.x),static_cast<int>(m_animData.tileCount.y) };
	float tileSize[] = { m_animData.tileSize.x,m_animData.tileSize.y };
	float tilePosition[] = { m_animData.tilePosition.x,m_animData.tilePosition.y };
	ImGui::InputFloat("EndTime", &m_animData.endTime);
	ImGui::InputInt2("TileCount", tileCount);
	m_animData.tileCount = { static_cast<float>(tileCount[0]),static_cast<float>(tileCount[1]) };
	ImGui::InputInt("MaxTileCount", &m_animData.tileMaxCount);
	ImGui::InputFloat2("TileSize", tileSize);
	m_animData.tileSize = { tileSize[0],tileSize[1] };
	ImGui::InputFloat2("TilePosition", tilePosition);
	m_animData.tilePosition = { tilePosition[0] * tileSize[0],tilePosition[1] * tileSize[1] };

	int label = static_cast<int>(m_animData.label);
	ImGui::Combo("EffectLabel", &label, "GREEN_SPLASH\0WHITE_SPLASH\0GREEN_WHITE_SPLASH\0RED_SPLASH\0BLUE_RED_IMPACT\0BLUE_RED_SPLASH\0\0");
	m_animData.label = static_cast<EffectTextureLabel>(label);
	if (ImGui::Button("Texture"))
	{
		EffectTextureLoader::GetInstance()->LoadEffectTexture(m_animData.label);
	}
	auto billBorad = EffectTextureLoader::GetInstance()->GetBillBoard(m_animData.label);
	if (billBorad)
	{
		ImGui::Begin("EffectTexture");
		auto texture = billBorad->GetShaderResourceView();
		ImGui::Image((void*)texture, ImVec2(300, 300));
		ImGui::End();
	}
}

void EffectAdominist::Init()
{
	m_sampleEffect.clear();
	m_selectedEffect.clear();
	m_selectSmapleEffect = 0;
	m_isSampleActive = false;
	m_sampleEffect.push_back(std::make_unique<AttackEffect>());
}

void EffectAdominist::Update(float& elapsedTime)
{
	if (m_isSampleActive)
	{
		for (auto& effect : m_sampleEffect)
		{
			if(!effect->GetIsEnd())
			effect->Update(elapsedTime);
		}
	}

	for (auto& effect : m_selectedEffect)
	{
		effect->Update(elapsedTime);
	}

	m_selectedEffect.erase(std::remove_if(m_selectedEffect.begin(), m_selectedEffect.end(),
		[](const std::unique_ptr<BaseEffect>& effect)
		{
			if (effect->GetIsEnd())
			{
				effect->SetIsEnd(false);
				return true;
			}
			return false;
		}), m_selectedEffect.end());
}

void EffectAdominist::Render(ID3D11DeviceContext* immediateContext, const FLOAT4X4& projection, const FLOAT4X4& view)
{
	if (m_isSampleActive)
	{
		for (auto& effect : m_sampleEffect)
		{
			effect->Render(immediateContext, projection, view);
		}
	}


	for (auto& effect : m_selectedEffect)
	{
		effect->Render(immediateContext, projection, view);
	}
}

void EffectAdominist::Clear()
{
	m_sampleEffect.clear();
	m_selectedEffect.clear();
}

void EffectAdominist::ImGui()
{
	ImGui::Begin("EffectManager");

	static bool isDebug = false;
	int sampleCount = static_cast<int>(m_sampleEffect.size());
	ImGui::DragInt("Select Sample Count", &sampleCount, 1.0f,sampleCount, sampleCount);
	ImGui::DragInt("Select Sample", &m_selectSmapleEffect, 0, sampleCount);
	if(ImGui::Button("SampleRender"))
		m_isSampleActive = true;
	ImGui::SameLine();

	if (ImGui::Button("Debug Active"))
	{
		isDebug = true;
	}
	ImGui::SameLine();
	if (ImGui::Button("Debug DeActive"))
	{
		isDebug = false;
		m_isSampleActive = false;
	}

	if (isDebug)
	{
		m_sampleEffect.at(m_selectSmapleEffect)->ImGui();
	}


	ImGui::End();
}

void EffectAdominist::AddEffect(BaseEffect* effect)
{
//	m_sampleEffect.push_back(std::make_shared<BaseEffect>(effect));
}

void EffectAdominist::SelectEffect(const EffectType& type, const VECTOR3F& position,const int count)
{
	VECTOR3F effectPos = position;
	std::random_device rd;
	std::default_random_engine eng(rd());
	std::uniform_real_distribution<float> distrX(-1.5f, 1.5f);
	std::uniform_real_distribution<float> distrY(-1.0f, 1.0f);
	std::uniform_real_distribution<float> distrZ(-0.5f, 0.5f);

	for (int i = 0; i < count; ++i)
	{
		switch (type)
		{
		case EffectType::FighterAttack:
			AttackEffect effect;
			memcpy(&effect, m_sampleEffect.at(type).get(), sizeof(AttackEffect));
			m_selectedEffect.push_back(std::make_unique<AttackEffect>(effect));

			break;
		}

		m_selectedEffect.back()->SetPosition(effectPos);
		srand((unsigned)time(NULL));
		effectPos.x += distrX(eng);
		//effectPos.y += distrY(eng);
		//effectPos.z += distrZ(eng);
	}
	std::sort(m_selectedEffect.begin(), m_selectedEffect.end(), m_operatDepth);

}
