#include "SampleEffects.h"
CEREAL_CLASS_VERSION(AttackEffect, 1);

void AttackEffect::Init()
{
	Reset();
	m_isActive = true;
	if (PathFileExistsA(std::string("../Asset/Binary/Effect/AttackEffect.bin").c_str()))
	{
		std::ifstream ifs;
		ifs.open(std::string("../Asset/Binary/Effect/AttackEffect.bin").c_str(), std::ios::binary);
		cereal::BinaryInputArchive i_archive(ifs);
		i_archive(*this);
	}
	EffectTextureLoader::GetInstance()->LoadEffectTexture(m_animData.label);
}

void AttackEffect::Update(float& elapsedTime)
{
	if (m_isActive)
		AnimationUpdate(elapsedTime);
}

void AttackEffect::ImGui()
{
	ImGui::Begin("AttackEffet", nullptr, ImGuiWindowFlags_MenuBar);//メニューバーをつかうならこのBEGIN

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))//ファイルの中には
		{
			if (ImGui::MenuItem("Save"))//データの保存とか
			{
				std::ofstream ofs;
				ofs.open((std::string("../Asset/Binary/Effect/AttackEffect.bin")).c_str(), std::ios::binary);
				cereal::BinaryOutputArchive o_archive(ofs);
				o_archive(*this);
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}
	if (ImGui::Button("Active"))
	{
		m_isActive = true;
	}
	ImGui::SameLine();
	if (ImGui::Button("DeActive"))
	{
		m_isActive = false;
	}

#pragma region AnimationData
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
		auto texture = billBorad->GetShaderResourceView();
		ImGui::Image((void*)texture, ImVec2(100, 100));

	}
#pragma endregion

	float position[] = { m_position.x,m_position.y,m_position.z };
	float angle[] = { m_angle.x,m_angle.y,m_angle.z };
	float color[] = { m_color.x,m_color.y,m_color.z };
	ImGui::InputFloat3("Position", position);
	ImGui::InputFloat3("Angle", angle);
	ImGui::SliderFloat("Scale", &m_scale,1.0f,20.0f);
	ImGui::InputFloat3("Color", color);

	m_position = { position[0],position[1],position[2] };
	m_angle = { angle[0],angle[1],angle[2] };
	m_color = { color[0],color[1],color[2],1.0f };

	int end = static_cast<int>(m_isEnd);
	ImGui::SliderInt("AnimationEnd", &end,0,1);
	m_isEnd = static_cast<bool>(end);
	ImGui::End();
}
