#include "Arrow.h"
#include ".\LibrarySource\ModelData.h"

#ifdef _DEBUG
#include "..\External_libraries\imgui\imgui.h"
#include "..\External_libraries\imgui\imgui_impl_dx11.h"
#include "..\External_libraries\imgui\imgui_impl_win32.h"
#include "..\External_libraries\imgui\imgui_internal.h"
#endif

void NormalArrow::Init()
{
	m_speed = {1.1f,1.0f,1.1f};
	m_velocity = {0.0f,0.0f,1.0f};
	m_attack = 0.0f;
	m_timer = 0;
	m_isExit = false;
}

void NormalArrow::Update()
{
//	m_velocity *= m_speed;
	++m_timer;
}

void NormalArrow::Relase()
{
	m_velocity = {};
	m_timer = 0;
	m_isExit = false;
}

void StrongArrow::Init()
{
	m_speed = {};
	m_velocity = {};
	m_attack = 0.0f;
	m_timer = 0;
	m_isExit = false;
	m_maxArrow = 0;
}

void StrongArrow::Update()
{
	m_velocity *= m_speed;
	++m_timer;
}

void StrongArrow::Relase()
{
	m_velocity = {};
	m_timer = 0;
	m_isExit = false;
}


void ArrowAdominist::Init()
{
	m_model = Source::ModelData::fbxLoader().GetStaticModel(Source::ModelData::StaticModel::ARROW);

	m_instanceData.resize(1);

	m_normalArrow = std::make_shared<NormalArrow>();
	m_normalArrow->Init();
	m_strongArrow = std::make_shared<StrongArrow>();
	m_strongArrow->Init();

	if (PathFileExistsA((std::string("../Asset/Binary/Player/Archer/Arrow") + ".bin").c_str()))
	{
		std::ifstream ifs;
		ifs.open((std::string("../Asset/Binary/Player/Archer/Arrow") + ".bin").c_str(), std::ios::binary);
		cereal::BinaryInputArchive i_archive(ifs);
		i_archive(*this);
	}
}

void ArrowAdominist::Update(float& elapsedTime)
{
	if (m_instanceData.empty()) return;

	if (!m_isShot) return;
		m_choisArrow->Update();

	for (auto& worldPram : m_instanceData)
	{
		VECTOR3F velocity = m_choisArrow->GetVelocity();
		VECTOR3F speed = m_choisArrow->GetSpeed();

		velocity *= speed;

		worldPram.position += velocity * elapsedTime;

		VECTOR3F distance = m_target - worldPram.position;
		worldPram.angle.y = atan2f(distance.x, distance.z);

		worldPram.CreateWorld();
	}

	if (m_survivalTime <= m_choisArrow->GetTimer())
	{
		m_choisArrow->Relase();
		m_isShot = false;
	}
}

void ArrowAdominist::Render(ID3D11DeviceContext* immediateContext)
{
	if (m_instanceData.empty()) return;
	if (!m_choisArrow) return;
	if (!m_choisArrow->GetExit()) return;

	m_model->Render(Framework::GetContext(), m_instanceData, VECTOR4F(1.0f, 1.0f, 1.0f, 1.0f));

}

void ArrowAdominist::SetArrow(const VECTOR3F& position, const VECTOR3F& angle, const VECTOR3F& target)
{
	m_instanceData[0].position = position;
	m_instanceData[0].angle = angle;
	m_instanceData[0].scale = m_scale;
	m_instanceData[0].color = { 1.0f,1.0f,1.0f,1.0f };

	m_choisArrow = m_normalArrow;

	VECTOR3F distance = target - position;
	m_instanceData[0].angle.y = atan2f(distance.x, distance.z);
	m_choisArrow->SetVelocity(NormalizeVec3(distance));
	m_choisArrow->SetExit(true);

	m_target = target;
	m_instanceData[0].CreateWorld();
}

void ArrowAdominist::ImGui(ID3D11Device* device)
{
#if _DEBUG
	ImGui::Begin("Arrow", nullptr, ImGuiWindowFlags_MenuBar);//メニューバーをつかうならこのBEGIN

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))//ファイルの中には
		{
			if (ImGui::MenuItem("Save"))//データの保存とか
			{
				std::ofstream ofs;
				ofs.open((std::string("../Asset/Binary/Player/Archer/Arrow") + ".bin").c_str(), std::ios::binary);
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
			ImGui::SliderFloat("Scale", &scale, 0.01f, 100.0f);
			m_scale = { scale,scale,scale };
		}
	}

	if (ImGui::CollapsingHeader("NormalArrow"))
	{
		//************
		// Speed
		//************
		{
			static float speed = m_normalArrow->GetSpeed().x;
			ImGui::SliderFloat("ArrowSpeed", &speed, 1.0f, 200.0f);
			VECTOR3F arrowSpeed = { speed,speed,speed };
			m_normalArrow->SetSpeed(arrowSpeed);
		}

		//***********
		// Attack
		//***********
		{
			static float attack = 1.0f;
			ImGui::SliderFloat("Attack", &attack, 1.0f, 20.0f);
			m_normalArrow->SetAttack(attack);
		}
	}

	//************
	// timer
	//************
	{
		 int time = m_normalArrow->GetTimer();
		ImGui::SliderInt("Time", &time, 1, 1000);
	}
	ImGui::End();
#endif
}



