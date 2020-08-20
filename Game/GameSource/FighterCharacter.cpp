#include "FighterCharacter.h"
#include ".\LibrarySource\ModelData.h"
#include ".\LibrarySource\VectorCombo.h"

#ifdef _DEBUG
#include "..\External_libraries\imgui\imgui.h"
#include "..\External_libraries\imgui\imgui_impl_dx11.h"
#include "..\External_libraries\imgui\imgui_impl_win32.h"
#include "..\External_libraries\imgui\imgui_internal.h"
#endif

CEREAL_CLASS_VERSION(CharacterParameter::Status, 1);
CEREAL_CLASS_VERSION(CharacterParameter::Step, 1);
CEREAL_CLASS_VERSION(CharacterParameter::Camera, 1);
CEREAL_CLASS_VERSION(Fighter, 1);


void Fighter::Init()
{
	m_transformParm.position = { -10.0f,0.0f,0.0f };
	m_transformParm.angle = { 0.0f * 0.01745f, 0.0f * 0.01745f,0.0f * 0.017454f };
	m_transformParm.scale = { 0.005f,0.005f,0.005f };
	m_transformParm.WorldUpdate();

	m_model = Source::ModelData::fbxLoader().GetActorModel(Source::ModelData::ActorModel::Fighter);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Players/Fighter/Walk.fbx", 60);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Players/Fighter/Run.fbx", 60);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Players/Fighter/Dive.fbx", 60);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Players/Fighter/Hook.fbx", 60);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Players/Fighter/Punching.fbx", 60);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Players/Fighter/Uppercut.fbx", 60);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Players/Fighter/Kicking.fbx", 60);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Players/Fighter/Impact.fbx", 60);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Players/Fighter/Death.fbx", 60);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Players/Fighter/StandingIdle.fbx", 60);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Players/Fighter/ArmStretching.fbx", 60);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Players/Fighter/FightIdle.fbx", 60);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Players/Fighter/MagicHeal.fbx", 60);
	//Source::ModelData::fbxLoader().SaveActForBinary(Source::ModelData::ActorModel::Fighter);

	m_blendAnimation.animationBlend.Init(m_model);
	m_blendAnimation.animationBlend.ChangeSampler(0, 1, m_model);
	m_blendAnimation.animationBlend.AddSampler(2, m_model);
	m_blendAnimation.animationBlend.AddSampler(3, m_model);
	//m_blendAnimation.partialBlend.Init(m_model);

	m_padDeadLine = 3760.0f;
	m_changeComand.isPlay = true;

	if (PathFileExistsA((std::string("../Asset/Binary/Player/Fighter/Parameter") + ".bin").c_str()))
	{
		std::ifstream ifs;
		ifs.open((std::string("../Asset/Binary/Player/Fighter/Parameter") + ".bin").c_str(), std::ios::binary);
		cereal::BinaryInputArchive i_archive(ifs);
		i_archive(*this);
	}

}

void Fighter::Update(float& elapsedTime)
{
	if (m_changeComand.isPlay)
	{
		m_input = PAD.GetPad(0);

		Move(elapsedTime);

		Step(elapsedTime);


		//switch (m_animationType)
		//{
		//case Fighter::IDLE:
		//	if (m_input->StickDeadzoneLX(10000) || m_input->StickDeadzoneLY(10000))
		//	{
		//		m_animationType = Fighter::MOVE;
		//	}
		//	break;
		//case Fighter::MOVE:
		//	if (!m_input->StickDeadzoneLX(10000) || !m_input->StickDeadzoneLY(1000))
		//	{
		//		m_animationType = Fighter::IDLE;
		//		m_blendAnimation.animationBlend._blendRatio = 0.0f;
		//	}
		//	else
		//		Move(elapsedTime);
		//	break;
		//case Fighter::DIVE:
		//	break;
		//case Fighter::ATTACK:
		//	break;
		//case Fighter::IMPACT:
		//	break;
		//case Fighter::DEATH:
		//	break;
		//case Fighter::IDLE2:
		//	break;
		//case Fighter::ARMSET:
		//	break;
		//case Fighter::FIGHTIDLE:
		//	break;
		//case Fighter::HEAL:
		//	break;
		//default:
		//	break;
		//}
	}
	else
	{
		if (m_state)
			m_state->Execute(this);
	}
	m_blendAnimation.animationBlend.Update(m_model, elapsedTime);

}

void Fighter::Render(ID3D11DeviceContext* immediateContext)
{
	auto& localTransforms = m_blendAnimation.animationBlend._blendLocals;
	//auto& localTransforms = m_blendAnimation.partialBlend._blendLocals;
	VECTOR4F color{ 1.0f,1.0f,1.0f,1.0f };
	m_model->Render(immediateContext, m_transformParm.world, color, localTransforms);
	m_debugObjects.debugObject.Render(immediateContext);
}


