#include "Stage.h"
#include "MessengTo.h"

void Stage::Init()
{
	m_position = { 0.0f,0.0f,0.0f };
	m_angle = { 0.0f * 0.01745f, 90.0f * 0.01745f,0.0f * 0.017454f };
	m_scale = { 13.0f,13.0f,13.0f };
	DirectX::XMMATRIX S, R, T;
	S = DirectX::XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);
	R = DirectX::XMMatrixRotationRollPitchYaw(m_angle.x, m_angle.y, m_angle.z);
	T = DirectX::XMMatrixTranslation(m_position.x, m_position.y, m_position.z);
	DirectX::XMMATRIX W = S * R * T;
	DirectX::XMStoreFloat4x4(&m_world, W);
	AddInstanceData(m_position, m_angle, m_scale, VECTOR4F(1.0f, 1.0f, 1.0f, 1.0f));
	m_model[0] = Source::ModelData::fbxLoader().GetStaticModel(Source::ModelData::StaticModel::STAGE);
	m_model[1] = Source::ModelData::fbxLoader().GetStaticModel(Source::ModelData::StaticModel::FENCE);
//	Source::ModelData::fbxLoader().SaveStaticForBinary(Source::ModelData::StaticModel::STAGE);
//	Source::ModelData::fbxLoader().SaveStaticForBinary(Source::ModelData::StaticModel::FENCE);

	//m_model->_shaderON.normalMap = true;

	m_circle.radius = 81.0f;
	m_circle.position = {};
	m_circle.scale = 1.0f;
	m_state = 0;
	m_timer = .0f;
	if (PathFileExistsA((std::string("../Asset/Binary/Stage/fence.bin")).c_str()))
	{
		std::ifstream ifs;
		ifs.open((std::string("../Asset/Binary/Stage/fence.bin")).c_str(), std::ios::binary);
		cereal::BinaryInputArchive i_archive(ifs);
		i_archive(*this);
	}
}

void Stage::Update(float& elapsedTime)
{
	int count = static_cast<int>(PlayerType::End);
	count += static_cast<int>(EnemyType::End);
	for (int i = 0; i < count; ++i)
	{
		CharacterAI* character = nullptr;
		if (i == 0 || i == 1)
		{
			PlayerType type = static_cast<PlayerType>(i);
			character = MESSENGER.CallPlayerInstance(type);
		}
		else
		{
			character = MESSENGER.CallEnemyInstance(EnemyType::Boss);
		}
		if (character == nullptr) continue;

		Collision coll;
		VECTOR3F pos = character->GetCollision().at(0).position[0];
		VECTOR2F target = { pos.x,pos.z };
		if (coll.JudgeCircleAndpoint(m_circle, target))
		{
			VECTOR2F distance;
			distance.x = m_circle.position.x - target.x;
			distance.y = m_circle.position.y - target.y;

			float length = sqrtf(distance.x * distance.x + distance.y * distance.y);
			float sub = m_circle.radius - length;
			distance.x /= length;
			distance.y /= length;
			distance.x *= sub;
			distance.y *= sub;
			character->GetWorldTransform().position.x -= distance.x;
			character->GetWorldTransform().position.z -= distance.y;
			character->GetWorldTransform().WorldUpdate();

		}
	}
}

bool Stage::FanceBreak(float& elapsedTime)
{
	switch (m_state)
	{
	case 0:
	{
		m_timer += elapsedTime;
		if (m_timer >= 2.0f)
		{
			++m_state;
		}
		
			float dx = (rand() % 3 - 1) * 0.5f;
			float dz = (rand() % 3 - 1) * 0.5f;

			for (auto& data : m_instanceData)
			{
				data.position.x += dx;
				data.CreateWorld();
			}
		}
		break;
	case 1:
		for (auto& data : m_instanceData)
		{
			data.position.y -= 2.0f;
			data.CreateWorld();
		}
		if (m_instanceData.back().position.y < -24.0f)
		{
			m_instanceData.clear();
			return true;
		}
		break;
	}

	return false;

}

void Stage::Render(ID3D11DeviceContext* immediateContext)
{
	m_model[0]->Render(Framework::GetContext(), m_world,VECTOR4F(1.0f, 1.0f, 1.0f, 1.0f));

#ifdef _DEBUG

#else
	if (!m_instanceData.empty())
	{
		m_model[1]->Render(Framework::GetContext(), m_instanceData, VECTOR4F(1.0f, 1.0f, 1.0f, 1.0f));

}
#endif
}

