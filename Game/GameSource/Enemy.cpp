#include "Enemy.h"
#include "MessengTo.h"
#include ".\LibrarySource\ModelData.h"
#include ".\LibrarySource\VectorCombo.h"

void Enemy::Init()
{
	m_transformParm.position = { -10.0f,0.0f,80.0f };
	m_transformParm.angle = { 0.0f * 0.01745f, 180.0f * 0.01745f,0.0f * 0.017454f };
	m_transformParm.scale = { 1.25f,1.25f,1.25f };
	m_transformParm.WorldUpdate();

	m_model = Source::ModelData::fbxLoader().GetActorModel(Source::ModelData::ActorModel::ENEMY);
	//Source::ModelData::fbxLoader().SaveActForBinary(Source::ModelData::ActorModel::ENEMY);

	m_blendAnimation.animationBlend.Init(m_model);
}

void Enemy::Update(float& elapsedTime)
{
	m_blendAnimation.animationBlend.Update(m_model, elapsedTime);

}

void Enemy::Render(ID3D11DeviceContext* immediateContext)
{
	auto& localTransforms = m_blendAnimation.animationBlend._blendLocals;
	//	auto& localTransforms = m_blendAnimation.partialBlend._blendLocals;
	VECTOR4F color{ 1.0f,1.0f,1.0f,1.0f };
	m_model->Render(immediateContext, m_transformParm.world, color, localTransforms);
	m_debugObjects.debugObject.Render(immediateContext);
}

void Enemy::ImGui(ID3D11Device* device)
{
#ifdef _DEBUG

	ImGui::Begin("Enemy", nullptr, ImGuiWindowFlags_MenuBar);//メニューバーをつかうならこのBEGIN

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

	ImGui::Text("askAttackFlg %d", m_message.askAttack);

	if (ImGui::Button("ASK_ATTACK"))
	{
		MESSENGER.MessageFromEnemy(m_id, MessengType::ASK_ATTACK);
	}


	static int currentMesh = 0;
	ImGui::BulletText(u8"Mesh%d番目", currentMesh);
	ImGui::SameLine();
	if (ImGui::ArrowButton("Front", ImGuiDir_Left))
	{
		if (0 >= currentMesh)
			currentMesh = 0;
		else
			--currentMesh;
	}
	ImGui::SameLine();
	if (ImGui::ArrowButton("Next", ImGuiDir_Right))
	{
		if (currentMesh >= static_cast<int>(m_blendAnimation.animationBlend.GetBoneName().size()) - 1)
			currentMesh = static_cast<int>(m_blendAnimation.animationBlend.GetBoneName().size()) - 1;
		else
			++currentMesh;
	}

	static int currentBone = 0;

	ImGui::Combo("Name_of_BoneName",
		&currentBone,
		vectorGetter,
		static_cast<void*>(&m_blendAnimation.animationBlend.GetBoneName()[currentMesh]),
		static_cast<int>(m_blendAnimation.animationBlend.GetBoneName()[currentMesh].size())
	);

	//**************************************
	// DebugCollijion
	//**************************************
	if (ImGui::CollapsingHeader("DebugCollijion"))
	{

		FLOAT4X4 blendBone = m_blendAnimation.animationBlend._blendLocals[currentMesh].at(currentBone);
		FLOAT4X4 modelAxisTransform = m_model->_resource->axisSystemTransform;
		FLOAT4X4 getBone = blendBone * m_transformParm.world * modelAxisTransform;


		float boneTranslates[] = { getBone._41,getBone._42,getBone._43 };
		VECTOR3F boneTranslate = { boneTranslates[0],boneTranslates[1],boneTranslates[2] };

		ImGui::SliderFloat3("BoneTranslate", boneTranslates, -1.0f, 1.0f);

		static bool isVisualization = false;
		if (ImGui::Button("Make visible?"))
			isVisualization = true;

		if (isVisualization)
		{
			if (!m_debugObjects.debugObject.IsGeomety())
			{
				auto primitive = m_debugObjects.GetCube(device);
				m_debugObjects.debugObject.AddGeometricPrimitive(std::move(primitive));
			}
			m_debugObjects.debugObject.AddInstanceData(boneTranslate, VECTOR3F(0.0f * 0.01745f, 180.0f * 0.01745f, 0.0f * 0.017454f),
				VECTOR3F(1.0f, 1.0f, 1.0f), VECTOR4F(1.0f, 1.0f, 1.0f, 1.0f));

			isVisualization = false;
		}

		if (m_debugObjects.debugObject.IsGeomety())
		{
			auto& geomtry = m_debugObjects.debugObject.GetInstanceData(0);


			VECTOR3F front = { getBone._31,getBone._32,getBone._33 };

			NormalizeVec3(front);
			front.y = 0.0f;

			static float frontVector = 0.0f;
			ImGui::InputFloat("Front", &frontVector, 1.0f, 1.0f);


			geomtry.position = boneTranslate + front * frontVector;

			geomtry.CreateWorld();
		}
	}


	float position[] = { m_transformParm.position.x,m_transformParm.position.y,m_transformParm.position.z };
	ImGui::SliderFloat3("ModelPosiiton", position, -100.0f, 100.0f);

	m_transformParm.position.x = position[0];
	m_transformParm.position.y = position[1];
	m_transformParm.position.z = position[2];

	m_transformParm.WorldUpdate();

	ImGui::End();

#endif
}