void Fighter::Move(float& elapsedTime)
{
	if (m_input->StickDeadzoneLX(m_padDeadLine) || m_input->StickDeadzoneLY(m_padDeadLine))
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
		VECTOR3F stickVec(-m_input->StickVectorLeft().x, 0.0f, -m_input->StickVectorLeft().y);
		DirectX::XMVECTOR vStickVex = DirectX::XMLoadFloat3(&stickVec);

		vStickVex = DirectX::XMVector4Transform(vStickVex, viewMatrix);
		DirectX::XMStoreFloat3(&m_status.velocity, vStickVex);

		VECTOR3F position = m_transformParm.position;
		VECTOR3F angle = m_transformParm.angle;

		VECTOR3F frontVec(sinf(angle.y), 0.0f, cosf(angle.y));


		DirectX::XMVECTOR vFront = DirectX::XMLoadFloat3(&frontVec);
		DirectX::XMVECTOR vCross = DirectX::XMVector3Cross(vFront, vStickVex);

		VECTOR4F crossF;
		DirectX::XMStoreFloat4(&crossF, vCross);
		float dot = frontVec.x * m_status.velocity.x +
			frontVec.y * m_status.velocity.y +
			frontVec.z * m_status.velocity.z;


		float dangle = 1 - dot;

		if (dangle >= DirectX::XMConvertToRadians(1))
		{
			dangle = DirectX::XMConvertToRadians(1);
		}


		if (1 - ::abs(dot) > DirectX::XMConvertToRadians(1))
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
			dangle = DirectX::XMConvertToRadians(1);
			if (crossF.y < 0.0f)
			{

				angle.y += dangle;
			}
			else
			{

				angle.y -= dangle;

			}
		}


		if (!m_stepParm.isStep)
		{
			float stickLxValue = m_input->GetStickLeftXValue();
			float stickLyValue = m_input->GetStickLeftYValue();

			float ratio = sqrtf(stickLxValue * stickLxValue + stickLyValue * stickLyValue) / 32767;
			if (ratio >= 1.0f) ratio = 1.0f;
			m_blendAnimation.animationBlend._blendRatio = ratio;

			m_transformParm.angle = angle;

			m_status.velocity.x = sinf(angle.y) * (m_status.speed.x * ratio);
			m_status.velocity.y = 0.0f;
			m_status.velocity.z = cosf(angle.y) * (m_status.speed.z * ratio);

			position += m_status.velocity * elapsedTime;

			m_transformParm.position = position;

			m_transformParm.WorldUpdate();
		}
	}
	else
		m_blendAnimation.animationBlend._blendRatio = 0.0f;

}

void Fighter::Step(float& elapsedTime)
{
	if (m_input->GetButtons(XINPUT_GAMEPAD_BUTTONS::PAD_A) == 1 || m_stepParm.isStep)
	{
		if (!m_stepParm.isStep)
		{
			m_blendAnimation.animationBlend.ChangeSampler(0, Fighter::Animation::DIVE, m_model);
			m_blendAnimation.animationBlend.FalseAnimationLoop(0);
			m_blendAnimation.animationBlend._blendRatio = 0.0f;
			m_stepParm.isStep = true;
		}
		m_stepParm.speed -= m_stepParm.accel;
		if (m_stepParm.speed.x <= 0)
			m_stepParm.speed = { 0.0f,0.0f,0.0f };

		m_status.velocity.x = sinf(m_transformParm.angle.y) * (m_stepParm.speed.x);
		m_status.velocity.y = 0.0f;
		m_status.velocity.z = cosf(m_transformParm.angle.y) * (m_stepParm.speed.z);

		m_transformParm.position += m_status.velocity * elapsedTime;

		m_transformParm.WorldUpdate();

		if (m_blendAnimation.animtionTime == m_blendAnimation.animationBlend.GetAnimationTime(0))
		{
			//m_stepParm.speed = {};
			m_stepParm.isStep = false;
			m_blendAnimation.animationBlend.ResetAnimationSampler(0);
			m_blendAnimation.animationBlend.ChangeSampler(0, Fighter::Animation::IDLE, m_model);
			m_blendAnimation.animtionTime = -1.0f;
			return;
		}

		m_blendAnimation.animtionTime = m_blendAnimation.animationBlend.GetAnimationTime(0);
	}

}

