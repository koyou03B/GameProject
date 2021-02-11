#include "Arrow.h"
#include "MessengTo.h"
#include ".\LibrarySource\ModelData.h"

#ifdef _DEBUG
#include "..\External_libraries\imgui\imgui.h"
#include "..\External_libraries\imgui\imgui_impl_dx11.h"
#include "..\External_libraries\imgui\imgui_impl_win32.h"
#include "..\External_libraries\imgui\imgui_internal.h"
#endif
CEREAL_CLASS_VERSION(Arrow, 2);

void Arrow::Init()
{
	Clear();
	m_model = Source::ModelData::fbxLoader().GetStaticModel(Source::ModelData::StaticModel::ARROW);
	m_arrowData.resize(3);
	m_renderData.resize(3);
	SetArrowParameter();
}

void Arrow::Update(float& elapsedTime)
{
	if (m_arrowData.empty()) return;
	using CollisionType = CharacterParameter::Collision::CollisionType;

	for (auto& arrow : m_arrowData)
	{
		if (arrow.second.isFlying)
		{
			VECTOR3F velocity = arrow.second.velocity;
			VECTOR3F speed = arrow.second.speed;
			arrow.second.fallTime += elapsedTime;

			if(arrow.second.fallTime >= m_defineFallTime)
				velocity.y -= m_arrowFallPower;
			velocity *= speed;
			arrow.first.position += velocity * elapsedTime;

			arrow.first.CreateWorld();

			m_collision.position[0] = arrow.first.position;
			m_collision.position[1] = arrow.first.position - velocity * elapsedTime;
			m_collision.scale = 1.0f;
			m_collision.radius = 2.0f;
			m_collision.collisionType = CollisionType::CAPSULE;
			if (MESSENGER.AttackingMessage(static_cast<int>(1), m_collision))
			{
				Reset(arrow);
				return;
			}

			if (arrow.first.position.y <= 0.0f)
			{
				Reset(arrow);
			}
		}
	}
}

void Arrow::Render(ID3D11DeviceContext* immediateContext)
{
	int count = 0;
	if (m_arrowData.empty()) return;
	size_t size = m_arrowData.size();
	for (size_t i = 0; i < size; ++i)
	{
		if (m_arrowData[i].second.isFlying)
		{
			++count;
			m_renderData[i] = m_arrowData[i].first;
		}
		else
			m_renderData[i].ClearData();
	}
	if (count != 0)
		m_model->Render(Framework::GetContext(), m_renderData, VECTOR4F(1.0f, 1.0f, 1.0f, 1.0f));
}

void Arrow::Clear()
{
	if (!m_arrowData.empty())
	{
		for (auto& arrow : m_arrowData)
		{
			arrow.first.ClearData();
			arrow.second.Clear();
		}
	}
}

void Arrow::Reset(std::pair<InstanceData, ArrowParam>& stoneData)
{
	stoneData.first.ClearData();
	stoneData.second.isFlying = false;
	stoneData.second.velocity = {};
}

void Arrow::Release()
{
	Clear();
	m_model.reset();
	m_renderData.clear();
}

void Arrow::PrepareForArrow(const VECTOR3F& position, const VECTOR3F& angle, const VECTOR3F& velocity)
{
	for (auto& arrow : m_arrowData)
	{
		if (arrow.second.isFlying) continue;
		arrow.first.position = position;
		arrow.first.angle = angle;
		arrow.first.scale = m_arrowScale;
		arrow.first.color = { 1.0f,1.0f,1.0f,1.0f };
		arrow.first.CreateWorld();
		arrow.second.velocity = velocity;
		arrow.second.isFlying = true;
		return;
	}
}

void Arrow::SetArrowParameter()
{
	if (PathFileExistsA((std::string("../Asset/Binary/Player/Archer/Arrow") + ".bin").c_str()))
	{
		std::ifstream ifs;
		ifs.open((std::string("../Asset/Binary/Player/Archer/Arrow") + ".bin").c_str(), std::ios::binary);
		cereal::BinaryInputArchive i_archive(ifs);
		i_archive(*this);
	}

	//m_arrowData[0].second.ReadBinary();
	//m_arrowData[1].second.ReadBinary();
	//m_arrowData[2].second.ReadBinary();
}

void Arrow::ImGui(ID3D11Device* device)
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

	if (ImGui::CollapsingHeader("Scale"))
	{
		static float scale = m_arrowScale.x;
		ImGui::SliderFloat("Scale", &scale, 0.001f, 10.0f);
		m_arrowScale = { scale,scale,scale };
		if (!m_arrowData.empty())
		{
			m_arrowData.begin()->first.scale = m_arrowScale;
			m_arrowData.begin()->first.CreateWorld();
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

	if (!m_arrowData.empty())
	{
		static int current = 0;
		ImGui::RadioButton("Zero", &current, 0); ImGui::SameLine();
		ImGui::RadioButton("OWE", &current, 1); ImGui::SameLine();
		ImGui::RadioButton("TWO", &current, 2);

		auto& stone = m_arrowData.at(current);
		if (ImGui::CollapsingHeader("Speed"))
		{
			static float speed = stone.second.speed.x;
			ImGui::SliderFloat("Speed", &speed, 0.0f, 200.0f);
			stone.second.speed = { speed,speed,speed };
		}
		if (ImGui::CollapsingHeader("FallPower"))
		{
			static float fallPower = m_arrowFallPower;
			ImGui::SliderFloat("FallPower->", &fallPower, 0.0f, 20.0f);
			m_arrowFallPower = fallPower;
		}
		if (ImGui::CollapsingHeader("DefainFallTime"))
		{
			static float dFallTime = m_defineFallTime;
			ImGui::SliderFloat("DefainFallTime->", &dFallTime, 0.0f, 20.0f);
			m_defineFallTime = dFallTime;
		}
		ImGui::Text("IsFly%d", stone.second.isFlying);

		if (ImGui::Button("Reset"))
			stone.first.position.y = -10.0f;

		if (ImGui::Button("Save"))
			m_arrowData.at(current).second.SaveBinary();
	}
	ImGui::End();
#endif
}