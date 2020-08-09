#include "SaberCharacter.h"
#include ".\LibrarySource\ModelData.h"
#include ".\LibrarySource\VectorCombo.h"

#ifdef _DEBUG
#include "..\External_libraries\imgui\imgui.h"
#include "..\External_libraries\imgui\imgui_impl_dx11.h"
#include "..\External_libraries\imgui\imgui_impl_win32.h"
#include "..\External_libraries\imgui\imgui_internal.h"
#endif

void Saber::Init()
{
	m_transformParm.translate = { 0.0f,0.0f,0.0f };
	m_transformParm.angle = { 0.0f * 0.01745f, 180.0f * 0.01745f,0.0f * 0.017454f };
	m_transformParm.scale = { 0.05f,0.05f,0.05f };
	m_transformParm.WorldUpdate();

	m_model = Source::ModelData::fbxLoader().GetActorModel(Source::ModelData::ActorModel::Saber);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Players/Saber/Walk.fbx", 60);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Players/Saber/Run.fbx",60);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Players/Saber/Slash.fbx", 60);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Players/Saber/Slash2.fbx", 60);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Players/Saber/Slash3.fbx", 60);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Players/Saber/Spell.fbx", 60);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Players/Saber/Impact.fbx", 60);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Players/Saber/Death.fbx", 60);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Players/Saber/Idle.fbx", 60);
	//Source::ModelData::fbxLoader().SaveActForBinary(Source::ModelData::ActorModel::Saber);

	m_blendAnimation.animationBlend.Init(m_model);
//	m_blendAnimation.partialBlend.Init(m_model);
}

void Saber::Update(float& elapsedTime)
{
    m_blendAnimation.animationBlend.Update(m_model,elapsedTime);
//	m_blendAnimation.partialBlend.Update(m_model, elapsedTime);
}

void Saber::Render(ID3D11DeviceContext* immediateContext)
{
	auto& localTransforms = m_blendAnimation.animationBlend._blendLocals;
//	auto& localTransforms = m_blendAnimation.partialBlend._blendLocals;
	VECTOR4F color{ 1.0f,1.0f,1.0f,1.0f };
	m_model->Render(immediateContext, m_transformParm.world, color, localTransforms);
//	m_debugObjects.debugObject.Render(immediateContext);
}

void Saber::ImGui(ID3D11Device* device)
{
#ifdef _DEBUG

	ImGui::Begin("SaberCharacter", nullptr, ImGuiWindowFlags_MenuBar);//メニューバーをつかうならこのBEGIN

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


#if 0
	auto BoneName = m_blendAnimation.partialBlend.GetBoneName().at(0);
	static int curringBone = 0;
	ImGui::Combo("Name_of_BoneName",
		&curringBone,
		vectorGetter,
		static_cast<void*>(&BoneName),
		static_cast<int>(BoneName.size())
	);

	std::vector<std::string> nodes;
	for (auto& n : m_blendAnimation.partialBlend.GetNodes())
	{
		nodes.push_back(n.name);
	}
	static int curringNode = 0;
	auto nodeName = nodes;
	ImGui::Combo("Name_of_NodeName",
		&curringNode,
		vectorGetter,
		static_cast<void*>(&nodeName),
		static_cast<int>(nodeName.size())
		);


	{
		FLOAT4X4 blendBone = m_blendAnimation.partialBlend._blendLocals[0].at(curringBone);
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
			geomtry.position = boneTranslate;
			geomtry.CreateWorld();
		}
	}

	{
		auto& lowerBodyBone = m_blendAnimation.partialBlend.GetPartialBoens().at(0);
		auto& upBodyBone = m_blendAnimation.partialBlend.GetPartialBoens().at(1);

		static float raito = 1.0f;
		ImGui::SliderFloat("BlendRatio", &raito, 0.0f, 1.0f);

		float& lowerWeight = lowerBodyBone.weight;
		float& upWeight = upBodyBone.weight;

		lowerWeight = 1.0f - raito;
		upWeight = raito;

		m_blendAnimation.partialBlend.SetupHalfBody(m_model);

		if (ImGui::Button("UpperBodyRoot?"))
		{
			m_blendAnimation.partialBlend._upperBodyRoot = curringNode;
			m_blendAnimation.partialBlend.SetupHalfBody(m_model);

		}



	}

#else
	static float raito = 1.0f;
	ImGui::SliderFloat("BlendRatio", &raito, 0.0f, 1.0f);
	m_blendAnimation.animationBlend._blendRatio = raito;

	auto animCount = m_model->_resource->_animationTakes.size();

	static int currentAnim = 0;
	if (ImGui::ArrowButton("Front", ImGuiDir_Left))
	{
		if (0 >= currentAnim)
			currentAnim = 0;
		else
			--currentAnim;
	}
	ImGui::SameLine();
	if (ImGui::ArrowButton("Next", ImGuiDir_Right))
	{
		if (currentAnim >= static_cast<int>(animCount))
			currentAnim = static_cast<int>(animCount);
		else
			++currentAnim;
	}

	if (ImGui::Button(u8"ChangeAnim"))
	{
		m_blendAnimation.animationBlend.ChangeSampler(0,currentAnim,m_model);
	}

	if (ImGui::Button(u8"AddAnim"))
	{
		m_blendAnimation.animationBlend.AddSampler(currentAnim, m_model);
	}

	if (ImGui::Button(u8"DeleateAnim"))
	{
		m_blendAnimation.animationBlend.ReleaseSampler(currentAnim);
	}
#endif
	ImGui::End();

#endif
}
