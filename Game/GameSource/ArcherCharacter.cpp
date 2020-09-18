#include "ArcherCharacter.h"
#include "MessengTo.h"
#include ".\LibrarySource\ModelData.h"
#include ".\LibrarySource\VectorCombo.h"

#ifdef _DEBUG
#include "..\External_libraries\imgui\imgui.h"
#include "..\External_libraries\imgui\imgui_impl_dx11.h"
#include "..\External_libraries\imgui\imgui_impl_win32.h"
#include "..\External_libraries\imgui\imgui_internal.h"
#endif
CEREAL_CLASS_VERSION(Archer, 4);

void Archer::Init()
{
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

	m_padDeadLine = 5000.0f;
	m_elapsedTime = 0.0f;



	m_transformParm.position = { 30.0f,0.0f,0.0f };
	m_transformParm.angle = { 0.0f * 0.01745f, 0.0f * 0.01745f,0.0f * 0.017454f };
	m_transformParm.scale = { 0.05f,0.05f,0.05f };
	m_transformParm.WorldUpdate();

	m_changeParm.isPlay = true;
	m_blendAnimation.animationBlend.Init(m_model);
	//m_blendAnimation.partialBlend.Init(m_model);

	SerialVersionUpdate(5);

	if (PathFileExistsA((std::string("../Asset/Binary/Player/Archer/Parameter") + ".bin").c_str()))
	{
		std::ifstream ifs;
		ifs.open((std::string("../Asset/Binary/Player/Archer/Parameter") + ".bin").c_str(), std::ios::binary);
		cereal::BinaryInputArchive i_archive(ifs);
		i_archive(*this);
	}

	m_stepParm.maxSpeed = m_stepParm.speed;

}

void Archer::Update(float& elapsedTime)
{
	m_elapsedTime = elapsedTime;
	if (m_changeParm.isPlay)
	{
		m_input = PAD.GetPad(0);

		if (m_input != nullptr)
		{

			if (m_mode != Mode::Aiming)
			{
				Move(m_elapsedTime);
				Aim();
				Step(m_elapsedTime);
			}
			else
			{
				Aiming();
				AimMove(m_elapsedTime);
				AimStep(m_elapsedTime);
			}



			ChangeCharacter();

			RestAnimationIdle();
		}
	}
	else
	{
		m_input = PAD.GetPad(0);

		//Step(m_elapsedTime);

		//Attack(m_elapsedTime);

		RestAnimationIdle();

	}

	m_blendAnimation.animationBlend.Update(m_model, m_elapsedTime);

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

void Archer::RestAnimationIdle()
{
	if (m_blendAnimation.animationBlend.GetSampler().size() == m_blendAnimation.samplerSize)
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
					m_moveParm.isMove = false;
					m_blendAnimation.animationBlend._blendRatio = 0.0f;
					m_stepParm.speed = m_stepParm.maxSpeed;

				}
			}
		}
	}

}

void Archer::ChangeCharacter()
{
	if (m_input->GetButtons(XINPUT_GAMEPAD_BUTTONS::PAD_RIGHT) == 1)
	{
		m_changeParm.changeType = CharacterParameter::Change::PlayerType::FIGHTER;

		MESSENGER.MessageFromPlayer(m_id, MessengType::CHANGE_PLAYER);

		m_input->ResetButton(XINPUT_GAMEPAD_BUTTONS::PAD_RIGHT);
	}
}

