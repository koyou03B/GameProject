#include "Stage.h"
#include "MessengTo.h"

void Stage::Init()
{
	VECTOR3F translate = { 0.0f,0.0f,0.0f };
	VECTOR3F angle = { 0.0f * 0.01745f, 90.0f * 0.01745f,0.0f * 0.017454f };
	VECTOR3F scale = { 13.0f,13.0f,13.0f };
	
	AddInstanceData(translate, angle, scale, VECTOR4F(1.0f, 1.0f, 1.0f, 1.0f));
	m_model = Source::ModelData::fbxLoader().GetStaticModel(Source::ModelData::StaticModel::STAGE);
	//Source::ModelData::fbxLoader().SaveStaticForBinary(Source::ModelData::StaticModel::STAGE);
	//m_model->_shaderON.normalMap = true;

	m_circle.radius = 81.0f;
	m_circle.position = {};
	m_circle.scale = 1.0f;

	//m_meshNames.emplace_back("Arena1_PVPdoor");
	//m_meshNames.emplace_back("QU_gargoyle01_b (3)");
	//m_meshNames.emplace_back("QU_gargoyle01_b (4)");
	//m_meshNames.emplace_back("QU_gargoyle01_b (5)");
	//m_meshNames.emplace_back("QU_gargoyle01_b (6)");
}

void Stage::Update(float& elapsedTime)
{
	auto players = MESSENGER.CallPlayersInstance();
	auto enemys = MESSENGER.CallEnemysInstance();

	for (auto& player : players)
	{
		Collision coll;
		VECTOR3F pos = player->GetWorldTransform().position;
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
			player->GetWorldTransform().position.x -= distance.x;
			player->GetWorldTransform().position.z -= distance.y;
			player->GetWorldTransform().WorldUpdate();

		}
	}

	for (auto& enemy : enemys)
	{
		Collision coll;
		VECTOR3F pos = enemy->GetWorldTransform().position;
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
			enemy->GetWorldTransform().position.x -= distance.x;
			enemy->GetWorldTransform().position.z -= distance.y;
			enemy->GetWorldTransform().WorldUpdate();

		}
	}
}

void Stage::Render(ID3D11DeviceContext* immediateContext)
{
	if (!m_instanceData.empty())
	{
		m_model->Render(Framework::GetContext(), m_instanceData, VECTOR4F(1.0f, 1.0f, 1.0f, 1.0f));
	}
}

int Stage::RayPick(const VECTOR3F& startPosition, const VECTOR3F& endPosition,
	VECTOR3F* outPosition, VECTOR3F* outNormal)
{
	Collision coll;
	
	VECTOR2F target = { startPosition.x,startPosition.z };
	if (!coll.JudgeCircleAndpoint(m_circle, target)) return -1;

	int matIndex = -1;
	// オブジェクト空間でのレイに変換 
	FLOAT4X4 world = m_instanceData.begin()->world * m_model->_resource->axisSystemTransform;
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

	int ret = m_model->RayPick(start, end, &hitPosition, &hitNormal, &outdistance, m_meshNames);
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

			//if (ImGui::MenuItem("Save"))//データの保存とか
			//{
			//	std::ofstream ofs;
			//	ofs.open((std::string("./Data/Bin/player") + ".bin").c_str(), std::ios::binary);
			//	cereal::BinaryOutputArchive o_archive(ofs);
			//	o_archive(*this);
			//}

			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	if (!m_instanceData.empty())
	{
		auto& data = m_instanceData[0];
		float scale = data.scale.x;
		ImGui::SliderFloat("Scale", &scale, 0.001f, 100.0f);

		data.scale = VECTOR3F(scale, scale, scale);
		data.CreateWorld();

	}


	ImGui::End();

#endif


}
