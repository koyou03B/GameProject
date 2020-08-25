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
	m_transformParm.position = { 0.0f,0.0f,0.0f };
	m_transformParm.angle = { 0.0f * 0.01745f, 0.0f * 0.01745f,0.0f * 0.017454f };
	m_transformParm.scale = { 0.07f,0.07f,0.07f };
	m_transformParm.WorldUpdate();
	m_changeParm.isPlay = true;

	m_model = Source::ModelData::fbxLoader().GetActorModel(Source::ModelData::ActorModel::Saber);
//	Source::ModelData::fbxLoader().SaveActForBinary(Source::ModelData::ActorModel::Saber);

	m_blendAnimation.animationBlend.Init(m_model);


	if (PathFileExistsA((std::string("../Asset/Binary/Player/Saber/Parameter") + ".bin").c_str()))
	{
		std::ifstream ifs;
		ifs.open((std::string("../Asset/Binary/Player/Saber/Parameter") + ".bin").c_str(), std::ios::binary);
		cereal::BinaryInputArchive i_archive(ifs);
		i_archive(*this);
	}

}

void Saber::Update(float& elapsedTime)
{
    m_blendAnimation.animationBlend.Update(m_model,elapsedTime);

	if (m_changeParm.isPlay)
	{
		auto input = PAD.GetPad(0);

		if (input->StickDeadzoneLX(10000) || input->StickDeadzoneLY(10000))
		{
			FLOAT4X4 view = Source::CameraControlle::CameraManager().GetInstance()->GetView();
			view._14 = 0.0f;
			view._24 = 0.0f;
			view._34 = 0.0f;
			view._41 = 0.0f;
			view._42 = 0.0f;
			view._43 = 0.0f;
			view._44 = 1.0f;

			DirectX::XMMATRIX viewMatrix = DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(&view));
			VECTOR3F stickVec(-input->StickVectorLeft().x, 0.0f, -input->StickVectorLeft().y);
			DirectX::XMVECTOR vStickVex = DirectX::XMLoadFloat3(&stickVec);

			vStickVex = DirectX::XMVector4Transform(vStickVex, viewMatrix);
			DirectX::XMStoreFloat3(&m_moveParm.velocity, vStickVex);

			VECTOR3F position = m_transformParm.position;
			VECTOR3F angle = m_transformParm.angle;

			VECTOR3F frontVec(sinf(angle.y), 0.0f, cosf(angle.y));


			DirectX::XMVECTOR vFront = DirectX::XMLoadFloat3(&frontVec);
			DirectX::XMVECTOR vCross = DirectX::XMVector3Cross(vFront, vStickVex);
	
			VECTOR4F crossF;
			DirectX::XMStoreFloat4(&crossF, vCross);
			float dot = frontVec.x * m_moveParm.velocity.x +
				frontVec.y * m_moveParm.velocity.y +
				frontVec.z * m_moveParm.velocity.z;


			float dangle = 1 - dot;

			if (dangle >= DirectX::XMConvertToRadians(5))
			{
				dangle = DirectX::XMConvertToRadians(5);
			}


			if (1 - ::abs(dot) > DirectX::XMConvertToRadians(5))
			{
				if (crossF.y < 0.0f)
				{

					angle.y += dangle;
				}
				else
				{

					angle.y -= dangle;

				}
			}
			else if (dot > 0.0f)
			{
				dangle = DirectX::XMConvertToRadians(5);
				if (crossF.y < 0.0f)
				{

					angle.y += dangle;
				}
				else
				{

					angle.y -= dangle;

				}
			}

			m_transformParm.angle = angle;

			m_moveParm.velocity.x = sinf(angle.y) * m_moveParm.speed.x;
			m_moveParm.velocity.y = 0.0f;
			m_moveParm.velocity.z = cosf(angle.y) * m_moveParm.speed.z;

			position += m_moveParm.velocity * elapsedTime;

			m_transformParm.position = position;

			m_transformParm.WorldUpdate();
		}

	}
}

void Saber::Render(ID3D11DeviceContext* immediateContext)
{
	auto& localTransforms = m_blendAnimation.animationBlend._blendLocals;
	VECTOR4F color{ 1.0f,1.0f,1.0f,1.0f };
	m_model->Render(immediateContext, m_transformParm.world, color, localTransforms);
	m_debugObjects.debugObject.Render(immediateContext);
}

void Saber::ImGui(ID3D11Device* device)
{
#ifdef _DEBUG

	ImGui::Begin("SaberCharacter", nullptr, ImGuiWindowFlags_MenuBar);//メニューバーをつかうならこのBEGIN

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))//ファイルの中には
		{
			if (ImGui::MenuItem("Save"))//データの保存とか
			{
				std::ofstream ofs;
				ofs.open((std::string("../Asset/Binary/Player/Saber/Parameter") + ".bin").c_str(), std::ios::binary);
				cereal::BinaryOutputArchive o_archive(ofs);
				o_archive(*this);
			}

			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
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

			front = NormalizeVec3(front);
			front.y = 0.0f;

			static float frontVector = 0.0f;
			ImGui::InputFloat("Front", &frontVector, 1.0f, 1.0f);


			geomtry.position = boneTranslate + front * frontVector;
			
			geomtry.CreateWorld();
		}
	}


	//**************************************
	// Animation
	//**************************************
	if (ImGui::CollapsingHeader("Animation"))
	{
		static float raito = 1.0f;
		ImGui::SliderFloat("BlendRatio", &raito, 0.0f, 1.0f);
		m_blendAnimation.animationBlend._blendRatio = raito;

		auto animCount = m_model->_resource->_animationTakes.size();

		static int currentAnim = 0;
		ImGui::BulletText("%d", currentAnim); 
		ImGui::SameLine();
		if (ImGui::ArrowButton("AnimFront", ImGuiDir_Left))
		{
			if (0 >= currentAnim)
				currentAnim = 0;
			else
				--currentAnim;
		}
		ImGui::SameLine();
		if (ImGui::ArrowButton("AnimNext", ImGuiDir_Right))
		{
			if (currentAnim >= static_cast<int>(animCount) - 1)
				currentAnim = static_cast<int>(animCount) - 1;
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
	}

	//**************************************
	// Camera
	//**************************************
	if (ImGui::CollapsingHeader("Camera"))
	{
		static float length = m_cameraParm.lenght.x;
		ImGui::SliderFloat("CameraLength", &length, -100, 100);

		ImGui::InputFloat("HeightAboveGround", &m_cameraParm.heightAboveGround, 0.5f, 0.5f);

		ImGui::InputFloat("Value", &m_cameraParm.value, 0.0, 1.0f);
		ImGui::InputFloat("Right", &m_cameraParm.focalLength, 0.0, 100.0f);

		m_cameraParm.lenght.x = length;
		m_cameraParm.lenght.y = 0.0f;
		m_cameraParm.lenght.z = length;

	}

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

		float position[] = { m_transformParm.position.x,m_transformParm.position.y,m_transformParm.position.z };
		ImGui::SliderFloat3("Position", position, -2000.0f, 2000.0f);


	}
	ImGui::End();

#endif
}
