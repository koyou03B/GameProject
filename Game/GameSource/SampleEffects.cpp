#include "SampleEffects.h"
CEREAL_CLASS_VERSION(AttackEffect, 2);

void AttackEffect::Init()
{
	Reset();
	m_isActive = true;
	m_scaleOffset = 1.0f;
	m_state = 0;
	m_timer = 0.0f;
	m_scale = 7.0f;
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
	//if (m_isActive)
	//	AnimationUpdate(elapsedTime);
	if (m_isActive)
	{
		m_scale += sinf(-m_scaleOffset*0.01745f);
		m_scaleOffset += 1.0f;

		m_timer += elapsedTime;
		if (m_timer >= 0.30f)
		{
			m_isEnd = true;
			m_timer = 0.0f;
			m_scale = 3.0f;
			m_scaleOffset = 10.0f;
		}
	}
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
	ImGuiOfAnimData();
#pragma endregion

	float position[] = { m_position.x,m_position.y,m_position.z };
	float angle[] = { m_angle.x,m_angle.y,m_angle.z };
	float color[] = { m_color.x,m_color.y,m_color.z };
	ImGui::InputFloat3("Position", position);
	ImGui::InputFloat3("Angle", angle);
	ImGui::SliderFloat("Scale", &m_scale,0.0f,20.0f);
	ImGui::InputFloat3("Color", color);
	ImGui::SliderFloat("ScaleOffset", &m_scaleOffset, .1f, 1.0f);
	m_position = { position[0],position[1],position[2] };
	m_angle = { angle[0],angle[1],angle[2] };
	m_color = { color[0],color[1],color[2],1.0f };

	int end = static_cast<int>(m_isEnd);
	ImGui::SliderInt("AnimationEnd", &end,0,1);
	m_isEnd = static_cast<bool>(end);
	ImGui::End();
}
