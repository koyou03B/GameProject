#include "Stone.h"
#include "MessengTo.h"
#include ".\LibrarySource\ModelData.h"

#ifdef _DEBUG
#include "..\External_libraries\imgui\imgui.h"
#include "..\External_libraries\imgui\imgui_impl_dx11.h"
#include "..\External_libraries\imgui\imgui_impl_win32.h"
#include "..\External_libraries\imgui\imgui_internal.h"
#endif
CEREAL_CLASS_VERSION(Stone, 2);

void Stone::Init()
{
	Clear();
	m_model = Source::ModelData::fbxLoader().GetStaticModel(Source::ModelData::StaticModel::STONE);
	m_stoneData.resize(3);
	m_renderData.resize(3);
	SetStoneParameter();
}

void Stone::Update(float& elapsedTime)
{
	if (m_stoneData.empty()) return;
	using CollisionType = CharacterParameter::Collision::CollisionType;

	for (auto& stone : m_stoneData)
	{
		if (stone.second.isFlying)
		{
			VECTOR3F velocity = stone.second.velocity;
			VECTOR3F speed = stone.second.speed;
			float upPower = stone.second.upPower;

			velocity *= speed;
			velocity.y = upPower;
			stone.second.upPower -= 0.5f;
			stone.first.position += velocity * elapsedTime;

			stone.first.CreateWorld();

			m_collision.position[0] = stone.first.position;
			m_collision.position[1] = stone.first.position - velocity * elapsedTime;
			m_collision.scale = 1.0f;
			m_collision.radius = 1.5f;
			m_collision.collisionType = CollisionType::SPHER;
			if (MESSENGER.AttackingMessage(EnemyType::Boss, m_collision))
			{
				Reset(stone);
				return;
			}

			if (stone.first.position.y <= -3.0f)
			{
				Reset(stone);
			}
		}
	}
}

void Stone::Render(ID3D11DeviceContext* immediateContext)
{
	int count = 0;
	if (m_stoneData.empty()) return;
	size_t size = m_stoneData.size();
	for (size_t i = 0; i < size; ++i)
	{
		if (m_stoneData[i].second.isFlying)
		{
			++count;
			m_renderData[i] = m_stoneData[i].first;
		}
		else
			m_renderData[i].ClearData();
	}
	if(count != 0)
		m_model->Render(Framework::GetContext(), m_renderData, VECTOR4F(1.0f, 1.0f, 1.0f, 1.0f));
}

void Stone::Clear()
{
	if (!m_stoneData.empty())
	{
		for (auto& stone : m_stoneData)
		{
			stone.first.ClearData();
			stone.second.Clear();
		}	
	}
}

void Stone::Reset(std::pair<InstanceData, StoneParam>& stoneData)
{
	stoneData.first.ClearData();
	stoneData.second.isFlying = false;
	stoneData.second.upPower = m_stoneUpPower;
	stoneData.second.velocity = {};
}

void Stone::Release()
{
	Clear();
	m_model.reset();
	m_renderData.clear();
}

void Stone::PrepareForStone(const VECTOR3F& position, const VECTOR3F& angle, const VECTOR3F& velocity)
{
	for (auto& stone : m_stoneData)
	{
		if (stone.second.isFlying) continue;
		stone.first.position = position;
		stone.first.angle = angle;
		stone.first.scale = m_stoneScale;
		stone.first.color = { 1.0f,1.0f,1.0f,1.0f };
		stone.first.CreateWorld();
		stone.second.velocity = velocity;
		stone.second.isFlying = true;
		return;
	}
}

void Stone::SetStoneParameter()
{
	if (PathFileExistsA((std::string("../Asset/Binary/Enemy/Stone/Stone") + ".bin").c_str()))
	{
		std::ifstream ifs;
		ifs.open((std::string("../Asset/Binary/Enemy/Stone/Stone") + ".bin").c_str(), std::ios::binary);
		cereal::BinaryInputArchive i_archive(ifs);
		i_archive(*this);
	}

	m_stoneData[0].second.ReadBinary();
	m_stoneData[1].second.ReadBinary();
	m_stoneData[2].second.ReadBinary();
}

void Stone::ImGui(ID3D11Device* device)
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

	if (ImGui::CollapsingHeader("Scale"))
	{
		static float scale = m_stoneScale.x;
		ImGui::SliderFloat("Scale", &scale, 0.001f, 10.0f);
		m_stoneScale = { scale,scale,scale };
		if (!m_stoneData.empty())
		{
			m_stoneData.begin()->first.scale = m_stoneScale;
			m_stoneData.begin()->first.CreateWorld();
		}
	}

	if (ImGui::CollapsingHeader("Offset"))
	{
		static int current = 0;
		ImGui::RadioButton("Z-Axis", &current, 0); ImGui::SameLine();
		ImGui::RadioButton("X-Axis", &current, 1);

		if (current == 0)
		{
			VECTOR3F offset = m_offsetZ;
			float offsetZ[] = { offset.x,offset.y,offset.z };
			ImGui::SliderFloat3("Z-offset", offsetZ, -8.0f, 8.0f);
			offset = { offsetZ[0],offsetZ[1],offsetZ[2] };
			m_offsetZ = offset;
		}
		else
		{
			VECTOR3F offset = m_offsetX;
			float offsetX[] = { offset.x,offset.y,offset.z };
			ImGui::SliderFloat3("X-offset", offsetX, -7.0f, 7.0f);
			offset = { offsetX[0],offsetX[1],offsetX[2] };
			m_offsetX = offset;
		}

	}

	if (!m_stoneData.empty())
	{
		static int current = 0;
		ImGui::RadioButton("Zero", &current, 0); ImGui::SameLine();
		ImGui::RadioButton("OWE", &current, 1); ImGui::SameLine();
		ImGui::RadioButton("TWO", &current, 2);

		auto& stone = m_stoneData.at(current);
		if (ImGui::CollapsingHeader("Speed"))
		{
			static float speed = stone.second.speed.x;
			ImGui::SliderFloat("Speed", &speed, 0.0f, 200.0f);
			stone.second.speed = { speed,speed,speed };
		}
		if (ImGui::CollapsingHeader("UpPower"))
		{
			static float upPower = stone.second.upPower;
			ImGui::SliderFloat("UpPower->", &upPower, 0.0f, 20.0f);
			m_stoneUpPower = stone.second.upPower = upPower;
		}

		ImGui::Text("IsFly%d", stone.second.isFlying);

		if (ImGui::Button("Reset"))
			stone.first.position.y = -10.0f;

		if (ImGui::Button("Save"))
			m_stoneData.at(current).second.SaveBinary();
	}
	ImGui::End();
#endif
}

