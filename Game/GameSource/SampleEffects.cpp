#include "SampleEffects.h"
#include "MessengTo.h"
CEREAL_CLASS_VERSION(AttackEffect, 2);
CEREAL_CLASS_VERSION(ArrowHitEffect, 2);
CEREAL_CLASS_VERSION(HealEffect, 4);

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
	m_animData.label = EffectTextureLabel::P_Effect;

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
			m_scale = 7.0f;
			m_scaleOffset = 1.0f;
		}
	}
}

void AttackEffect::ImGui()
{
#ifdef _DEBUG
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
#endif
}

void ArrowHitEffect::Init()
{
	Reset();
	m_isActive = true;
	m_scaleOffset = 1.0f;
	m_state = 0;
	m_timer = 0.0f;
	m_scale = 5.0f;
	if (PathFileExistsA(std::string("../Asset/Binary/Effect/ArrowHitEffect.bin").c_str()))
	{
		std::ifstream ifs;
		ifs.open(std::string("../Asset/Binary/Effect/ArrowHitEffect.bin").c_str(), std::ios::binary);
		cereal::BinaryInputArchive i_archive(ifs);
		i_archive(*this);
	}
	m_animData.label = EffectTextureLabel::A_Effect;
	EffectTextureLoader::GetInstance()->LoadEffectTexture(m_animData.label);
}

void ArrowHitEffect::Update(float& elapsedTime)
{
	//if (m_isActive)
//	AnimationUpdate(elapsedTime);
	if (m_isActive)
	{
		m_scale += sinf(-m_scaleOffset * 0.01745f);
		m_scaleOffset += 1.0f;

		m_timer += elapsedTime;
		if (m_timer >= 0.3f)
		{
			m_isEnd = true;
			m_timer = 0.0f;
			m_scale = 5.0f;
			m_scaleOffset = 1.0f;
		}
	}
}

void ArrowHitEffect::ImGui()
{
#ifdef _DEBUG
	ImGui::Begin("AttackEffet", nullptr, ImGuiWindowFlags_MenuBar);//メニューバーをつかうならこのBEGIN

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))//ファイルの中には
		{
			if (ImGui::MenuItem("Save"))//データの保存とか
			{
				std::ofstream ofs;
				ofs.open((std::string("../Asset/Binary/Effect/ArrowHitEffect.bin")).c_str(), std::ios::binary);
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
	ImGui::SliderFloat("Scale", &m_scale, 0.0f, 20.0f);
	ImGui::InputFloat3("Color", color);
	ImGui::SliderFloat("ScaleOffset", &m_scaleOffset, .1f, 1.0f);
	m_position = { position[0],position[1],position[2] };
	m_angle = { angle[0],angle[1],angle[2] };
	m_color = { color[0],color[1],color[2],1.0f };

	int end = static_cast<int>(m_isEnd);
	ImGui::SliderInt("AnimationEnd", &end, 0, 1);
	m_isEnd = static_cast<bool>(end);
	ImGui::End();
#endif
}

void HealEffect::Init()
{
	Reset();
	m_isActive = true;
	m_state = 0;
	m_timer = 0.0f;
	m_scale = 10.4f;
	m_maxTime = 0.0f;
	m_angleOffset = 0.0f;
	m_scaleOffset = 0.0f;
	m_position.y = 6.0f;
	m_targetID = 0;
	if (PathFileExistsA(std::string("../Asset/Binary/Effect/HealEffect.bin").c_str()))
	{
		std::ifstream ifs;
		ifs.open(std::string("../Asset/Binary/Effect/HealEffect.bin").c_str(), std::ios::binary);
		cereal::BinaryInputArchive i_archive(ifs);
		i_archive(*this);
	}
	m_animData.label = EffectTextureLabel::Heal_Effect;
	EffectTextureLoader::GetInstance()->LoadEffectTexture(m_animData.label);
}

void HealEffect::Update(float& elapsedTime)
{
	if (m_isActive)
	{
		auto character = MESSENGER.CallPlayerInstance(static_cast<PlayerType>(m_targetID));
		if (character)
		{
			VECTOR3F position = character->GetWorldTransform().position;
			m_position = position;
			m_position.y = 5.0f;
		}
		m_scale += m_scaleOffset;
		m_angle.z += m_angleOffset * 0.01745f;
		m_timer += elapsedTime;
		if (m_timer >= m_maxTime)
		{
			m_isEnd = true;
			m_timer = 0.0f;
			m_scale = 10.4f;
			m_isActive = false;
		}
	}
}

void HealEffect::ImGui()
{
#ifdef _DEBUG
	ImGui::Begin("HealEffect", nullptr, ImGuiWindowFlags_MenuBar);//メニューバーをつかうならこのBEGIN

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))//ファイルの中には
		{
			if (ImGui::MenuItem("Save"))//データの保存とか
			{
				std::ofstream ofs;
				ofs.open((std::string("../Asset/Binary/Effect/HealEffect.bin")).c_str(), std::ios::binary);
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
	ImGui::SliderFloat("Scale", &m_scale, 0.0f, 20.0f);
	ImGui::InputFloat3("Color", color);
	ImGui::SliderFloat("Alpha", &m_color.w, 0.0f, 1.0f);
	ImGui::SliderFloat("ScaleOffset", &m_scaleOffset, -10.0f, 10.0f);
	ImGui::SliderFloat("AngleOffset", &m_angleOffset, 1.f, 5.0f);
	ImGui::SliderFloat("MaxTime", &m_maxTime, 1.f, 5.0f);

	m_position = { position[0],position[1],position[2] };
	m_angle = { angle[0],angle[1],angle[2] };
	m_color = { color[0],color[1],color[2],m_color.w };

	int end = static_cast<int>(m_isEnd);
	ImGui::SliderInt("AnimationEnd", &end, 0, 1);
	m_isEnd = static_cast<bool>(end);
	ImGui::End();
#endif
}
