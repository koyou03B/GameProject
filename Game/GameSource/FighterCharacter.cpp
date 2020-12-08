#include "FighterCharacter.h"
#include "MessengTo.h"
#include ".\LibrarySource\ModelData.h"
#include ".\LibrarySource\VectorCombo.h"

#ifdef _DEBUG
#include "..\External_libraries\imgui\imgui.h"
#include "..\External_libraries\imgui\imgui_impl_dx11.h"
#include "..\External_libraries\imgui\imgui_impl_win32.h"
#include "..\External_libraries\imgui\imgui_internal.h"
#endif
CEREAL_CLASS_VERSION(Fighter, 12);
void Fighter::Init()
{
	m_transformParm.position = { 0.0f,0.0f,-30.0f };
	m_transformParm.angle = { 0.0f * 0.01745f, 0.0f * 0.01745f,0.0f * 0.017454f };
	m_transformParm.scale = { 0.005f,0.005f,0.005f };
	m_transformParm.WorldUpdate();

//	m_attackParm.resize(5);
//	m_collision.resize(5);
	m_effect.resize(4);
	m_model = Source::ModelData::fbxLoader().GetActorModel(Source::ModelData::ActorModel::Fighter);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Players/Fighter/Run.fbx", 60);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Players/Fighter/Dive.fbx", 60);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Players/Fighter/LeftKick.fbx", 60);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Players/Fighter/LeftDushKick.fbx", 60);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Players/Fighter/RightKick.fbx", 60);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Players/Fighter/LeftRollKick.fbx", 60);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Players/Fighter/RightRollKick.fbx", 60);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Players/Fighter/RightPunch.fbx", 60);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Players/Fighter/RightFlyKick.fbx", 60);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Players/Fighter/HitReaction.fbx", 60);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Players/Fighter/HitBigReaction.fbx", 60);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Players/Fighter/Death.fbx", 60);
	//Source::ModelData::fbxLoader().SaveActForBinary(Source::ModelData::ActorModel::Fighter);

	m_blendAnimation.animationBlend.Init(m_model);
	m_blendAnimation.animationBlend.ChangeSampler(0, Fighter::Animation::IDLE, m_model);

	m_padDeadLine = 5000.0f;
	m_statusParm.isExit = true;
	m_changeParm.isPlay = true;
	SerialVersionUpdate(9);

	m_attackParm.resize(7);
	for (auto& atk : m_attackParm)
	{
		atk.serialVersion = 17;
	}

	if (PathFileExistsA((std::string("../Asset/Binary/Player/Fighter/Parameter") + ".bin").c_str()))
	{
		std::ifstream ifs;
		ifs.open((std::string("../Asset/Binary/Player/Fighter/Parameter") + ".bin").c_str(), std::ios::binary);
		cereal::BinaryInputArchive i_archive(ifs);
		i_archive(*this);
	}
//	m_attackParm.resize(7);
	m_statusParm.life = 100;
	m_statusParm.maxLife = 100;
	m_stepParm.maxSpeed = m_stepParm.speed;
	m_blendAnimation.blendRatioMax = 1.0f;
	m_blendAnimation.samplerSize = 2;
	m_adjustAnimation = false;
	m_moveParm.velocity = {};

	VECTOR3F scale = { 0.8f,0.8f,0.8f };
	VECTOR3F bonePosition = { 0,0,0 };

	for (auto& attack : m_attackParm)
	{
		attack.maxSpeed = attack.speed;
	}
	m_damageParm.maxSpeed = m_damageParm.speed;
	m_blendAnimation.blendRatio = m_blendAnimation.idleBlendRtio;
}

void Fighter::Update(float& elapsedTime)
{
	m_elapsedTime = elapsedTime;
	if (m_changeParm.isPlay)
	{
		m_input = PAD.GetPad(0);

		if (m_input != nullptr)
		{
			if (!KnockBack())
			{
				if (!m_adjustAnimation)
				{	
					Move(m_elapsedTime);

					Step(m_elapsedTime);

					Attack(m_elapsedTime);
				}
			}
		}
	}

	RestAnimationIdle();

    m_blendAnimation.animationBlend.Update(m_model, elapsedTime);
	FLOAT4X4 blendBone = m_blendAnimation.animationBlend._blendLocals[m_collision[0].GetCurrentMesh(0)].at(m_collision[0].GetCurrentBone(0));
	FLOAT4X4 modelAxisTransform = m_model->_resource->axisSystemTransform;
	FLOAT4X4 getBone = blendBone * modelAxisTransform * m_transformParm.world;

	float bonePositions[] = { getBone._41,getBone._42,getBone._43 };
	m_collision[0].position[0] = { m_transformParm.position.x,getBone._42,m_transformParm.position.z };

	if (m_statusParm.isExit == false)
		MESSENGER.MessageFromPlayer(m_id, MessengType::TELL_DEAD);
}

void Fighter::Render(ID3D11DeviceContext* immediateContext)
{
	auto& localTransforms = m_blendAnimation.animationBlend._blendLocals;
	VECTOR4F color{ 1.0f,1.0f,1.0f,1.0f };


	m_model->Render(immediateContext, m_transformParm.world, color, localTransforms);

	VECTOR4F scroll{ 0.0f, 0.0f, 0.0f, 0.0f };
	m_debugObjects.debugObject.Render(immediateContext, scroll,true);


}

