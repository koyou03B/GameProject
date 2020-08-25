#include "FighterCharacter.h"
#include ".\LibrarySource\ModelData.h"
#include ".\LibrarySource\VectorCombo.h"

#ifdef _DEBUG
#include "..\External_libraries\imgui\imgui.h"
#include "..\External_libraries\imgui\imgui_impl_dx11.h"
#include "..\External_libraries\imgui\imgui_impl_win32.h"
#include "..\External_libraries\imgui\imgui_internal.h"
#endif

//CEREAL_CLASS_VERSION(CharacterParameter::Collision, 7);
CEREAL_CLASS_VERSION(CharacterParameter::BlendAnimation, 8);
CEREAL_CLASS_VERSION(CharacterParameter::Status, 8);
CEREAL_CLASS_VERSION(CharacterParameter::Move, 8);
CEREAL_CLASS_VERSION(CharacterParameter::Step, 8);
CEREAL_CLASS_VERSION(CharacterParameter::Attack, 8);
CEREAL_CLASS_VERSION(CharacterParameter::Camera, 8);
CEREAL_CLASS_VERSION(Fighter, 8);


void Fighter::Init()
{
	m_transformParm.position = { -10.0f,0.0f,0.0f };
	m_transformParm.angle = { 0.0f * 0.01745f, 0.0f * 0.01745f,0.0f * 0.017454f };
	m_transformParm.scale = { 0.005f,0.005f,0.005f };
	m_transformParm.WorldUpdate();

	m_attackParm.resize(5);
	m_collision.resize(5);
	m_model = Source::ModelData::fbxLoader().GetActorModel(Source::ModelData::ActorModel::Fighter);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Players/Fighter/Run.fbx", 60);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Players/Fighter/Running.fbx", 60);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Players/Fighter/Dive.fbx", 60);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Players/Fighter/RightPunching.fbx", 60);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Players/Fighter/LeftPunching.fbx", 60);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Players/Fighter/LastPunching.fbx", 60);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Players/Fighter/RightKick.fbx", 60);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Players/Fighter/LeftKicking.fbx", 60);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Players/Fighter/Impact.fbx", 60);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Players/Fighter/Death.fbx", 60);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Players/Fighter/StandingIdle.fbx", 60);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Players/Fighter/ArmStretching.fbx", 60);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Players/Fighter/FightIdle.fbx", 60);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Players/Fighter/MagicHeal.fbx", 60);
	//Source::ModelData::fbxLoader().SaveActForBinary(Source::ModelData::ActorModel::Fighter);

	m_blendAnimation.animationBlend.Init(m_model);
	m_blendAnimation.animationBlend.ChangeSampler(0, Fighter::Animation::IDLE, m_model);

	//m_blendAnimation.animationBlend.AddSampler(2, m_model);
	//m_blendAnimation.animationBlend.AddSampler(3, m_model);

	m_padDeadLine = 5000.0f;
	radian = 0.714f;
	m_changeParm.isPlay = true;

	if (PathFileExistsA((std::string("../Asset/Binary/Player/Fighter/Parameter") + ".bin").c_str()))
	{
		std::ifstream ifs;
		ifs.open((std::string("../Asset/Binary/Player/Fighter/Parameter") + ".bin").c_str(), std::ios::binary);
		cereal::BinaryInputArchive i_archive(ifs);
		i_archive(*this);
	}
	m_stepParm.maxSpeed = m_stepParm.speed;

}