//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
//IDLE WALK RUN : animtionBlend
//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
void Archer::Move(float& elapsedTime)
{
	if (m_stepParm.isStep || m_statusParm.isAttack || m_aimMode.isAim) return;

	if (m_input->StickDeadzoneLX(m_padDeadLine) || m_input->StickDeadzoneLY(m_padDeadLine))
	{
		if (!m_moveParm.isMove)
		{
			m_blendAnimation.animationBlend.ResetAnimationSampler(0);
			m_blendAnimation.animationBlend.AddSampler(Animation::WALK, m_model);
			m_blendAnimation.animationBlend.AddSampler(Animation::RUN, m_model);
			m_moveParm.isMove = true;
			m_moveParm.isWalk = true;
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

		//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
		// Giving an acceleration value 
		// to the direction of the stick
		//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
		m_moveParm.velocity.x += stickVector.x * m_moveParm.accle.x;
		m_moveParm.velocity.y += stickVector.y * m_moveParm.accle.y;
		m_moveParm.velocity.z += stickVector.z * m_moveParm.accle.z;

		//*-*-*-*-*-*-*-*-*-*-*-*-*-*-
		// VectorLength be the speed
		//*-*-*-*-*-*-*-*-*-*-*-*-*-*-
		float speed = ToDistVec3(m_moveParm.velocity);

		//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
		// Get the distance of the stick.
		//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
		float leftX = m_input->GetStickLeftXValue();
		float leftY = m_input->GetStickLeftYValue();
		float distance = sqrtf(leftX * leftX + leftY * leftY) / 32767;

		VECTOR3F vector = {};
		//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
		// If it's faster than a deceleration.
		//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
		if (speed > m_moveParm.decleleration)
		{
			//*-*-*-*-*-*-*-*-
			//Get Directions
			//*-*-*-*-*-*-*-*-
			vector = m_moveParm.velocity / speed;

			if (m_moveParm.isWalk)
			{
				//*-*-*-*-*-*-*-*-*-*-*-*-
				//If it's over max speed.
				//*-*-*-*-*-*-*-*-*-*-*-*-
				if (speed > m_moveParm.maxSpeed[0].x)
					m_moveParm.velocity = vector * m_moveParm.maxSpeed[0].x;
				else
				{
					m_moveParm.velocity -= vector * m_moveParm.decleleration;
					//m_blendAnimation.animationBlend._blendRatio += m_blendAnimation.moveBlendRatio;
					//if (m_blendAnimation.animationBlend._blendRatio >= m_blendAnimation.blendValueRange[0])
					//{
					//	m_blendAnimation.animationBlend._blendRatio = m_blendAnimation.blendValueRange[0];
					//	m_moveParm.isWalk = true;

					//}
				}

			}
			else if(m_moveParm.isRun)
			{
				if (speed > m_moveParm.maxSpeed[1].x)
					m_moveParm.velocity = vector * m_moveParm.maxSpeed[1].x;
				else
				{
					m_moveParm.velocity -= vector * m_moveParm.decleleration;
					//m_blendAnimation.animationBlend._blendRatio -= m_blendAnimation.moveBlendRatio;
					//if (m_blendAnimation.animationBlend._blendRatio <= m_blendAnimation.blendValueRange[0])
					//{
					//	m_blendAnimation.animationBlend._blendRatio = m_blendAnimation.blendValueRange[0];
					//	m_moveParm.isRun = false;
					//}
				}

			}

			//*-*-*-*-*-*-*-*-*-*-*-*-*-*-
			//Calculation of Blend Value
			//*-*-*-*-*-*-*-*-*-*-*-*-*-*-
			if (0.0f < distance && distance < m_blendAnimation.blendValueRange[0])
			{
				if (m_moveParm.isRun)
				{
					m_blendAnimation.animationBlend._blendRatio -= m_blendAnimation.moveBlendRatio;
					if (m_blendAnimation.animationBlend._blendRatio <= m_blendAnimation.blendValueRange[0])
					{
						m_blendAnimation.animationBlend._blendRatio = m_blendAnimation.blendValueRange[0];
						m_moveParm.isRun = false;
					}
				}
				else
				{
					m_blendAnimation.animationBlend._blendRatio += m_blendAnimation.moveBlendRatio;
					if (m_blendAnimation.animationBlend._blendRatio >= m_blendAnimation.blendValueRange[0])
					{
						m_blendAnimation.animationBlend._blendRatio = m_blendAnimation.blendValueRange[0];
						m_moveParm.isWalk = true;

					}
				}

			}
			else
			{
				m_blendAnimation.animationBlend._blendRatio += m_blendAnimation.moveBlendRatio;
				if (m_blendAnimation.animationBlend._blendRatio >= m_blendAnimation.blendValueRange[1])
				{
					m_blendAnimation.animationBlend._blendRatio = m_blendAnimation.blendValueRange[1];
				}
				m_moveParm.isRun = true;
				m_moveParm.isWalk = false;

			}


			m_transformParm.position += m_moveParm.velocity * elapsedTime;


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
			m_blendAnimation.animationBlend._blendRatio -= m_blendAnimation.idleBlendRtio;
			if (m_blendAnimation.animationBlend._blendRatio <= 0.0f)
			{
				m_blendAnimation.animationBlend._blendRatio = 0.0f;
				m_blendAnimation.animationBlend.ReleaseSampler(1);
				m_blendAnimation.animationBlend.ReleaseSampler(1);
				m_moveParm.isMove = false;
				m_moveParm.isWalk = false;
				m_moveParm.isRun = false;
			}
		}
	}
}

//*-*-*-*-*-*-*-*-*-*-*-
//STEP : animation
//*-*-*-*-*-*-*-*-*-*-*-
void Archer::Step(float& elapsedTime)
{
	if (m_input->GetButtons(XINPUT_GAMEPAD_BUTTONS::PAD_A) == 1 && !m_statusParm.isAttack || m_stepParm.isStep)
	{
		m_animationType = Animation::DIVE;
		Stepping(elapsedTime);
	}
}

void Archer::Stepping(float& elapsedTime)
{
	//*-*-*-*-*-*-*-*-*-*-*-*-*-
	// Reset the first time.
	//*-*-*-*-*-*-*-*-*-*-*-*-*-
	if (!m_stepParm.isStep)
	{
		m_blendAnimation.animationBlend.ResetAnimationFrame();
		int samplerCount = static_cast<int>(m_blendAnimation.animationBlend.GetSampler().size());
		for (int i = 0; i < samplerCount - 1; ++i)
		{
			m_blendAnimation.animationBlend.ReleaseSampler(1);
		}
		m_blendAnimation.animationBlend.ChangeSampler(0, m_animationType, m_model);
		m_blendAnimation.animationBlend.FalseAnimationLoop(0);
		m_blendAnimation.animationBlend._blendRatio = 0.0f;
		m_stepParm.isStep = true;
		m_moveParm.isMove = false;
		m_moveParm.isWalk = false;
		m_moveParm.isRun = false;

		//********************************
		//	Angle in the STEP direction
		//********************************
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
			VECTOR3F stickVec(m_input->StickVectorLeft().x, 0.0f, m_input->StickVectorLeft().y);
			DirectX::XMVECTOR vStickVex = DirectX::XMLoadFloat3(&stickVec);

			VECTOR3F stickVector;
			vStickVex = DirectX::XMVector4Transform(vStickVex, viewMatrix);
			DirectX::XMStoreFloat3(&stickVector, vStickVex);

			float dot = atan2f(stickVector.x, stickVector.z);

			m_transformParm.angle.y = dot;
		}
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
		m_animationType = Animation::IDLE;
		m_blendAnimation.animationBlend.ChangeSampler(0, m_animationType, m_model);
		m_stepParm.isStep = false;
		m_blendAnimation.animationBlend._blendRatio = 0.0f;
		m_stepParm.speed = m_stepParm.maxSpeed;
	}

}