//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
//IDLE WALK RUN : animtionBlend
//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
void Fighter::Move(float& elapsedTime)
{
	if (m_stepParm.isStep || m_statusParm.isAttack ) return;
	if (m_attackType == AttackType::LeftKick || m_attackType == AttackType::LeftDushKick) return;
	if (m_input->StickDeadzoneLX(m_padDeadLine) || m_input->StickDeadzoneLY(m_padDeadLine))
	{
		if (!m_moveParm.isMove)
		{
		//	m_blendAnimation.animationBlend.AddSampler(Animation::IDLE, m_model);
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

		VECTOR3F stickVector = {};
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

			//*-*-*-*-*-*-*-*-*-*-*-*-
			//If it's over max speed.
			if (speed > m_moveParm.maxSpeed[1].x)
				m_moveParm.velocity = vector * m_moveParm.maxSpeed[1].x;
			else
				m_moveParm.velocity -= vector * m_moveParm.decleleration;

			//*-*-*-*-*-*-*-*-*-*-*-*-*-*-
			//Calculation of Blend Value
			//*-*-*-*-*-*-*-*-*-*-*-*-*-*-
			if (!m_moveParm.isRun)
			{
				m_blendAnimation.animationBlend._blendRatio += m_blendAnimation.moveBlendRatio;
				if (m_blendAnimation.animationBlend._blendRatio >= 1.0f)
				{
					m_blendAnimation.animationBlend._blendRatio = 1.0f;
					m_moveParm.isRun = true;
				}
			}

			m_transformParm.position += m_moveParm.velocity * elapsedTime;


			VECTOR3F angle = m_transformParm.angle;

			float dx = sinf(angle.y);
			float dz = cosf(angle.y);

			float dot = (vector.x * dx) + (vector.z * dz);
			float rot = 1.0f - dot;

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
			m_moveParm.velocity = {};
	}
	else
	{
		m_moveParm.speed = {};
		if (m_moveParm.isMove)
		{		
			m_moveParm.isRun = false;

			auto& samplers = m_blendAnimation.animationBlend.GetSampler();
			if (samplers.at(0).first != Animation::IDLE)
			{
				m_blendAnimation.animationBlend.ResetAnimationSampler(0);
				m_blendAnimation.animationBlend.ChangeSampler(0, Animation::IDLE, m_model);
			}
			m_blendAnimation.animationBlend._blendRatio -= m_blendAnimation.idleBlendRtio;
			if (m_blendAnimation.animationBlend._blendRatio <= 0.0f)
			{
				m_blendAnimation.animationBlend._blendRatio = 0.0f;
				size_t samplerSize = samplers.size();
				for (int i = 1; i < static_cast<int>(samplerSize); ++i)
				{
					m_blendAnimation.animationBlend.ReleaseSampler(i);
				}
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
		m_blendAnimation.animationBlend.SetAnimationSpeed(1.6f);
		Stepping(elapsedTime);
	}

}

void Fighter::Stepping(float& elapsedTime)
{
	if (!m_stepParm.isStep)
	{
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
		
		m_stepParm.isStep = true;
		m_animationType = Fighter::Animation::DIVE;
		if (m_blendAnimation.animationBlend.SearchSampler(Animation::DIVE))
		{
			m_blendAnimation.animationBlend.ReleaseSampler(0);
		}
		m_blendAnimation.animationBlend.AddSampler(m_animationType, m_model);
		m_blendAnimation.animationBlend.ResetAnimationFrame();
		m_blendAnimation.animationBlend._blendRatio = 0.0f;
	}


	m_blendAnimation.animationBlend._blendRatio += m_blendAnimation.moveBlendRatio;
	if (m_blendAnimation.animationBlend._blendRatio >= m_blendAnimation.blendRatioMax)
	{
		m_blendAnimation.animationBlend._blendRatio = m_blendAnimation.blendRatioMax;
		int samplerCount = static_cast<int>(m_blendAnimation.animationBlend.GetSampler().size());

		for (int i = 0; i < samplerCount; ++i)
		{
			m_blendAnimation.animationBlend.ReleaseSampler(0);
		}
		m_blendAnimation.animationBlend.FalseAnimationLoop(0);
	}


	//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
	// Set the direction and move it.
	//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
	m_stepParm.speed -= m_stepParm.deceleration;
	if (m_stepParm.speed.x <= 0.0f)
		m_stepParm.speed = { 0.0f,0.0f,0.0f };

	m_moveParm.velocity.x = sinf(m_transformParm.angle.y) * (m_stepParm.speed.x);
	m_moveParm.velocity.y = 0.0f;
	m_moveParm.velocity.z = cosf(m_transformParm.angle.y) * (m_stepParm.speed.z);

	m_transformParm.position += m_moveParm.velocity * elapsedTime;
	m_transformParm.WorldUpdate();

	//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
	//When the animation ends, the attack ends.
	//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
	static bool isIdle = false;
	if (m_blendAnimation.animationBlend.GetSampler().size() != 1) return;
	uint32_t  currentAnimationFrame = m_blendAnimation.animationBlend.GetAnimationTime(0);

	if (currentAnimationFrame < m_stepParm.frameCount && currentAnimationFrame > 60  && !isIdle)
	{
		if (m_input->GetButtons(XINPUT_GAMEPAD_BUTTONS::PAD_A) == 1)
		{
			m_stepParm.isStep = true;
			isIdle = true;
			m_blendAnimation.animationBlend.TrueAnimationLoop(0);
		}
	}
	else if (currentAnimationFrame == m_stepParm.frameCount && isIdle)
	{
		m_blendAnimation.animationBlend.ResetAnimationSampler(0);
		m_stepParm.speed = m_stepParm.maxSpeed;
		isIdle = false;
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
	else if (currentAnimationFrame == m_stepParm.frameCount && !isIdle)
	{
		if (m_input->StickDeadzoneLX(m_padDeadLine) || m_input->StickDeadzoneLY(m_padDeadLine))
		{
			m_blendAnimation.animationBlend._blendRatio = 0.15f;
			m_stepParm.isStep = false;

			m_blendAnimation.animationBlend.SetAnimationSpeed(1.0f);
			if (m_moveParm.isMove)
			{
				m_moveParm.isMove = false;
				m_moveParm.isRun = false;
			}
		}
		else
		{
			m_animationType = Fighter::Animation::IDLE;
			m_blendAnimation.animationBlend.AddSampler(m_animationType, m_model);
			m_blendAnimation.animationBlend._blendRatio = 0.1f;
			m_blendAnimation.animationBlend.ResetAnimationFrame();
			m_adjustAnimation = true;
		}
		m_stepParm.speed = m_stepParm.maxSpeed;
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
	if (m_input->GetButtons(XINPUT_GAMEPAD_BUTTONS::PAD_X) == 1 && !m_statusParm.isAttack && !m_stepParm.isStep)
	{
		if (m_attackType != Fighter::AttackType::LeftDushKick)
		{
			m_animationType = Fighter::Animation::LEFT_KICK;
			m_attackType = Fighter::AttackType::LeftKick;
			if (m_moveParm.isMove)
			{
				m_moveParm.isMove = false;
				m_moveParm.isRun = false;
			}
		}
	}
	else if (m_moveParm.isRun && m_input->GetButtons(XINPUT_GAMEPAD_BUTTONS::PAD_Y) == 1 &&
		!m_statusParm.isAttack && !m_stepParm.isStep)
	{
		m_animationType = Fighter::Animation::LEFT_DUSH_KICK;
		m_attackType = Fighter::AttackType::LeftDushKick;
		if (m_moveParm.isMove)
		{
			m_moveParm.isMove = false;
			m_moveParm.isRun = false;
		}
		m_attackParm[1].speed = m_attackParm[1].maxSpeed;

	}

	switch (m_animationType)
	{
	case Fighter::Animation::LEFT_KICK:
		{
			Animation nextaAnimtion = { Animation::RIGHT_KICK };
			Attacking(m_animationType, nextaAnimtion, m_attackParm[0], m_collision[1]);

		}
		break;
	case Fighter::Animation::LEFT_DUSH_KICK:
	{
		Animation nextaAnimtion = { Animation::RIGHT_KICK };
		Attacking(m_animationType, nextaAnimtion, m_attackParm[1], m_collision[1]);

		//m_attackParm[1].speed -= m_attackParm[1].deceleration;
		//if (m_attackParm[1].speed.x <= 0.0f)
		//	m_attackParm[1].speed = { 0.0f,0.0f,0.0f };



		//m_moveParm.velocity.x = sinf(m_transformParm.angle.y) * (m_attackParm[1].speed.x);
		//m_moveParm.velocity.y = 0.0f;
		//m_moveParm.velocity.z = cosf(m_transformParm.angle.y) * (m_attackParm[1].speed.z);

		//m_transformParm.position += m_moveParm.velocity * elapsedTime;
		//m_transformParm.WorldUpdate();
	}
	break;
	case Fighter::RIGHT_KICK:
	{
	//	m_blendAnimation.animationBlend.SetAnimationSpeed(1.4f);
		Animation nextaAnimtions = {Animation::LEFT_ROLL_KICK };
		m_attackType = AttackType::RightKick;
		Attacking(m_animationType, nextaAnimtions, m_attackParm[2], m_collision[2]);
	}
		break;
	case Fighter::LEFT_ROLL_KICK:
	{
	//	m_blendAnimation.animationBlend.SetAnimationSpeed(1.4f);
		Animation nextaAnimtions = { Animation::RIGHT_ROLL_KICK };
		m_attackType = AttackType::LeftRollKick;
		Attacking(m_animationType, nextaAnimtions, m_attackParm[3], m_collision[2]);
	}
		break;
	case Fighter::RIGHT_ROLL_KICK:
	{
	//	m_blendAnimation.animationBlend.SetAnimationSpeed(1.2f);
		Animation nextaAnimtions = { Animation::RIGHT_FLY_KICK };
		m_attackType = AttackType::RightRollKick;
		Attacking(m_animationType, nextaAnimtions, m_attackParm[4], m_collision[2]);
	}
		break;
	case Fighter::RIGHT_PUNCH:
	{
		//	m_blendAnimation.animationBlend.SetAnimationSpeed(1.2f);
		Animation nextaAnimtions = { Animation::RIGHT_FLY_KICK };
		m_attackType = AttackType::RightPunch;
		Attacking(m_animationType, nextaAnimtions, m_attackParm[5], m_collision[3]);
	}
	break;
	case Fighter::RIGHT_FLY_KICK:
	{
		//	m_blendAnimation.animationBlend.SetAnimationSpeed(1.2f);
		Animation nextaAnimtions = { Animation::RIGHT_FLY_KICK };
		m_attackType = AttackType::RightFlyKick;
		Attacking(m_animationType, nextaAnimtions, m_attackParm[6], m_collision[1]);
	}
	break;
	//default:
	//	break;
	}
}

void Fighter::Attacking(Animation currentAnimation, Animation nextAnimations,
	CharacterParameter::Attack& attack, CharacterParameter::Collision& collision)
{
#if 0	
	//*********************************
	//　向きの修正
	//*********************************

	auto& enemy = MESSENGER.CallEnemyInstance(0);
	VECTOR3F enemyPos = enemy->GetWorldTransform().position;

	float direction = ToDistVec3(enemyPos - m_transformParm.position);
	if (direction < 18.0f)
	{
		VECTOR3F nDirection = NormalizeVec3(enemyPos - m_transformParm.position);
		VECTOR3F nForward = NormalizeVec3(VECTOR3F(sinf(m_transformParm.angle.y), 0.0f, cosf(m_transformParm.angle.y)));

		float dot = DotVec3(nDirection, nForward);
		float cosTheta = acosf(dot);
		if (cosTheta >= 0.4f && dot > -0.1f)
		{
			float rot = 1.0f - dot;

			float limit = m_moveParm.turnSpeed;

			if (rot > limit)
			{
				rot = limit;
			}

			VECTOR3F cross = CrossVec3(nForward, nDirection);
			if (cross.y > 0.0f)
			{
				m_transformParm.angle.y += rot;
			}
			else
			{
				m_transformParm.angle.y -= rot;
			}
			m_transformParm.WorldUpdate();
			return;
		}
		m_moveParm.velocity = {};
	}
#endif

	size_t samplerSize = m_blendAnimation.animationBlend.GetSampler().size();

	//*-*-*-*-*-*-*-*-*-*-*-*-*-
	// Reset the first time.
	//*-*-*-*-*-*-*-*-*-*-*-*-*-
	if (!m_statusParm.isAttack)
	{
		if (m_blendAnimation.animationBlend.GetSampler().at(0).first != Animation::RUN)
			m_blendAnimation.animationBlend.ReleaseSampler(0);

		m_blendAnimation.animationBlend.AddSampler(m_animationType, m_model);
		size_t samplerSize = m_blendAnimation.animationBlend.GetSampler().size();
		m_blendAnimation.animationBlend.ResetAnimationSampler(static_cast<int>(samplerSize) - 1);
		m_blendAnimation.animationBlend.ResetAnimationFrame();
		if (samplerSize != 2)
			m_blendAnimation.animationBlend._blendRatio = 0.66f;
		else
			m_blendAnimation.animationBlend._blendRatio = 0.0f;
	
		m_moveParm.velocity = GetInputDirection();
		m_statusParm.isAttack = true;
	}

	//*********************************
	//　移動処理
	//*********************************
	if (m_attackType == AttackType::LeftDushKick)
	{
		attack.speed -= attack.deceleration;
		if (attack.speed.x <= 0.0f)
			attack.speed = { 0.0f,0.0f,0.0f };

		VECTOR3F velocity = m_moveParm.velocity;
		velocity += velocity * attack.speed;
		m_transformParm.position += velocity * m_elapsedTime;
		m_transformParm.position.y = 0.0f;
		m_transformParm.WorldUpdate();
	}
	else
	{
		if (samplerSize == 1)
		{
			uint32_t  currentAnimationFrame = m_blendAnimation.animationBlend.GetAnimationTime(0);
			if (attack.moveFrameStart <= currentAnimationFrame)
			{
				attack.speed -= attack.deceleration;
				if (attack.speed.x <= 0.0f)
					attack.speed = { 0.0f,0.0f,0.0f };

				VECTOR3F velocity = m_moveParm.velocity;
				velocity += velocity * attack.speed;
				m_transformParm.position += velocity * m_elapsedTime;
				m_transformParm.position.y = 0.0f;
				m_transformParm.WorldUpdate();
			}
		}
	}
	//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
	// Set the blend ratio to 1.
	// When we get to 1, we stop blending.
	//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
	if (samplerSize >= 2)
	{
		{
			m_blendAnimation.animationBlend._blendRatio += m_blendAnimation.attackBlendRtio;
			if (m_blendAnimation.animationBlend._blendRatio >= m_blendAnimation.blendRatioMax)
			{
				m_blendAnimation.animationBlend._blendRatio = 0.0f;
				int samplerCount = static_cast<int>(m_blendAnimation.animationBlend.GetSampler().size());

				for (int i = 0; i < samplerCount; ++i)
				{
					m_blendAnimation.animationBlend.ReleaseSampler(0);
				}
				m_blendAnimation.animationBlend.FalseAnimationLoop(0);
			}
		}

		{	
			VECTOR2F vector = { m_moveParm.velocity.x, m_moveParm.velocity.z };
			m_transformParm.angle = GetRotationAfterAngle(vector,m_moveParm.turnSpeed);
			m_transformParm.WorldUpdate();
		}
		return;
	}

	//*********************************************
	// Collsion
	//*********************************************
	uint32_t  currentAnimationFrame = m_blendAnimation.animationBlend.GetAnimationTime(0);
	if (attack.attackTimerRange[0] < currentAnimationFrame && currentAnimationFrame < attack.attackTimerRange[1])
	{
		FLOAT4X4 blendBone = m_blendAnimation.animationBlend._blendLocals[collision.GetCurrentMesh(0)].at(collision.GetCurrentBone(0));
		FLOAT4X4 modelAxisTransform = m_model->_resource->axisSystemTransform;
		FLOAT4X4 getBone = blendBone * modelAxisTransform * m_transformParm.world;

		float bonePositions[] = { getBone._41,getBone._42,getBone._43 };
		collision.position[0] = { bonePositions[0],bonePositions[1],bonePositions[2] };
		m_statusParm.attackPoint = attack.attackPoint;

		if (!attack.hasAttacked && MESSENGER.AttackingMessage(static_cast<int>(m_id), collision))
		{
			attack.hasAttacked = true;
			//当たった演出発動
		}
	}

	//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
	// If the button is pressed within the attack
	// change range, the attack is changed
	//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
	if (attack.inputRange[0] < currentAnimationFrame && currentAnimationFrame < attack.inputRange[1])
	{
		size_t attackButton = attack.buttons.size();
		if (static_cast<int>(attackButton) == 1 && m_input->GetButtons(attack.buttons[0]) == 1)
		{
			m_blendAnimation.animationBlend.ResetAnimationFrame();
			m_blendAnimation.animationBlend.AddSampler(nextAnimations, m_model);
			m_blendAnimation.animationBlend.ResetAnimationSampler(1);
			m_animationType = nextAnimations;
			m_blendAnimation.animationBlend._blendRatio = {};
			attack.hasAttacked = false;
			attack.speed = attack.maxSpeed;
			m_moveParm.velocity = GetInputDirection();
			return;
		}
		else if (m_input->GetButtons(XINPUT_GAMEPAD_BUTTONS::PAD_A) == 1)
		{
			m_blendAnimation.animationBlend.ResetAnimationSampler(0);
			m_blendAnimation.animationBlend.ResetAnimationFrame();
			m_statusParm.isAttack = false;
			attack.hasAttacked = false;
			attack.speed = attack.maxSpeed;

			m_animationType = Fighter::Animation::DIVE;
			m_attackType = Fighter::AttackType::NON;
			Stepping(m_elapsedTime);
			return;
		}
	}

//	m_transformParm.WorldUpdate();

	if (attack.hasAttacked)
	{
		if (currentAnimationFrame > attack.slowTimeFrameCount[0] && currentAnimationFrame < attack.slowTimeFrameCount[1])
			m_blendAnimation.animationBlend.SetAnimationSpeed(attack.attackSpeed[1]);
		else
			m_blendAnimation.animationBlend.SetAnimationSpeed(attack.attackSpeed[0]);

		if (m_attackType == AttackType::LeftDushKick)
			attack.speed = { 5.0f,0.0f,5.0f, };
	}
	else
		m_blendAnimation.animationBlend.SetAnimationSpeed(attack.attackSpeed[0]);

	//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
	//When the animation ends, the attack ends.
	//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
	if (currentAnimationFrame == attack.frameCount)
	{
		if (m_input->StickDeadzoneLX(m_padDeadLine) || m_input->StickDeadzoneLY(m_padDeadLine))
		{
			m_statusParm.isAttack = false;
			m_blendAnimation.animationBlend.SetAnimationSpeed(1.0f);
			m_animationType = Fighter::Animation::IDLE;
		}
		else
		{
			m_animationType = Fighter::Animation::IDLE;
			m_blendAnimation.animationBlend.AddSampler(m_animationType, m_model);
			m_adjustAnimation = true;
		}
		m_attackType = Fighter::AttackType::NON;
		m_blendAnimation.animationBlend._blendRatio = 0.0f;
		m_blendAnimation.animationBlend.ResetAnimationFrame();
		attack.hasAttacked = false;
		m_moveParm.velocity = {};
		attack.speed = attack.maxSpeed;

	}
}

void Fighter::RestAnimationIdle()
{

	if (m_adjustAnimation && !m_statusParm.isDamage)
	{
		if (m_blendAnimation.animationBlend.GetSampler()[1].first == Animation::IDLE)
		{
			m_blendAnimation.animationBlend._blendRatio += m_blendAnimation.blendRatio;
			if (m_blendAnimation.animationBlend._blendRatio >= 1.0f)
			{
				m_blendAnimation.animationBlend.ReleaseSampler(0);
				m_statusParm.isAttack = false;
				m_stepParm.isStep = false;
				m_moveParm.isMove = false;
				m_moveParm.isRun = false;
				m_statusParm.isDamage = false;
				m_adjustAnimation = false;
				m_damageParm.hasBigDamaged = false;
				m_blendAnimation.animationBlend._blendRatio = 0.0f;
				m_stepParm.speed = m_stepParm.maxSpeed;
				m_damageParm.speed = m_damageParm.maxSpeed;
				m_attackType = AttackType::NON;
				m_moveParm.velocity = {};
				m_blendAnimation.blendRatio = m_blendAnimation.idleBlendRtio;
				m_blendAnimation.animationBlend.SetAnimationSpeed(1.0f);
			}
		}
	}

}

void Fighter::Impact()
{
	if (m_blendAnimation.animationBlend.SearchSampler(Animation::HIT_REACTION)) return;
	if (m_blendAnimation.animationBlend.SearchSampler(Animation::HIT_BIG_REACTION)) return;

	if (m_statusParm.life > 0)
	{
		if (m_damageParm.hasBigDamaged)
		{
			m_blendAnimation.animationBlend.AddSampler(Animation::HIT_BIG_REACTION, m_model);
			m_input->SetVibrationParm(UVECTOR2(65000, 65000), 23);
		}
		else
		{
			m_blendAnimation.animationBlend.AddSampler(Animation::HIT_REACTION, m_model);
			m_input->SetVibrationParm(UVECTOR2(65000, 65000), 10);
		}
	}
	else
	{
		m_blendAnimation.animationBlend.AddSampler(Animation::DEATH, m_model);
	}

	if (m_adjustAnimation)
		m_adjustAnimation = !m_adjustAnimation;

	m_statusParm.isDamage = true;
	float dot = atan2f(m_damageParm.vector.x, m_damageParm.vector.z);
	m_transformParm.angle.y = dot;
	m_transformParm.WorldUpdate();

	m_blendAnimation.animationBlend.SetAnimationSpeed(1.3f);
	KnockBack();
}

VECTOR3F Fighter::GetInputDirection()
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
		VECTOR3F stickVec(m_input->StickVectorLeft().x, 0.0f, m_input->StickVectorLeft().y);
		DirectX::XMVECTOR vStickVex = DirectX::XMLoadFloat3(&stickVec);

		VECTOR3F stickVector = {};
		vStickVex = DirectX::XMVector4Transform(vStickVex, viewMatrix);
		DirectX::XMStoreFloat3(&stickVector, vStickVex);
		stickVector = NormalizeVec3(stickVector);

		auto& enemy = MESSENGER.CallEnemyInstance(0);
		VECTOR3F enemyPos = enemy->GetWorldTransform().position;

		float direction = ToDistVec3(enemyPos - m_transformParm.position);
		if (direction < 20.0f)
		{
			//VECTOR3F nDirection = NormalizeVec3(enemyPos - m_transformParm.position);
			//FLOAT4X4 world = m_transformParm.world;
			//VECTOR3F front = { world._31,world._32,world._33 };
			//front = NormalizeVec3(front);
			//float dot = DotVec3(nDirection, stickVector);
			//float cosTheta = acosf(dot);
			//if (cosTheta >= 0.1f)
			//{
			//	float angle = m_transformParm.angle.y;
			//	VECTOR3F eDirection = nDirection;
			//	VECTOR3F sDirection = stickVector;

			//	stickVector = SphereLinearVec3(eDirection,sDirection , 0.25f);
			//}
		}
		return stickVector;
	}

	float angle = m_transformParm.angle.y;
	return { sinf(angle),0.0f,cosf(angle)};
}

VECTOR3F Fighter::GetRotationAfterAngle(VECTOR2F vector,float turnSpeed)
{
	VECTOR3F angle = m_transformParm.angle;

	float dx = sinf(angle.y);
	float dz = cosf(angle.y);
	float dot = (vector.x * dx) + (vector.y * dz);
	float rot = 1.0f - dot;

	float limit = turnSpeed;

	if (rot > limit)
		rot = limit;
	

	float cross = (vector.x * dz) - (vector.y * dx);
	if (cross > 0.0f)
	{
		angle.y += rot;
	}
	else
	{
		angle.y -= rot;
	}

	return angle;
}

bool Fighter::KnockBack()
{
	if (!m_statusParm.isDamage || m_adjustAnimation)
		return false;

	int samplerCount = static_cast<int>(m_blendAnimation.animationBlend.GetSampler().size());
	if (samplerCount != 1)
	{
		m_blendAnimation.animationBlend._blendRatio += m_blendAnimation.idleBlendRtio;
		if (m_blendAnimation.animationBlend._blendRatio >= m_blendAnimation.blendRatioMax)
		{
			m_blendAnimation.animationBlend._blendRatio = 0.0f;

			for (int i = 0; i < samplerCount - 1; ++i)
			{
				m_blendAnimation.animationBlend.ResetAnimationSampler(0);
				m_blendAnimation.animationBlend.ReleaseSampler(0);
			}

			m_blendAnimation.animationBlend.FalseAnimationLoop(0);
		}

		m_moveParm.isMove = false;
		m_moveParm.isWalk = false;
		m_moveParm.isRun = false;
		m_stepParm.isStep = false;

		for (auto& attack : m_attackParm)
		{
			attack.hasAttacked = false;
		}
	}
	else
	{
		if (m_statusParm.life > 0)
		{
			uint32_t animationEnd = m_damageParm.hasBigDamaged ? 131 : 79;

			uint32_t  currentAnimationFrame = m_blendAnimation.animationBlend.GetAnimationTime(0);

			//if(currentAnimationFrame >= 80)
			//	m_blendAnimation.animationBlend.SetAnimationSpeed(f);

			if (currentAnimationFrame == animationEnd)
			{
				m_blendAnimation.animationBlend._blendRatio = 0.0f;
				m_blendAnimation.animationBlend.ResetAnimationFrame();
				m_blendAnimation.animationBlend.SetAnimationSpeed(1.0f);
				m_animationType = Animation::IDLE;
				m_blendAnimation.animationBlend.AddSampler(m_animationType, m_model);
				m_blendAnimation.blendRatio = m_blendAnimation.damageBlendRatio;
				m_adjustAnimation = true;
				m_statusParm.isDamage = false;
			}
		}
		else
		{
			m_blendAnimation.animationBlend._blendRatio = 0.0f;
			m_blendAnimation.animationBlend.ResetAnimationFrame();
			m_blendAnimation.animationBlend.SetAnimationSpeed(1.0f);
		}
	}

	if (m_damageParm.hasBigDamaged)
	{
		m_damageParm.speed -= m_damageParm.deceleration;
		if (m_damageParm.speed.x <= 0.0f)
			m_damageParm.speed = { 0.0f,0.0f,0.0f };

		m_moveParm.velocity.x = sinf(m_transformParm.angle.y) * (m_damageParm.speed.x) * -1.0f;
		m_moveParm.velocity.y = 0.0f;
		m_moveParm.velocity.z = cosf(m_transformParm.angle.y) * (m_damageParm.speed.z) * -1.0f;

		m_transformParm.position += m_moveParm.velocity * m_elapsedTime;
		m_transformParm.WorldUpdate();
	}

	return true;

}

void Fighter::ChangeCharacter()
{
	if (m_input->GetButtons(XINPUT_GAMEPAD_BUTTONS::PAD_RIGHT) == 1)
	{
		m_changeParm.changeType = CharacterParameter::Change::PlayerType::ARCHER;

		MESSENGER.MessageFromPlayer(m_id, MessengType::CHANGE_PLAYER);
		m_input->ResetButton(XINPUT_GAMEPAD_BUTTONS::PAD_RIGHT);
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
	

	static int currentBone = 0;

	ImGui::Combo("Name_of_BoneName",
		&currentBone,
		vectorGetter,
		static_cast<void*>(&m_blendAnimation.animationBlend.GetBoneName()[currentMesh]),
		static_cast<int>(m_blendAnimation.animationBlend.GetBoneName()[currentMesh].size())
	);

	//**************************************
	// Collision
	//**************************************
	if (ImGui::CollapsingHeader("Collision"))
	{
		static int current = 0;
		ImGui::RadioButton("Body", &current, 0);
		ImGui::RadioButton("RightPunch", &current, 1); ImGui::SameLine();
		ImGui::RadioButton("LeftPunch", &current, 2);
		ImGui::RadioButton("RightKick", &current, 3); ImGui::SameLine();
		ImGui::RadioButton("LeftKick", &current, 4);

		FLOAT4X4 blendBone = m_blendAnimation.animationBlend._blendLocals[currentMesh].at(currentBone);
		FLOAT4X4 modelAxisTransform = m_model->_resource->axisSystemTransform;
		FLOAT4X4 getBone = blendBone * modelAxisTransform * m_transformParm.world;

		float bonePositions[] = { getBone._41,getBone._42,getBone._43 };
		VECTOR3F bonePosition = { bonePositions[0],bonePositions[1],bonePositions[2] };

		ImGui::SliderFloat3("BoneTranslate", bonePositions, -10.0f, 10.0f);

		static bool isVisualization = false;
		if (ImGui::Button("Visible?"))
			isVisualization = true;

		if (isVisualization)
		{
			if (current != 0)
			{
				auto primitive = m_debugObjects.GetSphere(device, "");
				m_debugObjects.debugObject.AddGeometricPrimitive(std::move(primitive));
			}
			else
			{
				auto primitive = m_debugObjects.GetCapsule(device, "");
				m_debugObjects.debugObject.AddGeometricPrimitive(std::move(primitive));
			}
			m_debugObjects.debugObject.AddInstanceData(bonePosition, VECTOR3F(0.0f * 0.01745f, 180.0f * 0.01745f, 0.0f * 0.017454f),
				VECTOR3F(1.0f, 1.0f, 1.0f), VECTOR4F(1.0f, 1.0f, 1.0f, 1.0f));

			isVisualization = false;
		}

		if (m_debugObjects.debugObject.IsGeomety())
		{


			if (m_debugObjects.debugObject.GetInstance().size() > current)
			{
				auto& geomtry = m_debugObjects.debugObject.GetInstanceData(current);

				//Position
				{
					geomtry.position = bonePosition;
				}
				//Scale
				{
					static float scale = m_collision[current].scale;
					ImGui::SliderFloat("Scale", &scale, 1.0f, 10.0f);
					if (ImGui::Button("ScaleChange"))
					{
						geomtry.scale = { scale ,scale ,scale };
						m_collision[current].scale = scale;
					}
				}

				//Radius
				{
					static float radius = m_collision[current].radius;
					ImGui::SliderFloat("Radius", &radius, 1, 10);
					if (ImGui::Button("RadiusChange"))
					{
						geomtry.scale = { 1.0f,1.0f,1.0f };
						geomtry.scale *= radius;
						m_collision[current].radius = radius;
					}
				}

				//Mesh & Bone SetNumber
				{
					if (ImGui::Button("Mesh 0"))
						m_collision[current].SetCurrentMesh(currentMesh,0);
					ImGui::SameLine();
					if (ImGui::Button("Mesh 1"))
						m_collision[current].SetCurrentMesh(currentMesh, 0);

					int mesh0 = m_collision[current].GetCurrentMesh(0);
					ImGui::BulletText("Mesh Number %d", mesh0);
					int mesh1 = m_collision[current].GetCurrentMesh(1);
					ImGui::BulletText("Mesh Number %d", mesh1);

					if (ImGui::Button("Bone 0"))
						m_collision[current].SetCurrentBone(currentBone, 0);
					ImGui::SameLine();
					if (ImGui::Button("Bone 1"))
						m_collision[current].SetCurrentBone(currentBone, 1);


					int bone0 = m_collision[current].GetCurrentBone(0);
					ImGui::BulletText("Bone Number %d", bone0);
					int bone1 = m_collision[current].GetCurrentBone(1);
					ImGui::BulletText("Bone Number %d", bone1);
				}

				//CollisionType
				{
					std::vector<std::string> collisionType = { "Cube","SPHER","CAPSULE","CIRCLE","RECT" };
					static int currentCollisionType = 0;

					ImGui::Combo("Name_of_CollisionType",
						&currentCollisionType,
						vectorGetter,
						static_cast<void*>(&collisionType),
						static_cast<int>(collisionType.size())
					);

					if (ImGui::Button("CollisionType"))
						m_collision[current].collisionType = static_cast<CharacterParameter::Collision::CollisionType>(currentCollisionType);
				}
				geomtry.CreateWorld();
			}
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

			
			ImGui::SliderFloat("DamageBlendRatio", &m_blendAnimation.damageBlendRatio, 0.0f, 1.0f);

		}

		{
			static float blendValue0 = 0.7f;
			ImGui::SliderFloat("WalkBlendValue", &blendValue0, 0.0f, 1.0f);
			if (ImGui::Button("Set WalkBlendValue"))
				m_blendAnimation.blendValueRange[0] = blendValue0;

			static float blendValue1 = 1.0f;
			ImGui::SliderFloat("RunBlendValue", &blendValue1, 0.0f, 1.0f);
			if (ImGui::Button("Set RunBlendValue"))
				m_blendAnimation.blendValueRange[1] = blendValue1;

			float animationSpeed = m_blendAnimation.animationBlend._animationSpeed;
			ImGui::SliderFloat("AnimationSpeed", &animationSpeed, -1.0f, 2.0f);
			m_blendAnimation.animationBlend._animationSpeed = animationSpeed;
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

		float position[] = { m_transformParm.position.x,m_transformParm.position.y,m_transformParm.position.z };
		ImGui::SliderFloat3("Position", position, -2000.0f, 2000.0f);

		float dist = ToDistVec3(m_transformParm.position);
		ImGui::SliderFloat("Dist", &dist, -2000.0f, 2000.0f);

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
		m_stepParm.speed.x = m_stepParm.speed.z = speed;
		//float animationSpeed = m_blendAnimation.animtionSpeed;
		//ImGui::SliderFloat("AnimationSpeed", &animationSpeed, 1.0f, 5.0f);

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
	// Attack
	//**************************************
	if (ImGui::CollapsingHeader("Attack"))
	{
		static int current = 0;
		ImGui::RadioButton("LeftKick", &current, 0); ImGui::SameLine();
		ImGui::RadioButton("LeftDushKick", &current, 1); ImGui::SameLine();
		ImGui::RadioButton("RightKick", &current, 2);
		ImGui::RadioButton("LeftRollKick", &current, 3); ImGui::SameLine();
		ImGui::RadioButton("RightRollKick", &current, 4);  ImGui::SameLine();
		ImGui::RadioButton("RightPunch", &current, 5); ImGui::SameLine();
		ImGui::RadioButton("RightFlyKick", &current, 6);
		//FrameCount
		{
			int frameCount = m_attackParm[current].frameCount;
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
					"PAD_Y",
					"NON"
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
			int start = m_attackParm[current].inputRange[0];
			ImGui::InputInt("Start", &start, 0, 100);

			m_attackParm[current].inputRange[0] = start;

			int end = m_attackParm[current].inputRange[1];
			ImGui::InputInt("End", &end, 0, 100);

			m_attackParm[current].inputRange[1] = end;
		}

		//attackPoint
		{
			static float point = m_attackParm[current].attackPoint;
			ImGui::InputFloat("AttackPoint", &point, 0, 100);

			if (ImGui::Button("ADD AttackPoint"))
				m_attackParm[current].attackPoint = point;
		}

		//attackSpeed
		{
			float attackSpeedFast = m_attackParm[current].attackSpeed[0];
			ImGui::InputFloat("AttackSpeedFast", &attackSpeedFast, 1.0f, 2.0f);
			m_attackParm[current].attackSpeed[0] = attackSpeedFast;

			float attackSpeedSlow = m_attackParm[current].attackSpeed[1];
			ImGui::InputFloat("AttackSpeedSlow", &attackSpeedSlow, 1.0f, 2.0f);
			m_attackParm[current].attackSpeed[1] = attackSpeedSlow;
		}

		//attackTimer
		{
			int attackTimerStart = m_attackParm[current].attackTimerRange[0];
			ImGui::InputInt("AttackTimerStart", &attackTimerStart, 0, 100);
			m_attackParm[current].attackTimerRange[0] = attackTimerStart;

			int attackTimerEnd = m_attackParm[current].attackTimerRange[1];
			ImGui::InputInt("AttackTimerEnd", &attackTimerEnd, 0, 100);
			m_attackParm[current].attackTimerRange[1] = attackTimerEnd;
		}

		//SlowTime
		{
			int slowTimeStart = m_attackParm[current].slowTimeFrameCount[0];
			ImGui::InputInt("SlowTimeStart", &slowTimeStart, 0, 100);
			m_attackParm[current].slowTimeFrameCount[0] = slowTimeStart;

			int slowTimeEnd = m_attackParm[current].slowTimeFrameCount[1];
			ImGui::InputInt("SlowTimeEnd", &slowTimeEnd, 0, 100);
			m_attackParm[current].slowTimeFrameCount[1] = slowTimeEnd;
		}

		//moveFrame
		{
			int moveFrameStart = m_attackParm[current].moveFrameStart;
			ImGui::InputInt("MoveFrameStart", &moveFrameStart, 0, 100);
			m_attackParm[current].moveFrameStart = moveFrameStart;
		}

		{
			float deceleration = m_attackParm[current].deceleration.x;
			ImGui::SliderFloat("AttackDeceleration", &deceleration, 0.0f, 1.0f);
			m_attackParm[current].deceleration = VECTOR3F(deceleration, 0.0f, deceleration);

			float speed = m_attackParm[current].speed.x;
			ImGui::SliderFloat("AttackSpeed", &speed, 0.0f, 100.0f);
			m_attackParm[current].speed.x = m_attackParm[current].speed.z = speed;

			if (ImGui::Button("SetMaxSpeed"))
			{
				m_attackParm[current].maxSpeed = m_attackParm[current].speed;
			}
			static float currentSpeed = 0.f;
			if (ImGui::Button("SetCurrentAttackSpeed"))
			{
				currentSpeed = speed;
			}
			if (ImGui::Button("GetCurrentAttackSpeed"))
			{
				speed = currentSpeed;
			}
		}
	}

	//**************************************
	// Life
	//**************************************
	if (ImGui::CollapsingHeader("Life"))
	{
		ImGui::BulletText("LIFE : %f", m_statusParm.life);
		ImGui::BulletText("Damage : %d", m_statusParm.isDamage);
		if (ImGui::Button("No Damge"))
			m_statusParm.isDamage = false;
	}

	//**************************************
	// Life
	//**************************************
	if (ImGui::CollapsingHeader("Damage"))
	{
		float accel = m_damageParm.deceleration.x;
		ImGui::SliderFloat("AwayAccel", &accel, 0.0f, 1.0f);
		m_damageParm.deceleration.x = m_damageParm.deceleration.z = accel;
		float speed = m_damageParm.speed.x;
		ImGui::SliderFloat("AwaySpeed", &speed, 0.0f, 100.0f);
		m_damageParm.speed.x = m_damageParm.speed.z = speed;

		ImGui::SliderFloat("DamageComparison", &m_damageParm.hitComparison, 0.0f, 100.0f);
	}

	float scale = m_transformParm.scale.x;
	ImGui::SliderFloat("Scale", &scale, 0.001f, 0.005f);

	m_transformParm.scale = VECTOR3F(scale, scale, scale);
	m_transformParm.WorldUpdate();

	ImGui::End();
#endif
}