void Fighter::ImGui(ID3D11Device* device)
{
#ifdef _DEBUG

	ImGui::Begin("FighterCharacter", nullptr, ImGuiWindowFlags_MenuBar);//メニューバーをつかうならこのBEGIN

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))//ファイルの中には
		{
			if (ImGui::MenuItem("Save"))//データの保存とか
			{
				std::ofstream ofs;
				ofs.open((std::string("../Asset/Binary/Player/Fighter/Parameter") + ".bin").c_str(), std::ios::binary);
				cereal::BinaryOutputArchive o_archive(ofs);
				o_archive(*this);
			}

			if (ImGui::MenuItem("Load"))
			{
				if (PathFileExistsA((std::string("../Asset/Binary/Player/Fighter/Parameter") + ".bin").c_str()))
				{
					std::ifstream ifs;
					ifs.open((std::string("../Asset/Binary/Player/Fighter/Parameter") + ".bin").c_str(), std::ios::binary);
					cereal::BinaryInputArchive i_archive(ifs);
					i_archive(*this);
				}
			}
			ImGui::EndMenu();

		}
		ImGui::EndMenuBar();
	}

	static int currentMesh = 0;
	ImGui::BulletText(u8"Mesh%d番目", currentMesh);
	//ImGui::SameLine();
	//if (ImGui::ArrowButton("Front", ImGuiDir_Left))
	//{
	//	if (0 >= currentMesh)
	//		currentMesh = 0;
	//	else
	//		--currentMesh;
	//}
	//ImGui::SameLine();
	//if (ImGui::ArrowButton("Next", ImGuiDir_Right))
	//{
	//	if (currentMesh >= static_cast<int>(m_blendAnimation.animationBlend.GetBoneName().size()) - 1)
	//		currentMesh = static_cast<int>(m_blendAnimation.animationBlend.GetBoneName().size()) - 1;
	//	else
	//		++currentMesh;
	//}

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
			geomtry.position = boneTranslate;

			geomtry.CreateWorld();
		}
	}

	//**************************************
	// Animation
	//**************************************
	if (ImGui::CollapsingHeader("Animation"))
	{
		ImGui::SliderFloat("BlendRatio", &m_blendAnimation.animationBlend._blendRatio, 0.0f, 1.0f);


		auto animCount = m_model->_resource->_animationTakes.size();

		static int currentAnim = 0;
		ImGui::BulletText("%d", currentAnim);
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

		//if (ImGui::Button(u8"DeleateAnim"))
		//{
		//	m_blendAnimation.animationBlend.ReleaseSampler(currentAnim);
		//}
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

	//**************************************
	// Status
	//**************************************
	if (ImGui::CollapsingHeader("Status"))
	{

		static float  speed = m_status.speed.x;
		ImGui::SliderFloat("Speed", &speed, 0.0f, 50.0f);

		m_status.speed.x = speed;
		m_status.speed.z = speed;

	}

	//**************************************
	// Position
	//**************************************
	if (ImGui::CollapsingHeader("Position"))
	{

		static float position[] = { m_transformParm.position.x,m_transformParm.position.y,m_transformParm.position.z };
		ImGui::SliderFloat3("Position", position, -2000.0f, 2000.0f);


	}

	//**************************************
	// Step
	//**************************************
	if (ImGui::CollapsingHeader("Step"))
	{
		float accel = m_stepParm.accel.x;
		ImGui::SliderFloat("StepAccel", &accel, 0.0f, 1.0f);

		float speed = m_stepParm.speed.x;
		ImGui::SliderFloat("StepSpeed", &speed, 0.0f, 100.0f);

		float animationSpeed = m_blendAnimation.animtionSpeed;
		ImGui::SliderFloat("AnimationSpeed", &animationSpeed, 1.0f, 5.0f);


		m_stepParm.accel = VECTOR3F(accel, 0.0f, accel);
		m_stepParm.speed = VECTOR3F(speed, 0.0f, speed);
		m_blendAnimation.animtionSpeed = animationSpeed;
	}
	//**************************************
	// Controller
	//**************************************
	if (ImGui::CollapsingHeader("Controller"))
	{
		VECTOR2F inputValue = m_input->StickVectorLeft();

		ImGui::SliderFloat("DeadLine", &m_padDeadLine, 0.0f, 30000.0f);

		auto lx = m_input->GetStickLeftXValue();
		auto ly = m_input->GetStickLeftYValue();

		float len = sqrtf(lx * lx + ly * ly) / 32767;

		ImGui::SliderFloat("len", &len, -40000.0f, 40000.0f);

		ImGui::SliderFloat("LX", &lx, -40000.0f, 40000.0f);
		ImGui::SliderFloat("LY", &ly, -40000.0f, 40000.0f);

		ImGui::SliderFloat("inputValueX", &inputValue.x
			, -40000.0f, 40000.0f);
		ImGui::SliderFloat("inputValueY", &inputValue.y
			, -40000.0f, 40000.0f);
		auto f = ToDistVec2(inputValue);
		ImGui::SliderFloat("Dist", &f, -40000.0f, 40000.0f);
	}



	ImGui::End();
#endif
}