void Archer::Aim()
{
	if (m_stepParm.isStep) return;
	if (m_input->GetButtons(XINPUT_GAMEPAD_BUTTONS::PAD_LSHOULDER) == 1)
	{
		//****************************
		// Change of Animationtype
		//****************************
		if (m_moveParm.isMove)
		{
			m_moveParm.velocity = {};
			m_animationType = Animation::AIMWALK;
		}
		else
			m_animationType = Animation::AIM;

		//*******************************
		// Blending process according to 
		// the number of samplers
		//*******************************
		if (m_blendAnimation.animationBlend.GetSampler().size() == 2)
		{
			if (m_blendAnimation.animationBlend.GetSampler()[1].first != m_animationType)
				m_blendAnimation.animationBlend.AddSampler(m_animationType, m_model);
		}
		else if (m_blendAnimation.animationBlend.GetSampler().size() == 3)
		{
			float ratio = m_blendAnimation.animationBlend._blendRatio;
			if (0.0f < ratio && ratio < m_blendAnimation.blendValueRange[0])
			{
				m_blendAnimation.animationBlend.ResetAnimationSampler(2);
				m_blendAnimation.animationBlend.ReleaseSampler(2);
				m_blendAnimation.animationBlend.ResetAnimationSampler(0);
				m_blendAnimation.animationBlend.ReleaseSampler(0);

			}
			else
			{
				m_blendAnimation.animationBlend.ResetAnimationSampler(1);
				m_blendAnimation.animationBlend.ReleaseSampler(1);
				m_blendAnimation.animationBlend.ResetAnimationSampler(0);
				m_blendAnimation.animationBlend.ReleaseSampler(0);
			}


			m_moveParm.isMove = false;
			m_moveParm.isWalk = false;
			m_moveParm.isRun = false;



			m_blendAnimation.animationBlend.AddSampler(m_animationType, m_model);

		}
		else
			m_blendAnimation.animationBlend.AddSampler(m_animationType, m_model);

		if (!m_aimMode.isAim)
			m_blendAnimation.animationBlend._blendRatio = 0.0f;

		m_mode = Mode::Aiming;


		//********************************
		// Drection of Target
		//********************************
		VECTOR3F eye = Source::CameraControlle::CameraManager::GetInstance()->GetEye();
		VECTOR3F target = Source::CameraControlle::CameraManager::GetInstance()->GetTarget();
		VECTOR3F distance = target - eye;
		float dot = atan2f(distance.x, distance.z);
		m_transformParm.angle.y = dot;
		m_transformParm.WorldUpdate();

		//*****************************
		// Messenge of Aim
		//*****************************
		MESSENGER.MessageFromPlayer(m_id, MessengType::SHIFT_AIM_MODE);

	}

}