void Fighter::Update(float& elapsedTime)
{
	m_elapsedTime = elapsedTime;
	if (m_changeParm.isPlay)
	{
		m_input = PAD.GetPad(0);

		Move(m_elapsedTime);

		Step(m_elapsedTime);

		Attack(m_elapsedTime);

		//関数化↓
		if (m_blendAnimation.animationBlend.GetSampler().size() == 2)
		{
			if (m_statusParm.isAttack || m_stepParm.isStep)
			{
				if (m_blendAnimation.animationBlend.GetSampler()[1].first == Animation::IDLE)
				{
					m_blendAnimation.animationBlend._blendRatio += m_blendAnimation.idleBlendRtio;
					if (m_blendAnimation.animationBlend._blendRatio >= 1.0f)
					{
						m_blendAnimation.animationBlend.ReleaseSampler(0);
						m_statusParm.isAttack = false;
						m_stepParm.isStep = false;
						m_blendAnimation.animationBlend._blendRatio = 0.0f;
						m_stepParm.speed = m_stepParm.maxSpeed;

					}
				}
			}
		}
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

//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
//IDLE WALK RUN : animtionBlend
//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
void Fighter::Move(float& elapsedTime)
{
	if (m_input->StickDeadzoneLX(m_padDeadLine) || m_input->StickDeadzoneLY(m_padDeadLine))
	{	
		if (!m_moveParm.isMove)
		{
			m_blendAnimation.animationBlend.ResetAnimationSampler(0);
			m_blendAnimation.animationBlend.AddSampler(Animation::WALK, m_model);
			m_blendAnimation.animationBlend.AddSampler(Animation::RUN, m_model);
			m_moveParm.isMove = true;
		}

		//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
		// Input direction of the PAD as 
		// seen from camera space
		//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
		FLOAT4X4 view = Source::CameraControlle::CameraManager().GetInstance()->GetView();
		view._14 = 0.0f;
		view._24 = 0.0f;
		view._34 = 0.0f;
		view._41 = 0.0f;
		view._42 = 0.0f;
		view._43 = 0.0f;
		view._44 = 1.0f;
		
		DirectX::XMMATRIX viewMatrix = DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(&view));
		VECTOR3F stickVec(m_input->StickVectorLeft().x, 0.0f, m_input->StickVectorLeft().y);
		DirectX::XMVECTOR vStickVex = DirectX::XMLoadFloat3(&stickVec);
		
		VECTOR3F stickVector;
		vStickVex = DirectX::XMVector4Transform(vStickVex, viewMatrix);
		DirectX::XMStoreFloat3(&stickVector, vStickVex);
	
		m_moveParm.velocity.x += stickVector.x * m_moveParm.accle.x;
		m_moveParm.velocity.y += stickVector.y * m_moveParm.accle.y;
		m_moveParm.velocity.z += stickVector.z * m_moveParm.accle.z;

		//*-*-*-*-*-*-*-*-
		// VectorLength
		//*-*-*-*-*-*-*-*-
		float speed = ToDistVec3(m_moveParm.velocity);

		float leftX = m_input->GetStickLeftXValue();
		float leftY = m_input->GetStickLeftYValue();
		float dean = sqrtf(leftX * leftX + leftY * leftY) / 32767;

		VECTOR3F vector = {};
		if (speed > m_moveParm.decleleration)
		{
			vector = m_moveParm.velocity / speed;
			m_blendAnimation.animationBlend._blendRatio += m_blendAnimation.moveBlendRatio;

			if (speed > m_moveParm.maxSpeed[0].x)
			{
				if (0 < dean && dean < 0.7f)
				{
					m_moveParm.velocity = vector * m_moveParm.maxSpeed[0].x;
					if (m_blendAnimation.animationBlend._blendRatio >= 0.7f)
					{
						m_blendAnimation.animationBlend._blendRatio = 0.7f;
					}
				}
				else
				{
					m_moveParm.velocity = vector * m_moveParm.maxSpeed[1].x;
				}
			}
			else
			{
				m_moveParm.velocity -= vector * m_moveParm.decleleration;
				if (m_blendAnimation.animationBlend._blendRatio >= 0.7)
				{
					m_blendAnimation.animationBlend._blendRatio -= m_blendAnimation.moveBlendRatio;
					if (m_blendAnimation.animationBlend._blendRatio >= 0.7f)
						m_blendAnimation.animationBlend._blendRatio = 0.7f;
				}
				
			}

			m_transformParm.position += m_moveParm.velocity * elapsedTime;
			if (m_blendAnimation.animationBlend._blendRatio >= 1.0f)
			{
				m_blendAnimation.animationBlend._blendRatio = 1.0f;
			}

			VECTOR3F angle = m_transformParm.angle;

			float dx = sinf(angle.y);
			float dz = cosf(angle.y);

			float dot = (vector.x * dx) + (vector.z * dz);
			float rot = 1 - dot;

			float limit = m_moveParm.turnSpeed;

			if (rot > limit)
			{
				rot = limit;
			}

			float cross = (vector.x * dz) - (vector.z * dx);
			if (cross > 0.0f)
			{
				angle.y += rot;
			}
			else
			{
				angle.y -= rot;
			}
			m_transformParm.angle = angle;

			m_transformParm.WorldUpdate();


		}
		else
		{
			m_moveParm.velocity = {};
		}
	}
	else
	{
		m_moveParm.speed = {};
		if (m_moveParm.isMove)
		{
			m_blendAnimation.animationBlend._blendRatio -= 0.1f;
			if (m_blendAnimation.animationBlend._blendRatio <= 0.0f)
			{
				m_blendAnimation.animationBlend._blendRatio = 0.0f;
				m_blendAnimation.animationBlend.ReleaseSampler(1);
				m_blendAnimation.animationBlend.ReleaseSampler(1);
				m_moveParm.isMove = false;
			}
		}
	}

}

//*-*-*-*-*-*-*-*-*-*-*-
//STEP : animation
//*-*-*-*-*-*-*-*-*-*-*-
void Fighter::Step(float& elapsedTime)
{
	if (m_input->GetButtons(XINPUT_GAMEPAD_BUTTONS::PAD_A) == 1 && !m_statusParm.isAttack || m_stepParm.isStep)
	{
		m_animationType = Fighter::Animation::DIVE;
		Stepping(elapsedTime);
	}

}

//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
//	RIGHTPUNCHING
//	LEFTPUNCHING
//	LAStTPUNCHING
//	RIGHTKICK
//	LEFTKICKING : animtion
//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-

void Fighter::Attack(float& elapsedTime)
{
	if (m_input->GetButtons(XINPUT_GAMEPAD_BUTTONS::PAD_X) == 1 && !m_statusParm.isAttack)
	{
		m_animationType = Fighter::Animation::RIGHTPUNCHING;
		m_blendAnimation.animationBlend.AddSampler(m_animationType, m_model);

	}


	switch (m_animationType)
	{
	case Fighter::RIGHTPUNCHING:
	{
		Animation nextaAnimtions[] = { Animation::LEFTPUNCHING,Animation::LEFTKICKING };
		Attacking(m_animationType, nextaAnimtions, m_attackParm[0], m_collision[0]);
	}
		break;
	case Fighter::LEFTPUNCHING:
	{
		Animation nextaAnimtions[] = { Animation::LASTTPUNCHING,Animation::RIGHTKICK };
		Attacking(m_animationType, nextaAnimtions, m_attackParm[1], m_collision[1]);
	}
		break;
	case Fighter::LASTTPUNCHING:
	{
		Animation nextaAnimtions[] = { Animation::LEFTPUNCHING,Animation::LEFTKICKING };
		Attacking(m_animationType, nextaAnimtions, m_attackParm[2], m_collision[2]);
	}
		break;
	case Fighter::RIGHTKICK:
	{
		Animation nextaAnimtions[] = { Animation::RIGHTPUNCHING,Animation::RIGHTPUNCHING };
		Attacking(m_animationType, nextaAnimtions, m_attackParm[3], m_collision[3]);
	}
		break;
	case Fighter::LEFTKICKING:
	{
		Animation nextaAnimtions[] = { Animation::LASTTPUNCHING,Animation::RIGHTKICK };
		Attacking(m_animationType, nextaAnimtions, m_attackParm[4], m_collision[4]);
	}
		break;
	default:
		break;
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
		int animationFrame = m_blendAnimation.animationBlend.GetAnimationTime(0) ;
		FLOAT4X4 blendBone = m_blendAnimation.animationBlend._blendLocals[currentMesh].at(currentBone);
		FLOAT4X4 modelAxisTransform = m_model->_resource->axisSystemTransform;
		FLOAT4X4 getBone = blendBone * modelAxisTransform * m_transformParm.world;

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
				auto primitive = m_debugObjects.GetSphere(device);
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
		{
			static float ratio = 0;
			ImGui::SliderFloat("BlendRatio", &ratio, 0.0f, 1.0f);
			if (ImGui::Button("ADD BlendRatio"))
				m_blendAnimation.animationBlend._blendRatio = ratio;

			static float attackRatio = 0;
			ImGui::SliderFloat("AttackBlendRatio", &attackRatio, 0.0f, 0.5f);
			if (ImGui::Button("Attack BlendRatio"))
				m_blendAnimation.attackBlendRtio = attackRatio;

			static float idleRatio = 0;
			ImGui::SliderFloat("IdleBlendRatio", &idleRatio, 0.0f, 1.0f);
			if (ImGui::Button("Idle BlendRatio"))
				m_blendAnimation.idleBlendRtio = idleRatio;

			static float moveRatio = 0;
			ImGui::SliderFloat("MoveBlendRatio", &moveRatio, 0.0f, 1.0f);
			if (ImGui::Button("Move BlendRatio"))
				m_blendAnimation.moveBlendRatio = moveRatio;
		}

		{
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


			{
				if (ImGui::Button(u8"ChangeAnim"))
				{
					m_blendAnimation.animationBlend.ChangeSampler(0, currentAnim, m_model);
				}

				if (ImGui::Button(u8"AddAnim"))
				{
					m_blendAnimation.animationBlend.AddSampler(currentAnim, m_model);
				}

				if (ImGui::Button(u8"UnLoop"))
				{
					m_blendAnimation.animationBlend.FalseAnimationLoop(0);
				}

				if (ImGui::Button(u8"Reset"))
				{
					m_blendAnimation.animationBlend.ResetAnimationSampler(0);
				}
				ImGui::BulletText("%d", m_blendAnimation.animationBlend.GetAnimationTime(0));
			}
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
	if (ImGui::CollapsingHeader("Move"))
	{
		{
			static float  accel0 = m_moveParm.accle.x;

			ImGui::SliderFloat("Accel0", &accel0, 0.0f, 10.0f);

			m_moveParm.accle = { accel0 ,0.0f,accel0 };
		}

		{
			static float  maxSpeed0 = m_moveParm.maxSpeed[0].x;
			static float  maxSpeed1 = m_moveParm.maxSpeed[1].x;
			ImGui::SliderFloat("MaxSpeed0", &maxSpeed0, 0.0f, 100.0f);
			ImGui::SliderFloat("MaxSpeed1", &maxSpeed1, 0.0f, 100.0f);

			m_moveParm.maxSpeed[0] = { maxSpeed0 ,0.0f,maxSpeed0 };
			m_moveParm.maxSpeed[1] = { maxSpeed1 ,0.0f,maxSpeed1 };
		}

		{
			static float decleleration = m_moveParm.decleleration;

			ImGui::SliderFloat("Decleleration", &decleleration, 0.0f, 1.0f);

			m_moveParm.decleleration = decleleration;
		}

		{
			static float turnSpeed = m_moveParm.turnSpeed;

			ImGui::SliderFloat("TurnSpeed", &turnSpeed, 0.0f, 1.0f);

			m_moveParm.turnSpeed = turnSpeed;
		}

		{
			float velocity[] = { m_moveParm.velocity.x,m_moveParm.velocity.y,m_moveParm.velocity.z };
			ImGui::SliderFloat3("Velocity", velocity, -100.0f, 100.0f);

		}
		static float  speed = m_moveParm.speed.x;
		ImGui::Text("Speed : %f", &speed);


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

		//FrameCount
		{
			static float frameCount = 0;
			ImGui::InputFloat("FrameCount", &frameCount, 0, 100); ImGui::SameLine();
			if (ImGui::ArrowButton("Front", ImGuiDir_Left))
			{
				if (0 >= frameCount)
					frameCount = 0;
				else
					--frameCount;
			}
			ImGui::SameLine();
			if (ImGui::ArrowButton("Next", ImGuiDir_Right))
				++frameCount;
			if (ImGui::Button("ADD FrameCount"))
				m_stepParm.frameCount = frameCount;
		}

		float accel = m_stepParm.deceleration.x;
		ImGui::SliderFloat("StepAccel", &accel, 0.0f, 1.0f);

		float speed = m_stepParm.speed.x;
		ImGui::SliderFloat("StepSpeed", &speed, 0.0f, 100.0f);

		float animationSpeed = m_blendAnimation.animtionSpeed;
		ImGui::SliderFloat("AnimationSpeed", &animationSpeed, 1.0f, 5.0f);


		ImGui::SliderFloat("Radian", &radian, 0.0f, 50.0f);


		m_stepParm.deceleration = VECTOR3F(accel, 0.0f, accel);
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

	//**************************************
	// Attack
	//**************************************
	if (ImGui::CollapsingHeader("Attack"))
	{
		static int current = 0;
		ImGui::RadioButton("RightPunch", &current, 0); ImGui::SameLine();
		ImGui::RadioButton("LeftPunch" , &current, 1); ImGui::SameLine();
		ImGui::RadioButton("LastPunch" , &current, 2); ImGui::SameLine();
		ImGui::RadioButton("RightKick" , &current, 3); ImGui::SameLine();
		ImGui::RadioButton("LeftKick"  , &current, 4); 

		//FrameCount
		{
			static int frameCount = 0;
			ImGui::InputInt("FrameCount", &frameCount, 0, 100); ImGui::SameLine();
			if (ImGui::ArrowButton("Front", ImGuiDir_Left))
			{
				if (0 >= frameCount)
					frameCount = 0;
				else
					--frameCount;
			}
			ImGui::SameLine();
			if (ImGui::ArrowButton("Next", ImGuiDir_Right))
				++frameCount;
			if (ImGui::Button("ADD FrameCount"))
				m_attackParm[current].frameCount = frameCount;
		}
		
		//Buttons
		{
			static int button = 0;
			std::vector<std::string> padName =
			{
					"PAD_UP",
					"PAD_DOWN",
					"PAD_LEFT",
					"PAD_RIGHT",
					"PAD_START",
					"PAD_BACK",
					"PAD_LTHUMB",
					"PAD_RTHUMB",
					"PAD_LSHOULDER",
					"PAD_RSHOULDER",
					"PAD_A",
					"PAD_B",
					"PAD_X",
					"PAD_Y"
			};

			ImGui::Combo("Name_of_Buttons",
				&button,
				vectorGetter,
				static_cast<void*>(&padName),
				static_cast<int>(padName.size())
			);
			ImGui::BulletText("Size %d", m_attackParm[current].buttons.size()); ImGui::SameLine();

			if (ImGui::Button("ADD Buttons"))
			{
				if (m_attackParm[current].buttons.size() != 2)
					m_attackParm[current].buttons.push_back(static_cast<XINPUT_GAMEPAD_BUTTONS>(button));
			}
			ImGui::SameLine();
			if (ImGui::Button("POP Buttons"))
			{
				m_attackParm[current].buttons.pop_back();
			}

		}

		//InputRange
		{
			static int start = m_attackParm[current].inputRange[0];
			ImGui::InputInt("Start", &start, 0, 100);

			if (ImGui::Button("ADD InputRange[0]"))
				m_attackParm[current].inputRange[0] = start;

			static int end = m_attackParm[current].inputRange[1];
			ImGui::InputInt("End", &end, 0, 100); 

			if (ImGui::Button("ADD InputRange[1]"))
				m_attackParm[current].inputRange[1] = end;
		}

		//attackPoint
		{
			static float point = m_attackParm[current].attackPoint;
			ImGui::InputFloat("AttackPoint", &point, 0, 100); 

			if (ImGui::Button("ADD AttackPoint"))
				m_attackParm[current].attackPoint = point;
		}
	}

	ImGui::End();
#endif
}

void Fighter::Attacking(Animation currentAnimation, Animation nextAnimations[2],
	CharacterParameter::Attack& attack, CharacterParameter::Collision collision)
{
	//*-*-*-*-*-*-*-*-*-*-*-*-*-
	// Reset the first time.
	//*-*-*-*-*-*-*-*-*-*-*-*-*-
	if (!m_statusParm.isAttack)
	{
		m_blendAnimation.animationBlend.ResetAnimationFrame();
		m_blendAnimation.animationBlend.ResetAnimationSampler(0);
		m_blendAnimation.animationBlend.ResetAnimationSampler(1);
		m_blendAnimation.animationBlend._blendRatio = 0.0f;
		m_statusParm.isAttack = true;
	}

	//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
	// Set the blend ratio to 1.
	// When we get to 1, we stop blending.
	//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-

	m_blendAnimation.animationBlend._blendRatio += m_blendAnimation.attackBlendRtio;
	if (m_blendAnimation.animationBlend._blendRatio >= 1.0f)
	{
		m_blendAnimation.animationBlend._blendRatio = 1.0f;
		m_blendAnimation.animationBlend.ReleaseSampler(0);
		m_blendAnimation.animationBlend.FalseAnimationLoop(0);
	}


	//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
	// If the button is pressed within the attack
	// change range, the attack is changed
	//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
	uint32_t  currentAnimationFrame = m_blendAnimation.animationBlend.GetAnimationTime(0);
	int nextAttackCount = static_cast<int>(attack.buttons.size());

	if (attack.inputRange[0] < currentAnimationFrame && currentAnimationFrame < attack.inputRange[1])
	{
		for (int i = 0; i < nextAttackCount; ++i)
		{
			if (m_input->GetButtons(attack.buttons[i]) == 1)
			{
				m_blendAnimation.animationBlend.ResetAnimationFrame();
				m_blendAnimation.animationBlend.AddSampler(nextAnimations[i], m_model);
				m_blendAnimation.animationBlend.ResetAnimationSampler(1);
				m_animationType = nextAnimations[i];
				m_blendAnimation.animationBlend._blendRatio = {};

				return;
			}
			else if (m_input->GetButtons(XINPUT_GAMEPAD_BUTTONS::PAD_A) == 1)
			{
				m_blendAnimation.animationBlend.ResetAnimationSampler(0);
				m_blendAnimation.animationBlend.ResetAnimationFrame();
				m_statusParm.isAttack = false;
				m_animationType = Fighter::Animation::DIVE;
				Stepping(m_elapsedTime);
				return;
			}
		}

	}

	//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
	//When the animation ends, the attack ends.
	//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
	if (currentAnimationFrame == attack.frameCount)
	{
		m_blendAnimation.animationBlend._blendRatio = 0.0f;
		m_blendAnimation.animationBlend.ResetAnimationSampler(0);
		m_blendAnimation.animationBlend.ResetAnimationFrame();
		m_animationType = Fighter::Animation::IDLE;
		m_blendAnimation.animationBlend.AddSampler(m_animationType, m_model);
	}
}

void Fighter::Stepping(float& elapsedTime)
{
	//*-*-*-*-*-*-*-*-*-*-*-*-*-
	// Reset the first time.
	//*-*-*-*-*-*-*-*-*-*-*-*-*-
	if (!m_stepParm.isStep)
	{
		m_blendAnimation.animationBlend.ResetAnimationFrame();
		m_blendAnimation.animationBlend.ChangeSampler(0, m_animationType, m_model);
		m_blendAnimation.animationBlend.FalseAnimationLoop(0);
		m_blendAnimation.animationBlend._blendRatio = 0.0f;
		m_stepParm.isStep = true;
	}

	//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
	// Set the direction and move it.
	//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
	m_stepParm.speed -= m_stepParm.deceleration;
	if (m_stepParm.speed.x <= 0)
		m_stepParm.speed = { 0.0f,0.0f,0.0f };

	m_moveParm.velocity.x = sinf(m_transformParm.angle.y) * (m_stepParm.speed.x);
	m_moveParm.velocity.y = 0.0f;
	m_moveParm.velocity.z = cosf(m_transformParm.angle.y) * (m_stepParm.speed.z);

	m_transformParm.position += m_moveParm.velocity * elapsedTime;
	m_transformParm.WorldUpdate();

	//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
	//When the animation ends, the attack ends.
	//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
	uint32_t  currentAnimationFrame = m_blendAnimation.animationBlend.GetAnimationTime(0);
	if (currentAnimationFrame == m_stepParm.frameCount)
	{
		m_blendAnimation.animationBlend._blendRatio = 0.0f;
		m_blendAnimation.animationBlend.ResetAnimationSampler(0);
		m_blendAnimation.animationBlend.ResetAnimationFrame();
		m_animationType = Fighter::Animation::IDLE;
		m_blendAnimation.animationBlend.AddSampler(m_animationType, m_model);
	}
}