int Stage::RayPick(const VECTOR3F& startPosition, const VECTOR3F& endPosition,
	VECTOR3F* outPosition, VECTOR3F* outNormal)
{
	Collision coll;
	
	VECTOR2F target = { startPosition.x,startPosition.z };
	if (!coll.JudgeCircleAndpoint(m_circle, target)) return -1;

	int matIndex = -1;
	// オブジェクト空間でのレイに変換 
	FLOAT4X4 world = m_instanceData.begin()->world * m_model[0]->_resource->axisSystemTransform;
	DirectX::XMMATRIX worldtransform = DirectX::XMLoadFloat4x4(&world);

	//逆行列化
	DirectX::XMMATRIX inverseTransform = DirectX::XMMatrixInverse(nullptr, worldtransform);

	DirectX::XMVECTOR worldstart = DirectX::XMLoadFloat3(&startPosition);
	DirectX::XMVECTOR worldend = DirectX::XMLoadFloat3(&endPosition);
	DirectX::XMVECTOR localStart = DirectX::XMVector3TransformCoord(worldstart, inverseTransform);
	DirectX::XMVECTOR localEnd = DirectX::XMVector3TransformCoord(worldend, inverseTransform);

	//localStart = DirectX::XMVector3TransformCoord(localStart, CovertTransform);
	//localEnd = DirectX::XMVector3TransformCoord(localEnd, CovertTransform);

	// レイピック 
	float outdistance;
	VECTOR3F start, end;
	VECTOR3F hitPosition, hitNormal;
	DirectX::XMStoreFloat3(&start, localStart);
	DirectX::XMStoreFloat3(&end, localEnd);

	int ret = m_model[0]->RayPick(start, end, &hitPosition, &hitNormal, &outdistance, m_meshNames);
	if (ret != -1)
	{
		DirectX::XMVECTOR localposition = DirectX::XMLoadFloat3(&hitPosition);
		DirectX::XMVECTOR localnormal = DirectX::XMLoadFloat3(&hitNormal);
		DirectX::XMVECTOR worldposition = DirectX::XMVector3TransformCoord(localposition, worldtransform);
		//positionを抜きたいから関数を使ってる。
		DirectX::XMVECTOR worldnormal = DirectX::XMVector3TransformNormal(localnormal, worldtransform);

		DirectX::XMStoreFloat3(&hitPosition, worldposition);
		DirectX::XMStoreFloat3(&hitNormal, worldnormal);

		matIndex = ret;
		*outPosition = hitPosition;
		*outNormal = hitNormal;
	}
	return matIndex;
}

int Stage::MoveCheck(const VECTOR3F& startPosition, const VECTOR3F& endPosition, VECTOR3F* outPosition)
{
	return 0;
}

void Stage::ImGui()
{
#ifdef _DEBUG

	ImGui::Begin("Stage", nullptr, ImGuiWindowFlags_MenuBar);//メニューバーをつかうならこのBEGIN

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))//ファイルの中には
		{

			if (ImGui::MenuItem("Save"))//データの保存とか
			{
				std::ofstream ofs;
				ofs.open((std::string("../Asset/Binary/Stage/fence.bin")).c_str(), std::ios::binary);
				cereal::BinaryOutputArchive o_archive(ofs);
				o_archive(*this);
			}

			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	if (!m_instanceData.empty())
	{
		int count = static_cast<int>(m_instanceData.size());
		static int select = 0;
		ImGui::SliderInt("Select", &select,0, count-1);
		auto& data = m_instanceData[select];
		float position[] = { data.position.x,data.position.y,data.position.z };
		float scale[] = { data.scale.x,data.scale.y,data.scale.z };
		float angle[] = { data.angle.x,data.angle.y,data.angle.z };
		ImGui::SliderFloat3("Position", position, -100.0f, 100.0f);
		ImGui::SliderFloat3("Angle", angle,0.0f, 180 * 0.01745f);
		ImGui::SliderFloat3("Scale", scale, 0.0f, 50.0f);
		data.position = { position[0],position[1],position[2] };
		data.scale = { scale[0],scale[1],scale[2] };
		data.angle = { angle[0],angle[1],angle[2] };
		data.CreateWorld();

		if(ImGui::Button("Add"))
			AddInstanceData(m_position, m_angle, m_scale, VECTOR4F(1.0f, 1.0f, 1.0f, 1.0f));

	}


	ImGui::End();

#endif


}

void Stage::AddInstanceData(const VECTOR3F& position, const VECTOR3F& angle, const VECTOR3F& scale, const VECTOR4F& color)
{
	Source::InstanceData::InstanceData data;
	data.position = position;
	data.angle = angle;
	data.scale = scale;
	data.color = color;
	data.CreateWorld();

	m_instanceData.emplace_back(data);
}