void Archer::Aiming()
{
	if (m_animationType == Animation::DIVE) return;

	if (!m_aimMode.isAim)
	{
		if (m_input->GetButtons(XINPUT_GAMEPAD_BUTTONS::PAD_LSHOULDER) == 0)
		{
			m_blendAnimation.animationBlend._blendRatio -= m_blendAnimation.idleBlendRtio;

			if (m_blendAnimation.animationBlend._blendRatio <= 0.0f)
			{
				m_blendAnimation.animationBlend._blendRatio = 0.0f;
				m_blendAnimation.animationBlend.ResetAnimationSampler(1);
				m_blendAnimation.animationBlend.ReleaseSampler(1);

				m_mode = Mode::Moving;
				m_aimMode.isAim = false;
				MESSENGER.MessageFromPlayer(m_id, MessengType::SHIFT_AIM_MODE);

			}
			VECTOR3F nowLength = Source::CameraControlle::CameraManager().GetInstance()->GetLength();
			VECTOR3F length = LerpVec3(m_cameraParm.lenght, nowLength, m_blendAnimation.animationBlend._blendRatio);

			Source::CameraControlle::CameraManager().GetInstance()->SetLength(length);
		}
		else
		{
			m_blendAnimation.animationBlend._blendRatio += m_blendAnimation.idleBlendRtio;

			if (m_blendAnimation.animationBlend._blendRatio >= 1.0f)
			{
				m_blendAnimation.animationBlend._blendRatio = 0.0f;
				m_blendAnimation.animationBlend.ResetAnimationSampler(0);
				m_blendAnimation.animationBlend.ReleaseSampler(0);
				m_aimMode.isAim = true;
			}
			VECTOR3F nowLength = Source::CameraControlle::CameraManager().GetInstance()->GetLength();

			VECTOR3F length = LerpVec3(nowLength, m_aimMode.aimCameraParm.lenght, m_blendAnimation.animationBlend._blendRatio);

			Source::CameraControlle::CameraManager().GetInstance()->SetLength(length);
		}
	}
	else
	{
		if (m_input->GetButtons(XINPUT_GAMEPAD_BUTTONS::PAD_LSHOULDER) == -1)
		{
			if (m_animationType != Animation::IDLE)
			{
				m_animationType = Animation::IDLE;
				m_blendAnimation.animationBlend.AddSampler(m_animationType, m_model);
				m_blendAnimation.animationBlend._blendRatio = 0.0f;
			}

		}
		else if (m_animationType == Animation::IDLE)
		{
			m_blendAnimation.animationBlend._blendRatio += m_blendAnimation.idleBlendRtio;

			if (m_blendAnimation.animationBlend._blendRatio >= 1.0f)
			{
				m_blendAnimation.animationBlend._blendRatio = 0.0f;
				m_blendAnimation.animationBlend.ResetAnimationSampler(0);
				m_blendAnimation.animationBlend.ReleaseSampler(0);

				m_mode = Mode::Moving;
				m_aimMode.isAim = false;
				MESSENGER.MessageFromPlayer(m_id, MessengType::SHIFT_AIM_MODE);

			}
			VECTOR3F nowLength = Source::CameraControlle::CameraManager().GetInstance()->GetLength();
			VECTOR3F length = LerpVec3(nowLength, m_cameraParm.lenght, m_blendAnimation.animationBlend._blendRatio);

			Source::CameraControlle::CameraManager().GetInstance()->SetLength(length);
		}

	}
}

