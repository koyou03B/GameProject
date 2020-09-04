#include "ArcherCharacter.h"
#include ".\LibrarySource\ModelData.h"
#include ".\LibrarySource\VectorCombo.h"

#ifdef _DEBUG
#include "..\External_libraries\imgui\imgui.h"
#include "..\External_libraries\imgui\imgui_impl_dx11.h"
#include "..\External_libraries\imgui\imgui_impl_win32.h"
#include "..\External_libraries\imgui\imgui_internal.h"
#endif

void Archer::Init()
{
	m_transformParm.position = { 30.0f,0.0f,0.0f };
	m_transformParm.angle = { 0.0f * 0.01745f, 0.0f * 0.01745f,0.0f * 0.017454f };
	m_transformParm.scale = { 0.05f,0.05f,0.05f };
	m_transformParm.WorldUpdate();

	m_model = Source::ModelData::fbxLoader().GetActorModel(Source::ModelData::ActorModel::Archer);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Players/Archer/Walk.fbx", 60);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Players/Archer/Run.fbx",60);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Players/Archer/Dive.fbx", 60);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Players/Archer/DrawArrow.fbx", 60);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Players/Archer/Idle.fbx", 60);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Players/Archer/IdleWalk.fbx", 60);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Players/Archer/AimRecoil.fbx", 60);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Players/Archer/Impact.fbx", 60);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Players/Archer/Death.fbx", 60);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Players/Archer/Spell.fbx", 60);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Players/Archer/AimWalk.fbx", 60);
	//Source::ModelData::fbxLoader().SaveActForBinary(Source::ModelData::ActorModel::Archer);
	m_changeParm.isPlay = true;
	m_blendAnimation.animationBlend.Init(m_model);
	//m_blendAnimation.partialBlend.Init(m_model);


	if (PathFileExistsA((std::string("../Asset/Binary/Player/Archer/Parameter") + ".bin").c_str()))
	{
		std::ifstream ifs;
		ifs.open((std::string("../Asset/Binary/Player/Archer/Parameter") + ".bin").c_str(), std::ios::binary);
		cereal::BinaryInputArchive i_archive(ifs);
		i_archive(*this);
	}


}

void Archer::Update(float& elapsedTime)
{
	m_blendAnimation.animationBlend.Update(m_model, elapsedTime);

	//if (m_changeParm.isPlay)
	//{
	//	auto input = PAD.GetPad(0);

	//	if (input->StickDeadzoneLX(10000) || input->StickDeadzoneLY(10000))
	//	{
	//		FLOAT4X4 view = Source::CameraControlle::CameraManager().GetInstance()->GetView();
	//		view._14 = 0.0f;
	//		view._24 = 0.0f;
	//		view._34 = 0.0f;
	//		view._41 = 0.0f;
	//		view._42 = 0.0f;
	//		view._43 = 0.0f;
	//		view._44 = 1.0f;

	//		DirectX::XMMATRIX viewMatrix = DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(&view));
	//		VECTOR3F stickVec(-input->StickVectorLeft().x, 0.0f, -input->StickVectorLeft().y);
	//		DirectX::XMVECTOR vStickVex = DirectX::XMLoadFloat3(&stickVec);

	//		vStickVex = DirectX::XMVector4Transform(vStickVex, viewMatrix);
	//		DirectX::XMStoreFloat3(&m_moveParm.velocity, vStickVex);

	//		VECTOR3F position = m_transformParm.position;
	//		VECTOR3F angle = m_transformParm.angle;

	//		VECTOR3F frontVec(sinf(angle.y), 0.0f, cosf(angle.y));


	//		DirectX::XMVECTOR vFront = DirectX::XMLoadFloat3(&frontVec);
	//		DirectX::XMVECTOR vCross = DirectX::XMVector3Cross(vFront, vStickVex);

	//		VECTOR4F crossF;
	//		DirectX::XMStoreFloat4(&crossF, vCross);
	//		float dot = frontVec.x * m_moveParm.velocity.x +
	//			frontVec.y * m_moveParm.velocity.y +
	//			frontVec.z * m_moveParm.velocity.z;


	//		float dangle = 1 - dot;

	//		if (dangle >= DirectX::XMConvertToRadians(3))
	//		{
	//			dangle = DirectX::XMConvertToRadians(3);
	//		}


	//		if (1 - ::abs(dot) > DirectX::XMConvertToRadians(3))
	//		{
	//			if (crossF.y < 0.0f)
	//			{

	//				angle.y += dangle;
	//			}
	//			else
	//			{

	//				angle.y -= dangle;

	//			}
	//		}
	//		else if (dot > 0.0f)
	//		{
	//			dangle = DirectX::XMConvertToRadians(3);
	//			if (crossF.y < 0.0f)
	//			{

	//				angle.y += dangle;
	//			}
	//			else
	//			{

	//				angle.y -= dangle;

	//			}
	//		}

	//		m_transformParm.angle = angle;

	//		m_moveParm.velocity.x = sinf(angle.y) * m_moveParm.speed.x;
	//		m_moveParm.velocity.y = 0.0f;
	//		m_moveParm.velocity.z = cosf(angle.y) * m_moveParm.speed.z;

	//		position += m_moveParm.velocity * elapsedTime;

	//		m_transformParm.position = position;

	//		m_transformParm.WorldUpdate();
	//	}

	//}

}

