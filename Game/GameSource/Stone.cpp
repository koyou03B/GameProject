#include "Stone.h"
#include ".\LibrarySource\ModelData.h"

#ifdef _DEBUG
#include "..\External_libraries\imgui\imgui.h"
#include "..\External_libraries\imgui\imgui_impl_dx11.h"
#include "..\External_libraries\imgui\imgui_impl_win32.h"
#include "..\External_libraries\imgui\imgui_internal.h"
#endif
CEREAL_CLASS_VERSION(StoneAdominist, 1);
void Stone::Init()
{
	m_speed = { 150.0f,0.0f,150.0f };
	m_velocity = { 0.0f,1.0f,1.0f };
	m_attack = 0.0f;
	m_timer = 0;
	m_isExit = false;
}

void Stone::Update(float& elapsedTime)
{
	m_timer+= elapsedTime;
}

void Stone::Relase()
{
	m_velocity = {};
	m_timer = 0;
	m_isExit = false;
}


void StoneAdominist::Init()
{
	m_model = Source::ModelData::fbxLoader().GetStaticModel(Source::ModelData::StaticModel::STONE);
	m_instanceData.resize(1);

	m_stone = std::make_shared<Stone>();
	m_stone->Init();

	if (PathFileExistsA((std::string("../Asset/Binary/Enemy/Stone/Stone") + ".bin").c_str()))
	{
		std::ifstream ifs;
		ifs.open((std::string("../Asset/Binary/Enemy/Stone/Stone") + ".bin").c_str(), std::ios::binary);
		cereal::BinaryInputArchive i_archive(ifs);
		i_archive(*this);
	}
}

void StoneAdominist::Update(float& elapsedTime)
{
	if (m_instanceData.empty()) return;

	if (!m_isFly) return;
	//	m_stone->Update(elapsedTime);

	for (auto& worldPram : m_instanceData)
	{
		VECTOR3F velocity = m_stone->GetVelocity();
		VECTOR3F speed = m_stone->GetSpeed();
	
		velocity *= speed;
		velocity.y = m_power;
		worldPram.position += velocity * elapsedTime;
		m_power -= 0.5f;
		//VECTOR3F distance = m_target - worldPram.position;
		//worldPram.angle.y = atan2f(distance.x, distance.z);

		worldPram.CreateWorld();

		if (worldPram.position.y <= 0)
		{
			Reset();
		}
	}

	//if (m_survivalTime <= m_stone->GetTimer())
	//{
	//	m_stone->Relase();
	//	m_isFly = false;
	//}
}

void StoneAdominist::Render(ID3D11DeviceContext* immediateContext)
{
	if (m_instanceData.empty()) return;
	if (!m_stone) return;
	if (!m_stone->GetExit()) return;

	m_model->Render(Framework::GetContext(), m_instanceData, VECTOR4F(1.0f, 1.0f, 1.0f, 1.0f));

}

void StoneAdominist::SetStone(const VECTOR3F& position, const VECTOR3F& angle, const VECTOR3F& target)
{
	for (auto& stone : m_instanceData)
	{
		stone.position = position;
		stone.angle = angle;
		stone.scale = m_scale;
		stone.color = { 1.0f,1.0f,1.0f,1.0f };
		stone.CreateWorld();
	}
	m_stone->SetVelocity(target);
	m_stone->SetExit(true);
	m_isFly = true;
}

void StoneAdominist::ImGui(ID3D11Device* device)
{
#if _DEBUG
	ImGui::Begin("Stone", nullptr, ImGuiWindowFlags_MenuBar);//メニューバーをつかうならこのBEGIN

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))//ファイルの中には
		{
			if (ImGui::MenuItem("Save"))//データの保存とか
			{
				std::ofstream ofs;
				ofs.open((std::string("../Asset/Binary/Enemy/Stone/Stone") + ".bin").c_str(), std::ios::binary);
				cereal::BinaryOutputArchive o_archive(ofs);
				o_archive(*this);
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	if (ImGui::CollapsingHeader("Adominist"))
	{
		//************
		// timer
		//************
		{
			static int survivalTime = m_survivalTime;
			ImGui::SliderInt("SurvivalTime", &survivalTime, 1, 100);
			m_survivalTime = survivalTime;
		}

		//************
		// Scale
		//************
		{
			static float scale = m_scale.x;
			ImGui::SliderFloat("Scale", &scale, 0.001f, 1.0f);
			m_scale = { scale,scale,scale };
			m_instanceData[0].scale = m_scale;
			m_instanceData[0].CreateWorld();
		}
	}


	ImGui::End();
#endif
}