void Archer::AimMove(float& elapsedTime)
{
	if (!m_aimMode.isAim || m_animationType == Animation::DIVE) return;

	if (m_input->StickDeadzoneLX(m_padDeadLine) || m_input->StickDeadzoneLY(m_padDeadLine))
	{
		if (!m_aimMode.aimMoveParm.isMove)
		{
			m_animationType = Animation::AIMWALK;
			m_blendAnimation.animationBlend.ResetAnimationSampler(0);
			m_blendAnimation.animationBlend.AddSampler(m_animationType, m_model);
			m_aimMode.aimMoveParm.isMove = true;
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

		//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
		// Giving an acceleration value 
		// to the direction of the stick
		//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
		m_aimMode.aimMoveParm.velocity.x += stickVector.x * m_aimMode.aimMoveParm.accle.x;
		m_aimMode.aimMoveParm.velocity.y += stickVector.y * m_aimMode.aimMoveParm.accle.y;
		m_aimMode.aimMoveParm.velocity.z += stickVector.z * m_aimMode.aimMoveParm.accle.z;

		//*-*-*-*-*-*-*-*-*-*-*-*-*-*-
		// VectorLength be the speed
		//*-*-*-*-*-*-*-*-*-*-*-*-*-*-
		float speed = ToDistVec3(m_aimMode.aimMoveParm.velocity);

		//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
		// Get the distance of the stick.
		//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
		float leftX = m_input->GetStickLeftXValue();
		float leftY = m_input->GetStickLeftYValue();
		float distance = sqrtf(leftX * leftX + leftY * leftY) / 32767;

		VECTOR3F vector = {};
		//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
		// If it's faster than a deceleration.
		//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
		if (speed > m_aimMode.aimMoveParm.decleleration)
		{
			//*-*-*-*-*-*-*-*-
			//Get Directions
			//*-*-*-*-*-*-*-*-
			vector = m_aimMode.aimMoveParm.velocity / speed;


			//*-*-*-*-*-*-*-*-*-*-*-*-
			//If it's over max speed.
			//*-*-*-*-*-*-*-*-*-*-*-*-
			if (speed > m_aimMode.aimMoveParm.maxSpeed[0].x)
				m_aimMode.aimMoveParm.velocity = vector * m_aimMode.aimMoveParm.maxSpeed[0].x;
			else
			{
				m_aimMode.aimMoveParm.velocity -= vector * m_aimMode.aimMoveParm.decleleration;
			}

			if (m_blendAnimation.animationBlend.GetSampler().size() == 2)
			{
				m_blendAnimation.animationBlend._blendRatio += m_blendAnimation.aimMoveBlendRatio;
				if (m_blendAnimation.animationBlend._blendRatio >= m_blendAnimation.blendValueRange[1])
				{
					m_blendAnimation.animationBlend._blendRatio = 0.0f;
					m_blendAnimation.animationBlend.ResetAnimationSampler(0);
					m_blendAnimation.animationBlend.ReleaseSampler(0);
				}
			}

			m_transformParm.position += m_aimMode.aimMoveParm.velocity * elapsedTime;


			//********************************
			// Drection of Target
			//********************************
			VECTOR3F eye = Source::CameraControlle::CameraManager::GetInstance()->GetEye();
			VECTOR3F target = Source::CameraControlle::CameraManager::GetInstance()->GetTarget();
			VECTOR3F distance = target - eye;
			float dot = atan2f(distance.x, distance.z);
			m_transformParm.angle.y = dot;

			m_transformParm.WorldUpdate();


		}
		else
		{
			m_aimMode.aimMoveParm.velocity = {};
		}
	}
	else
	{
		m_aimMode.aimMoveParm.speed = {};

		//*********************************
		// The elseif is a bug prevention
		//*********************************
		if (m_aimMode.aimMoveParm.isMove)
		{
			if (m_animationType != Animation::AIM)
			{
				m_animationType = Animation::AIM;
				m_blendAnimation.animationBlend.AddSampler(m_animationType, m_model);
				m_blendAnimation.animationBlend._blendRatio = 0.0f;
			}

			m_blendAnimation.animationBlend._blendRatio += m_blendAnimation.idleBlendRtio;

			if (m_blendAnimation.animationBlend._blendRatio >= 1.0f)
			{
				m_blendAnimation.animationBlend._blendRatio = 0.0f;
				m_blendAnimation.animationBlend.ResetAnimationSampler(0);
				m_blendAnimation.animationBlend.ReleaseSampler(0);
				m_aimMode.aimMoveParm.isMove = false;

			}
		}
		else if (m_animationType == Animation::AIMWALK)
		{
			if (m_blendAnimation.animationBlend.GetSampler().size() == 1)
			{
				m_blendAnimation.animationBlend.AddSampler(Animation::AIM, m_model);
				m_blendAnimation.animationBlend._blendRatio = 0.0f;
			}
			m_blendAnimation.animationBlend._blendRatio += m_blendAnimation.idleBlendRtio;

			if (m_blendAnimation.animationBlend._blendRatio >= 1.0f)
			{
				m_blendAnimation.animationBlend._blendRatio = 0.0f;
				m_blendAnimation.animationBlend.ResetAnimationSampler(0);
				m_blendAnimation.animationBlend.ReleaseSampler(0);
				m_aimMode.aimMoveParm.isMove = false;
				m_animationType = Animation::AIM;
			}
		}
	}
}

void Archer::AimStep(float& elapsedTime)
{
	if (!m_aimMode.isAim) return;
	if(m_blendAnimation.animationBlend._blendRatio == 0.0f)
	if (m_input->GetButtons(XINPUT_GAMEPAD_BUTTONS::PAD_A) == 1 && !m_statusParm.isAttack && m_animationType != Animation::DIVE)
	{
		//*-*-*-*-*-*-*-*-*-*-*-*-*-
		// Reset the first time.
		//*-*-*-*-*-*-*-*-*-*-*-*-*-		

		m_animationType = Animation::DIVE;
		m_blendAnimation.animationBlend.AddSampler(m_animationType, m_model);

		m_blendAnimation.animationBlend._blendRatio = 0.0f;
		MESSENGER.MessageFromPlayer(m_id, MessengType::SHIFT_AIM_MODE);

		m_aimMode.aimMoveParm.isMove = false;
		//********************************
		//	Angle in the STEP direction
		//********************************
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
				VECTOR3F stickVec(m_input->StickVectorLeft().x, 0.0f, m_input->StickVectorLeft().y);
				DirectX::XMVECTOR vStickVex = DirectX::XMLoadFloat3(&stickVec);

				VECTOR3F stickVector;
				vStickVex = DirectX::XMVector4Transform(vStickVex, viewMatrix);
				DirectX::XMStoreFloat3(&stickVector, vStickVex);

				float dot = atan2f(stickVector.x, stickVector.z);

				m_transformParm.angle.y = dot;
			}
		
	}
	if (m_animationType == Animation::DIVE)
		AimStepping(elapsedTime);
}