void Archer::Render(ID3D11DeviceContext* immediateContext)
{
	auto& localTransforms = m_blendAnimation.animationBlend._blendLocals;
	//auto& localTransforms = m_blendAnimation.partialBlend._blendLocals;
	VECTOR4F color{ 1.0f,1.0f,1.0f,1.0f };
	m_model->Render(immediateContext, m_transformParm.world, color, localTransforms);
	VECTOR4F scroll{ 0.0f, 0.0f, 0.0f, 0.0f };
	m_debugObjects.debugObject.Render(immediateContext, scroll, true);
}

void Archer::ImGui(ID3D11Device* device)
{
#ifdef _DEBUG

	ImGui::Begin("ArcherCharacter", nullptr, ImGuiWindowFlags_MenuBar);//メニューバーをつかうならこのBEGIN

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))//ファイルの中には
		{
			if (ImGui::MenuItem("Save"))//データの保存とか
			{
				std::ofstream ofs;
				ofs.open((std::string("../Asset/Binary/Player/Archer/Parameter") + ".bin").c_str(), std::ios::binary);
				cereal::BinaryOutputArchive o_archive(ofs);
				o_archive(*this);
			}
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

		auto animCount = m_model->_resource->_animationTakes.size();

		static int currentAnim = 0;

		ImGui::Text("AnimNo %d", currentAnim);
		ImGui::SameLine();
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
			m_blendAnimation.partialBlend.ChangeSampler(0, currentAnim, m_model);
		}

		if (ImGui::Button(u8"AddAnim"))
		{
			m_blendAnimation.partialBlend.AddSampler(currentAnim, m_model);
		}

		if (ImGui::Button(u8"DeleateAnim"))
		{
			m_blendAnimation.partialBlend.ReleaseSampler(currentAnim);
		}

	}