void Archer::AimStepping(float& elapsedTime)
{
	//*-*-*-*-*-*-*-*-*-*-*-*-*-
	// BlendRatio Upadete
	//*-*-*-*-*-*-*-*-*-*-*-*-*-
	if (!m_aimMode.isStep)
	{
		m_blendAnimation.animationBlend._blendRatio += m_blendAnimation.idleBlendRtio;

		if (m_blendAnimation.animationBlend._blendRatio >= 1.0f)
		{
			m_blendAnimation.animationBlend._blendRatio = 1.0f;
			m_blendAnimation.animationBlend.ResetAnimationSampler(0);
			m_blendAnimation.animationBlend.ReleaseSampler(0);
			m_blendAnimation.animationBlend.FalseAnimationLoop(0);

			m_aimMode.isStep = true;
		}
		VECTOR3F nowLength = Source::CameraControlle::CameraManager().GetInstance()->GetLength();
		VECTOR3F length = LerpVec3(nowLength, m_cameraParm.lenght, m_blendAnimation.animationBlend._blendRatio);

		Source::CameraControlle::CameraManager().GetInstance()->SetLength(length);
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

		//*****************************
		// If it's not AimMode
		//*****************************
		if (m_input->GetButtons(XINPUT_GAMEPAD_BUTTONS::PAD_LSHOULDER) > 0)
		{
			m_blendAnimation.animationBlend.ResetAnimationSampler(0);
			m_blendAnimation.animationBlend.ResetAnimationFrame();
			m_blendAnimation.animationBlend._blendRatio = 0.0f;
			m_mode = Mode::Aiming;
			if (m_input->StickDeadzoneLX(m_padDeadLine) || m_input->StickDeadzoneLY(m_padDeadLine))
			{
				m_animationType = Animation::AIMWALK;
				m_blendAnimation.animationBlend.AddSampler(m_animationType, m_model);
			}
			else
			{

				m_animationType = Animation::AIM;
				m_blendAnimation.animationBlend.AddSampler(m_animationType, m_model);
			}

			//********************************
			// Drection of Target
			//********************************
			VECTOR3F eye = Source::CameraControlle::CameraManager::GetInstance()->GetEye();
			VECTOR3F target = Source::CameraControlle::CameraManager::GetInstance()->GetTarget();
			VECTOR3F distance = target - eye;
			float dot = atan2f(distance.x, distance.z);
			m_transformParm.angle.y = dot;
			m_transformParm.WorldUpdate();

			MESSENGER.MessageFromPlayer(m_id, MessengType::SHIFT_AIM_MODE);
		}
		else
		{
			m_blendAnimation.animationBlend._blendRatio = 0.0f;
			m_blendAnimation.animationBlend.ResetAnimationSampler(0);
			m_blendAnimation.animationBlend.ResetAnimationFrame();
			m_animationType = Animation::IDLE;
			m_mode = Mode::Moving;
			m_blendAnimation.animationBlend.ChangeSampler(0, m_animationType, m_model);
		}

		m_aimMode.isAim = false;
		m_aimMode.isStep = false;
		m_stepParm.speed = m_stepParm.maxSpeed;
		m_moveParm.velocity = {};
	}

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
	
	//**************************************
	// Animation
	//**************************************
	if (ImGui::CollapsingHeader("Animation"))
	{
		{
			float ratio = m_blendAnimation.animationBlend._blendRatio;
			ImGui::SliderFloat("BlendRatio", &ratio, 0.0f, 1.0f);
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
		

			static float aimMoveRatio = 0;
			ImGui::SliderFloat("AimMoveBlendRatio", &aimMoveRatio, 0.0f, 1.0f);
			if (ImGui::Button("AimMove BlendRatio"))
				m_blendAnimation.aimMoveBlendRatio = aimMoveRatio;
		}

		{
			static float blendValue0 = 0.5f;
			ImGui::SliderFloat("WalkBlendValue", &blendValue0, 0.0f, 1.0f);
			if (ImGui::Button("Set WalkBlendValue"))
				m_blendAnimation.blendValueRange[0] = blendValue0;

			static float blendValue1 = 1.0f;
			ImGui::SliderFloat("RunBlendValue", &blendValue1, 0.0f, 1.0f);
			if (ImGui::Button("Set RunBlendValue"))
				m_blendAnimation.blendValueRange[1] = blendValue1;
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


		if(m_aimMode.isAim)
		{
			static float aimModeLength = m_aimMode.aimCameraParm.lenght.x;
			ImGui::SliderFloat("CameraLength", &aimModeLength, -100, 100);

			ImGui::InputFloat("HeightAboveGround", &m_aimMode.aimCameraParm.heightAboveGround, 0.5f, 0.5f);

			static float right = 0.0f;
			ImGui::SliderFloat("Right", &right, 0.0, -100.0f);

			FLOAT4X4 modelAxisTransform = m_model->_resource->axisSystemTransform;
			FLOAT4X4 world = modelAxisTransform * m_transformParm.world ;

			VECTOR3F rightAxis = { world._11,world._12,world._13 };
			rightAxis = NormalizeVec3(rightAxis);

			//if (rightAxis.x >= 0)
			//	rightAxis.x *= -1;
			//if (rightAxis.z >= 0)
			//	rightAxis.z *= -1;

			float rigthValue = rightAxis.x * right;

			m_aimMode.aimCameraParm.lenght.x = aimModeLength;
			m_aimMode.aimCameraParm.lenght.y = 0.0f;
			m_aimMode.aimCameraParm.lenght.z = aimModeLength;

			Source::CameraControlle::CameraManager::GetInstance()->SetLength(m_aimMode.aimCameraParm.lenght);
			Source::CameraControlle::CameraManager::GetInstance()->SetFocalLength(rigthValue);
		}
		else
		{
			if(ImGui::Button("SetLength"))
				Source::CameraControlle::CameraManager::GetInstance()->SetLength(m_cameraParm.lenght);

			{
				static float length = m_cameraParm.lenght.x;
				ImGui::SliderFloat("CameraLength", &length, -100, 100);

				ImGui::InputFloat("HeightAboveGround", &m_cameraParm.heightAboveGround, 0.5f, 0.5f);

				ImGui::SliderFloat("Value", &m_cameraParm.value, 0.0, 1.0f);

				m_cameraParm.lenght.x = length;
				m_cameraParm.lenght.y = 0.0f;
				m_cameraParm.lenght.z = length;
			}
		}
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

		m_stepParm.deceleration = VECTOR3F(accel, 0.0f, accel);

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
	// AimMove
	//**************************************
	if (ImGui::CollapsingHeader("AimModeMove"))
	{
		{
			static float  accel0 = m_moveParm.accle.x;

			ImGui::SliderFloat("AimModeAccel0", &accel0, 0.0f, 10.0f);

			m_aimMode.aimMoveParm.accle = { accel0 ,0.0f,accel0 };
		}

		{
			static float  maxSpeed0 = m_moveParm.maxSpeed[0].x;
			static float  maxSpeed1 = m_moveParm.maxSpeed[1].x;
			ImGui::SliderFloat("AimModeMaxSpeed0", &maxSpeed0, 0.0f, 100.0f);

			m_aimMode.aimMoveParm.maxSpeed[0] = { maxSpeed0 ,0.0f,maxSpeed0 };
		}

		{
			static float decleleration = m_moveParm.decleleration;

			ImGui::SliderFloat("AimModeDecleleration", &decleleration, 0.0f, 1.0f);

			m_aimMode.aimMoveParm.decleleration = decleleration;
		}


		{
			float velocity[] = { m_aimMode.aimMoveParm.velocity.x,m_aimMode.aimMoveParm.velocity.y,m_aimMode.aimMoveParm.velocity.z };
			ImGui::SliderFloat3("AimModeVelocity", velocity, -100.0f, 100.0f);

		}
		static float  speed = m_moveParm.speed.x;
		ImGui::Text("AimModeSpeed : %f", &speed);


	}

#endif
	ImGui::End();

#endif
}