#else
	{
		auto BoneName = m_blendAnimation.animationBlend.GetBoneName().at(2);
		static int curringBone = 0;
		ImGui::Combo("Name_of_BoneName",
			&curringBone,
			vectorGetter,
			static_cast<void*>(&BoneName),
			static_cast<int>(BoneName.size())
		);
		FLOAT4X4 blendBone = m_blendAnimation.animationBlend._blendLocals[2].at(curringBone);
		FLOAT4X4 modelAxisTransform = m_model->_resource->axisSystemTransform;
		FLOAT4X4 getBone = blendBone * modelAxisTransform * m_transformParm.world ;

		float boneTranslates[] = { getBone._41,getBone._42,getBone._43 };

		VECTOR3F boneTranslate = { boneTranslates[0],boneTranslates[1],boneTranslates[2] };
		VECTOR3F boneFront = { getBone._31,getBone._32,getBone._33 };
		ImGui::SliderFloat3("BoneTranslate", boneTranslates, -1.0f, 1.0f);

		static bool isVisualization = false;
		if (ImGui::Button("Make visible?"))
			isVisualization = true;

		if (isVisualization)
		{
			if (!m_debugObjects.debugObject.IsGeomety())
			{
				auto primitive = m_debugObjects.GetCube(device, "../Asset/Texture/n64.png");
				m_debugObjects.debugObject.AddGeometricPrimitive(std::move(primitive));
			}
			m_debugObjects.debugObject.AddInstanceData(boneTranslate, VECTOR3F(0.0f * 0.01745f, 180.0f * 0.01745f, 0.0f * 0.017454f),
				VECTOR3F(1.0f, 1.0f, 1.0f), VECTOR4F(1.0f, 1.0f, 1.0f, 1.0f));

			isVisualization = false;
		}

		if (m_debugObjects.debugObject.IsGeomety())
		{
			auto& geomtry = m_debugObjects.debugObject.GetInstanceData(0);
			boneFront = NormalizeVec3(boneFront);
			static float frontX = 1.0f, frontY = 1.0f, frontZ = 1.0f;
			ImGui::SliderFloat("frontX", &frontX, -100.0f, 100.0f);
			ImGui::SliderFloat("frontY", &frontY, -100.0f, 100.0f);
			ImGui::SliderFloat("frontZ", &frontZ, -100.0f, 100.0f);

			boneFront.x *= frontX;
			boneFront.y *= frontY;
			boneFront.z *= frontZ;


			geomtry.position = boneTranslate + boneFront;
			geomtry.CreateWorld();
		}
	}



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
		m_blendAnimation.animationBlend.ChangeSampler(0, currentAnim, m_model);
	}

	if (ImGui::Button(u8"AddAnim"))
	{
		m_blendAnimation.animationBlend.AddSampler(currentAnim, m_model);
	}

	if (ImGui::Button(u8"DeleateAnim"))
	{
		m_blendAnimation.animationBlend.ReleaseSampler(currentAnim);
	}

	//**************************************
	// Camera
	//**************************************
	if (ImGui::CollapsingHeader("Camera"))
	{
		static float length =  m_cameraParm.lenght.x;
		ImGui::SliderFloat("CameraLength", &length, -100, 100);

		ImGui::InputFloat("HeightAboveGround", &m_cameraParm.heightAboveGround, 0.5f, 0.5f);

		ImGui::SliderFloat("Value", &m_cameraParm.value, 0.0, 1.0f);
		static float right = 0.0f;
		ImGui::SliderFloat("Right", &right, 0.0, -100.0f);

		FLOAT4X4 modelAxisTransform = m_model->_resource->axisSystemTransform;
		FLOAT4X4 world =  m_transformParm.world * modelAxisTransform;

		VECTOR3F rightAxis = { world._11,world._12,world._13 };
		rightAxis = NormalizeVec3(rightAxis);
		if (rightAxis.x >= 0)
			rightAxis.x *= -1;
		if (rightAxis.z >= 0)
			rightAxis.z *= -1;

		m_cameraParm.lenght.x = length + rightAxis.x * right;
		m_cameraParm.lenght.y = 0.0f;
		m_cameraParm.lenght.z = length + rightAxis.z * right;

	}
	//**************************************
	// Status
	//**************************************
	if (ImGui::CollapsingHeader("Status"))
	{

		static float  speed = m_moveParm.speed.x;
		ImGui::SliderFloat("Speed", &speed, 0.0f, 50.0f);

		m_moveParm.speed.x = speed;
		m_moveParm.speed.z = speed;

	}

	//**************************************
	// Position
	//**************************************
	if (ImGui::CollapsingHeader("Position"))
	{

		static float position[] = { m_transformParm.position.x,m_transformParm.position.y,m_transformParm.position.z };
		ImGui::SliderFloat3("Position", position, -2000.0f, 2000.0f);


	}
#endif
	ImGui::End();

#endif
}
