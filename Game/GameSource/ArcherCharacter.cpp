#include "ArcherCharacter.h"
#include "MessengTo.h"
#include "Arrow.h"
#include "Collision.h"
#include ".\LibrarySource\ModelData.h"
#include ".\LibrarySource\VectorCombo.h"


#ifdef _DEBUG
#include "..\External_libraries\imgui\imgui.h"
#include "..\External_libraries\imgui\imgui_impl_dx11.h"
#include "..\External_libraries\imgui\imgui_impl_win32.h"
#include "..\External_libraries\imgui\imgui_internal.h"
#endif
CEREAL_CLASS_VERSION(Archer, 1);

void Archer::Init()
{
	m_model = Source::ModelData::fbxLoader().GetActorModel(Source::ModelData::ActorModel::Archer);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Players/Archer/Run.fbx", 60);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Players/Archer/RunLeft.fbx", 60);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Players/Archer/RunRight.fbx", 60);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Players/Archer/Dive.fbx", 60);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Players/Archer/SetArrow.fbx", 60);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Players/Archer/Aim.fbx", 60);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Players/Archer/Shoot.fbx", 60);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Players/Archer/Hit.fbx", 60);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Players/Archer/HitBig.fbx", 60);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Players/Archer/Heal.fbx", 60);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Players/Archer/Revival.fbx", 60);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Players/Archer/Death.fbx", 60);
	//Source::ModelData::fbxLoader().SaveActForBinary(Source::ModelData::ActorModel::Archer);

	m_padDeadLine = 5000.0f;
	m_elapsedTime = 0.0f;

	m_transformParm.position = { 30.0f,0.0f,0.0f };
	m_transformParm.angle = { 0.0f * 0.01745f, 0.0f * 0.01745f,0.0f * 0.017454f };
	m_transformParm.scale = { 0.04f,0.04f,0.04f };
	m_transformParm.WorldUpdate();

	m_changeParm.isPlay = false;
	m_blendAnimation.animationBlend.Init(m_model);
	SerialVersionUpdate(0);

	if (PathFileExistsA((std::string("../Asset/Binary/Player/Archer/Parameter") + ".bin").c_str()))
	{
		std::ifstream ifs;
		ifs.open((std::string("../Asset/Binary/Player/Archer/Parameter") + ".bin").c_str(), std::ios::binary);
		cereal::BinaryInputArchive i_archive(ifs);
		i_archive(*this);
	}
	m_domain.AllSet(m_domainConverter);

	m_domain.GetPrimitiveTask(PrimitiveTaskType::FindAttackPoint)->SetOperator(&Archer::FindAttackPoint);
	m_domain.GetPrimitiveTask(PrimitiveTaskType::Move)->SetOperator(&Archer::MoveRun);
	m_domain.GetPrimitiveTask(PrimitiveTaskType::SetArrow)->SetOperator(&Archer::SetArrow);
	m_domain.GetPrimitiveTask(PrimitiveTaskType::ShootArrow)->SetOperator(&Archer::Shoot);
	m_domain.GetPrimitiveTask(PrimitiveTaskType::Avoiding)->SetOperator(&Archer::Avoid);
	m_domain.GetPrimitiveTask(PrimitiveTaskType::FindDirectionAvoid)->SetOperator(&Archer::FindDirectionToAvoid);

	m_statusParm.life = 12000;
	m_stepParm.maxSpeed = m_stepParm.speed;
	m_controlPoint.resize(8);
	m_canRun = true;

	auto& wepon = RunningMarket().FindProductConer(0);
	std::unique_ptr<Arrow> arrow{ wepon.GiveProduct<Arrow>() };
	m_arrow = std::move(arrow);
}

void Archer::Update(float& elapsedTime)
{
	m_elapsedTime = elapsedTime;

	//if (m_changeParm.isPlay)
	//{
	//	m_input = PAD.GetPad(0);
	//	if (m_input != nullptr)
	//	{
	//		Move(m_elapsedTime);
	//	}
	//}

	//if (m_changeParm.isPlay)
	//{
	//	m_input = PAD.GetPad(0);
	//	if (m_input != nullptr)
	//	{
	//		if (!KnockBack())
	//		{
	//			//***********************
	//			// Movement in each mode
	//			//***********************
	//			if (m_mode != Mode::Aiming)
	//			{
	//				Move(m_elapsedTime);
	//				Aim();
	//				Step(m_elapsedTime);
	//			}
	//			else
	//			{
	//				Aiming();
	//				AimMove(m_elapsedTime);
	//				AimStep(m_elapsedTime);
	//				Shot();
	//			}
	//		}
	//		//***********************
	//		// Common Movements
	//		//***********************
	//		ArrowInstamce.Update(elapsedTime);
	//		ChangeCharacter();
	//		RestAnimationIdle();
	//	}
	//}
	//else
	//{
	//	m_input = PAD.GetPad(0);
	//	//Step(m_elapsedTime);
	//	//Attack(m_elapsedTime);
	//	KnockBack();
	//	RestAnimationIdle();
	//}

	//*********************
	// Collision Detection
	//*********************
	m_blendAnimation.animationBlend.Update(m_model, m_elapsedTime);
	FLOAT4X4 blendBone = m_blendAnimation.animationBlend._blendLocals[m_collision[0].GetCurrentMesh(0)].at(m_collision[0].GetCurrentBone(0));
	FLOAT4X4 modelAxisTransform = m_model->_resource->axisSystemTransform;
	FLOAT4X4 getBone = blendBone * modelAxisTransform * m_transformParm.world;
	m_collision[0].position[0] = { getBone._41,getBone._42,getBone._43 };
}

void Archer::Render(ID3D11DeviceContext* immediateContext)
{
//	ArrowInstamce.Render(immediateContext);
	auto& localTransforms = m_blendAnimation.animationBlend._blendLocals;
	//auto& localTransforms = m_blendAnimation.partialBlend._blendLocals;
	VECTOR4F color{ 1.0f,1.0f,1.0f,1.0f };
	m_model->_shaderON.specular = false;
	m_model->Render(immediateContext, m_transformParm.world, color, localTransforms);
	m_arrow->Render(immediateContext);

	VECTOR4F scroll{ 0.0f, 0.0f, 0.0f, 0.0f };
	m_debugObjects.debugObject.Render(immediateContext, scroll, true);
	m_debugObjects.controlPoint.Render(immediateContext, scroll, false);
}

void Archer::Move(float& elapsedTime)
{
	if (m_input->StickDeadzoneLX(m_padDeadLine) || m_input->StickDeadzoneLY(m_padDeadLine))
	{
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
			//*-*-*-*-*-*-*-*-*-*-*-*-
			if (speed > m_moveParm.maxSpeed[1].x)
				m_moveParm.velocity = vector * m_moveParm.maxSpeed[1].x;
			else
				m_moveParm.velocity -= vector * m_moveParm.decleleration;

			m_transformParm.position += m_moveParm.velocity * elapsedTime;

			VECTOR3F angle = m_transformParm.angle;

			float dx = sinf(angle.y);
			float dz = cosf(angle.y);
			float dot = (vector.x * dx) + (vector.z * dz);
			float rot = 1.0f - dot;

			float limit = m_moveParm.turnSpeed;
			if (rot > limit)
				rot = limit;

			float cross = (vector.x * dz) - (vector.z * dx);
			if (cross > 0.0f)
				angle.y += rot;
			else
				angle.y -= rot;

			m_transformParm.angle = angle;
			m_transformParm.WorldUpdate();
		}
		else
			m_moveParm.velocity = {};
	}
	else
	{
		m_moveParm.speed = {};
	}
}

void Archer::ChangeCharacter()
{
	if (m_input->GetButtons(XINPUT_GAMEPAD_BUTTONS::PAD_RIGHT) == 1)
	{
		m_changeParm.changeType = CharacterParameter::Change::PlayerType::FIGHTER;

		MESSENGER.MessageFromPlayer(m_id, MessengType::CHANGE_PLAYER);

		m_input->ResetButton(XINPUT_GAMEPAD_BUTTONS::PAD_RIGHT);

		if (m_mode == Mode::Aiming)
		{
			MESSENGER.MessageFromPlayer(m_id, MessengType::SHIFT_AIM_MODE);
			m_mode = Mode::Moving;
			int samplerSize = static_cast<int>(m_blendAnimation.animationBlend.GetSampler().size());
			if (samplerSize >= 2)
			{
				for (int i = 0; i < samplerSize; ++i)
				{
					m_blendAnimation.animationBlend.ResetAnimationSampler(i);
					m_blendAnimation.animationBlend.ReleaseSampler(i);
				}
			}
			m_blendAnimation.animationBlend.ResetAnimationSampler(0);
			m_animationType = Animation::IDLE;
			m_blendAnimation.animationBlend.ChangeSampler(0, m_animationType,m_model);
		}
	}
}

void Archer::RestAnimationIdle()
{
	if (m_blendAnimation.animationBlend.GetSampler().size() == m_blendAnimation.samplerSize)
	{
		if (m_statusParm.isDamage)
		{
			if (m_blendAnimation.animationBlend.GetSampler()[1].first == Animation::IDLE)
			{
				m_blendAnimation.animationBlend._blendRatio += m_blendAnimation.idleBlendRtio;
				if (m_blendAnimation.animationBlend._blendRatio >= m_blendAnimation.blendRatioMax)
				{
					m_blendAnimation.animationBlend.ReleaseSampler(0);
					m_statusParm.isDamage = false;
					m_blendAnimation.animationBlend._blendRatio = 0.0f;
				}
			}
		}
	}

}

void Archer::Impact()
{
	if (m_blendAnimation.animationBlend.SearchSampler(Animation::HIT)) return;
	m_blendAnimation.animationBlend.AddSampler(Animation::HIT, m_model);
	m_statusParm.isDamage = true;
	Source::CameraControlle::CameraManager().GetInstance()->SetLength(m_cameraParm.lenght);

}

bool Archer::KnockBack()
{
	if (!m_statusParm.isDamage)
		return false;

	int samplerCount = static_cast<int>(m_blendAnimation.animationBlend.GetSampler().size());
	if (samplerCount != 1)
	{
		m_blendAnimation.animationBlend._blendRatio += m_blendAnimation.idleBlendRtio;
		if (m_blendAnimation.animationBlend._blendRatio >= m_blendAnimation.blendRatioMax)
		{
			m_blendAnimation.animationBlend._blendRatio = m_blendAnimation.blendRatioMax;

			for (int i = 0; i < samplerCount - 1; ++i)
			{
				m_blendAnimation.animationBlend.ResetAnimationSampler(0);
				m_blendAnimation.animationBlend.ReleaseSampler(0);
			}

			m_blendAnimation.animationBlend.FalseAnimationLoop(0);
		}

		m_mode = Mode::Moving;
		m_moveParm.isMove = false;
		m_moveParm.isWalk = false;
		m_moveParm.isRun = false;
		m_stepParm.isStep = false;
	}
	else
	{
		uint32_t  currentAnimationFrame = m_blendAnimation.animationBlend.GetAnimationTime(0);
		if (currentAnimationFrame == 58)
		{
			m_blendAnimation.animationBlend._blendRatio = 0.0f;
			m_blendAnimation.animationBlend.ResetAnimationSampler(0);
			m_blendAnimation.animationBlend.ResetAnimationFrame();
			m_animationType = Animation::IDLE;
			m_blendAnimation.animationBlend.AddSampler(m_animationType, m_model);

		}
		else
			return true;
	}
	return true;

}

bool Archer::JudgeBlendRatio(const bool isLoop)
{
	m_blendAnimation.animationBlend._blendRatio += kBlendValue;
	if (m_blendAnimation.animationBlend._blendRatio >= m_blendAnimation.blendRatioMax)//magicNumber
	{
		m_blendAnimation.animationBlend._blendRatio = 0.0f;
		size_t samplerSize = m_blendAnimation.animationBlend.GetSampler().size();
		for (size_t i = 0; i < samplerSize; ++i)
		{
			m_blendAnimation.animationBlend.ReleaseSampler(0);
		}
		if (!isLoop)
			m_blendAnimation.animationBlend.FalseAnimationLoop(0);

		return true;
	}

	return false;
}

bool Archer::Rotate(VECTOR3F& target, const float turnSpeed, bool isLookEnemy)
{
	VECTOR3F normalizeDist = NormalizeVec3(target - m_transformParm.position);
	VECTOR3F angle = m_transformParm.angle;
	VECTOR3F front = VECTOR3F(sinf(angle.y), 0.0f, cosf(angle.y));
	front = NormalizeVec3(front);

	float dot = DotVec3(front, normalizeDist);
	float cosTheta = acosf(dot);
	float range = isLookEnemy ? kAimRange : kViewRange;
	if (cosTheta <= range)
	{
		return true;
	}

	float rot = 1.0f - dot;
	float limit = turnSpeed;
	if (rot > limit)
		rot = limit;

	float cross = (normalizeDist.x * front.z) - (normalizeDist.z * front.x);
	if (cross > 0.0f)
		m_transformParm.angle.y += rot;
	else
		m_transformParm.angle.y -= rot;

	m_transformParm.WorldUpdate();

	return false;
}

bool Archer::FindAttackPoint()
{
#if 0
	auto& enemy = MESSENGER.CallEnemyInstance(0);
	VECTOR3F ePosition = enemy->GetWorldTransform().position;
	VECTOR3F angle = enemy->GetWorldTransform().angle;
	VECTOR3F front = VECTOR3F(sinf(angle.y), 0.0f, cosf(angle.y));
	front = NormalizeVec3(front);

	Collision::Sphere mySelf,target,stage,point;
	mySelf.position = m_transformParm.position;
	mySelf.radius = m_collision[0].radius;
	mySelf.scale = m_collision[0].scale;
	
	target.position = ePosition;
	target.radius = kSafeAreaRadius;
	target.scale = 1.0f;
	Collision collision;
	if (!collision.JudgeSphereAndSphere(mySelf, target))
	{
		float radius = 0.0f;
		float minDistance = FLT_MAX;
		for (int i = 0; i < 8; ++i)
		{
			m_controlPoint[i].second.x = ePosition.x + cosf(radius * 0.01745f) * (kSafeAreaRadius * 0.9f);
			m_controlPoint[i].second.z = ePosition.z + sinf(radius * 0.01745f) * (kSafeAreaRadius * 0.9f);
			radius += 45.0f;

			VECTOR3F normalizeDist = NormalizeVec3(m_controlPoint[i].second - ePosition);
			float dot = DotVec3(front, normalizeDist);
			float cosTheta = acosf(dot);
			if (dot >= 0)
				m_controlPoint[i].first = true;
			else
				m_controlPoint[i].first = false;

			stage.position = {};
			stage.radius = 81.0f;
			stage.scale = 1.0f;

			point.position = m_controlPoint[i].second;
			point.radius = 2.0f;
			point.scale = 1.0f;
			if(!collision.JudgeSphereAndSphere(point, stage))
				m_controlPoint[i].first = true;

			float distance = ToDistVec3(m_controlPoint[i].second - m_transformParm.position);
			if (!m_controlPoint[i].first && distance <= minDistance)
			{
				minDistance = distance;
				m_attackPoint = m_controlPoint[i].second;
			}
		}
	}
	else
	{

	}

#else
	auto& enemy = MESSENGER.CallEnemyInstance(0);
	VECTOR3F ePosition = enemy->GetWorldTransform().position;
	VECTOR3F angle = enemy->GetWorldTransform().angle;
	VECTOR3F front = VECTOR3F(sinf(angle.y), 0.0f, cosf(angle.y));
	front = NormalizeVec3(front);

	Collision::Sphere stage, point;
	Collision collision;
	float radius = 0.0f;
	float minDistance = FLT_MAX;
	VECTOR3F atkPos = {};
	for (int i = 0; i < 8; ++i)
	{
		m_controlPoint[i].second.x = ePosition.x + cosf(radius * 0.01745f) * (kSafeAreaRadius * 0.9f);
		m_controlPoint[i].second.z = ePosition.z + sinf(radius * 0.01745f) * (kSafeAreaRadius * 0.9f);
		radius += 45.0f;

		VECTOR3F normalizeDist = NormalizeVec3(m_controlPoint[i].second - ePosition);
		float dot = DotVec3(front, normalizeDist);
		if (dot >= 0)
			m_controlPoint[i].first = true;
		else
			m_controlPoint[i].first = false;

		stage.position = {};
		stage.radius = 81.0f;
		stage.scale = 1.0f;

		point.position = m_controlPoint[i].second;
		point.radius = 2.0f;
		point.scale = 1.0f;
		if (!collision.JudgeSphereAndSphere(point, stage))
			m_controlPoint[i].first = true;

		float distance = ToDistVec3(m_controlPoint[i].second - m_transformParm.position);
		if (!m_controlPoint[i].first && distance <= minDistance)
		{
			minDistance = distance;
			atkPos = m_controlPoint[i].second;
		}
	}

#endif

	float dist = ToDistVec3(atkPos - m_attackPoint);
	if (dist > 10.0f)
		m_attackPoint = atkPos;
	else
		m_canRun = false;

	m_hasRotated = false;
	m_moveParm.velocity = NormalizeVec3(m_attackPoint - m_transformParm.position);

#if _DEBUG
	m_debugObjects.controlPoint.GetInstance().clear();
	if (m_debugObjects.controlPoint.GetInstance().empty())
	{
		auto primitive = m_debugObjects.GetSphere(Framework::GetInstance().GetDevice(), "");
		m_debugObjects.controlPoint.AddGeometricPrimitive(std::move(primitive));

		for (int i = 0; i < 8; ++i)
		{
			VECTOR4F color = VECTOR4F(1.0f, 0.0f, 0.0f, 1.0f);
			if (m_controlPoint.at(i).first)
			{
				color.x = 0.0f;
				color.z = 1.0f;
			}
			m_debugObjects.controlPoint.AddInstanceData(m_controlPoint.at(i).second, VECTOR3F(0.0f * 0.01745f, 180.0f * 0.01745f, 0.0f * 0.017454f),
				VECTOR3F(2.0f, 2.0f, 2.0f), color);
		}
	}
	else
	{
		for (int i = 0; i < 8; ++i)
		{
			VECTOR4F color = VECTOR4F(1.0f, 0.0f, 0.0f, 1.0f);
			if (m_controlPoint.at(i).first)
			{
				color.x = 0.0f;
				color.z = 1.0f;
			}
			m_debugObjects.controlPoint.GetInstanceData(i).position = m_controlPoint.at(i).second;
			m_debugObjects.controlPoint.GetInstanceData(i).color = color;
			m_debugObjects.controlPoint.GetInstanceData(i).CreateWorld();
		}
	}
#endif
	return true;
}

bool Archer::MoveRun()
{
	if (!m_canRun)
	{
		m_state = 0;
		m_hasRotated = false;
		m_blendAnimation.animationBlend.AddSampler(Animation::AIM, m_model);
		m_hasBlendAnim = false;
		m_canRun = true;
		return true;
	}

	if (!m_hasRotated)
		m_hasRotated = Rotate(m_attackPoint, 0.1f);

	switch (m_state)
	{
	case 0:
	{
		if (m_blendAnimation.animationBlend.SearchSampler(Animation::RUN))
		{
			if (JudgeBlendRatio())
				++m_state;
		}
		else
		{
			m_blendAnimation.animationBlend.ResetAnimationSampler(0);
			m_blendAnimation.animationBlend.AddSampler(Animation::RUN, m_model);
		}
	}
	break;
	case 1:
	{
		VECTOR3F vector = m_moveParm.velocity * m_moveParm.accle;
		m_transformParm.position += vector * m_elapsedTime;
		m_transformParm.WorldUpdate();

		float distance = ToDistVec3(m_attackPoint - m_transformParm.position);
		if (distance <= 1.0f)
		{
			m_state = 0;
			m_hasRotated = false;
			m_blendAnimation.animationBlend.AddSampler(Animation::AIM, m_model);
			m_hasBlendAnim = false;
			return true;
		}
	}
	}
	return false;
}

bool Archer::SetArrow()
{	
	auto& enemy = MESSENGER.CallEnemyInstance(0);
	VECTOR3F ePosition = enemy->GetWorldTransform().position;
	if (!m_hasBlendAnim)
		m_hasBlendAnim = JudgeBlendRatio();

	if (!m_hasRotated)
		m_hasRotated = Rotate(ePosition,0.3f,true);

	if (m_hasRotated && m_hasBlendAnim)
		return true;

	return false;
}

bool Archer::Shoot()
{
	switch (m_state)
	{
	case 0:
		m_blendAnimation.animationBlend.ResetAnimationSampler(0);
		m_blendAnimation.animationBlend.AddSampler(Animation::SHOOT, m_model);
		m_hasBlendAnim = false;
		++m_state;
		break;
	case 1:
		if (!m_hasBlendAnim)
			m_hasBlendAnim = JudgeBlendRatio(false);
		else
		{
			m_state = 0;
			m_hasBlendAnim = false;
			return true;
		}
		break;
	}
	return false;
}

bool Archer::FindDirectionToAvoid()
{
	auto& enemy = MESSENGER.CallEnemyInstance(0);

	std::vector<int> entryPointID;
	Collision::Sphere stage,point,target;
	stage.position = {};
	stage.radius = 81.0f;
	stage.scale = 1.0f;

	point.radius = 5.0f;
	point.scale = 1.0f;

	target.position = enemy->GetWorldTransform().position;
	target.radius = enemy->GetCollision().at(0).radius;
	target.scale = 1.0f;

	float angleOffset = 0.0f;
	VECTOR3F angle = m_transformParm.angle;
	VECTOR3F front = VECTOR3F(sinf(angle.y), 0.0f, cosf(angle.y));
	front = NormalizeVec3(front);
	VECTOR3F normalizeDist = NormalizeVec3(enemy->GetWorldTransform().position - m_transformParm.position);
	float dot = DotVec3(front, normalizeDist);
	if (dot >= 0)
		angleOffset = -36.0f;
	else
		angleOffset = 36.0f;

	Collision collision;
	float radius = 0.0f;
	for (int i = 0; i < 6; ++i)
	{
		m_controlPoint[i].second.x = m_transformParm.position.x + cosf(radius * 0.01745f) * (kSafeAreaRadius * 0.6f);
		m_controlPoint[i].second.z = m_transformParm.position.z + sinf(radius * 0.01745f) * (kSafeAreaRadius * 0.6f);
		radius += angleOffset;
		point.position = m_controlPoint[i].second;

		if (!collision.JudgeSphereAndSphere(point, stage))
		{
			m_controlPoint[i].first = true;
			continue;
		}
		else if (collision.JudgeSphereAndSphere(point, target))
		{
			m_controlPoint[i].first = true;
			continue;
		}
		m_controlPoint[i].first = false;
		entryPointID.push_back(i);
	}

	/* 乱数系列の変更 */
	srand((unsigned)time(NULL));
	int selectID = rand() % static_cast<int>(entryPointID.size());
	int selectPointID = entryPointID[selectID];
	m_avoidPoint = m_controlPoint[selectPointID].second;
	m_hasRotated = false;
	m_moveParm.velocity = NormalizeVec3(m_avoidPoint - m_transformParm.position);

#if _DEBUG
	m_debugObjects.controlPoint.GetInstance().clear();
	if (m_debugObjects.controlPoint.GetInstance().empty())
	{
		auto primitive = m_debugObjects.GetSphere(Framework::GetInstance().GetDevice(), "");
		m_debugObjects.controlPoint.AddGeometricPrimitive(std::move(primitive));

		for (int i = 0; i < 6; ++i)
		{
			VECTOR4F color = VECTOR4F(1.0f, 0.0f, 0.0f, 1.0f);
			if (m_controlPoint.at(i).first)
			{
				color.x = 0.0f;
				color.z = 1.0f;
			}
			else if (selectPointID == i)
			{
				color = VECTOR4F(0.0f, 1.0f, 0.0f, 1.0f);
			}
			m_debugObjects.controlPoint.AddInstanceData(m_controlPoint.at(i).second, VECTOR3F(0.0f * 0.01745f, 180.0f * 0.01745f, 0.0f * 0.017454f),
				VECTOR3F(5.0f, 5.0f, 5.0f), color);
		}
	}
	else
	{
		for (int i = 0; i < 6; ++i)
		{
			VECTOR4F color = VECTOR4F(1.0f, 0.0f, 0.0f, 1.0f);
			if (m_controlPoint.at(i).first)
			{
				color.x = 0.0f;
				color.z = 1.0f;
			}
			else if (selectPointID == i)
			{
				color = VECTOR4F(0.0f, 1.0f, 0.0f, 1.0f);
			}
			m_debugObjects.controlPoint.GetInstanceData(i).position = m_controlPoint.at(i).second;
			m_debugObjects.controlPoint.GetInstanceData(i).color = color;
			m_debugObjects.controlPoint.GetInstanceData(i).CreateWorld();
		}
	}
#endif

	return true;
}

bool Archer::Avoid()
{
	if (!m_hasRotated)
		m_hasRotated = Rotate(m_avoidPoint, 0.1f);

	m_stepParm.speed -= m_stepParm.deceleration;
	if (m_stepParm.speed.x <= 0.0f)
		m_stepParm.speed = { 0.0f,0.0f,0.0f };

	VECTOR3F  velocity = {};
	velocity.x = m_moveParm.velocity.x * m_stepParm.speed.x;
	velocity.y =  0.0f;
	velocity.z = m_moveParm.velocity.z * m_stepParm.speed.z;

	m_transformParm.position += velocity * m_elapsedTime;
	m_transformParm.WorldUpdate();

	if (!m_stepParm.isStep)
	{
		if (m_blendAnimation.animationBlend.SearchSampler(Animation::DIVE))
		{
			if (JudgeBlendRatio(false))
				m_stepParm.isStep = true;
		}
		else
		{
			m_blendAnimation.animationBlend.ResetAnimationSampler(0);
			m_blendAnimation.animationBlend.AddSampler(Animation::DIVE, m_model);
			m_blendAnimation.animationBlend.SetAnimationSpeed(1.6f);
		}
	}
	else
	{
		uint32_t  currentAnimationFrame = m_blendAnimation.animationBlend.GetAnimationTime(0);
		if (currentAnimationFrame == m_stepParm.frameCount)
		{
			m_state = 0;
			m_hasRotated = false;
			m_stepParm.isStep = false;
			m_stepParm.speed = m_stepParm.maxSpeed;
			m_blendAnimation.animationBlend.SetAnimationSpeed(1.0f);
			return true;
		}
	}

	return false;
}

bool Archer::Heal()
{
	return false;
}

bool Archer::Revival()
{
	return false;
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
	
static int currentMesh = 2;
ImGui::BulletText(u8"Mesh%d番目", currentMesh);

static int currentBone = 0;

ImGui::Combo("Name_of_BoneName",
	&currentBone,
	vectorGetter,
	static_cast<void*>(&m_blendAnimation.animationBlend.GetBoneName()[currentMesh]),
	static_cast<int>(m_blendAnimation.animationBlend.GetBoneName()[currentMesh].size())
);

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
	// Move
	//**************************************
	if (ImGui::CollapsingHeader("Move"))
	{
		{
			static float  accel0 = m_moveParm.accle.x;

			ImGui::SliderFloat("Accel0", &accel0, 0.0f, 50.0f);

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
	// Step
	//**************************************
	if (ImGui::CollapsingHeader("Step"))
	{
		//FrameCount
		{
			static float frameCount = m_stepParm.frameCount;
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

		float deceleration = m_stepParm.deceleration.x;
		ImGui::SliderFloat("StepDeceleration", &deceleration, 0.0f, 1.0f);
		m_stepParm.deceleration = VECTOR3F(deceleration, 0.0f, deceleration);

		float speed = m_stepParm.speed.x;
		ImGui::SliderFloat("StepSpeed", &speed, 0.0f, 100.0f);
		m_stepParm.speed.x = m_stepParm.speed.z = speed;

	}

	//**************************************
	// Collision
	//**************************************
	if (ImGui::CollapsingHeader("Collision"))
	{
		static int current = 0;
		ImGui::RadioButton("Body", &current, 0); ImGui::SameLine();
		ImGui::RadioButton("SafeArea", &current, 1);

		float bonePositions[3] = {};
		VECTOR3F cPosition;
		if (current == 0)
		{
			FLOAT4X4 blendBone = m_blendAnimation.animationBlend._blendLocals[currentMesh].at(currentBone);
			FLOAT4X4 modelAxisTransform = m_model->_resource->axisSystemTransform;
			FLOAT4X4 getBone = blendBone * modelAxisTransform * m_transformParm.world;

			bonePositions[0] = getBone._41;
			bonePositions[1] = getBone._42;
			bonePositions[2] = getBone._43;
			ImGui::SliderFloat3("Position", bonePositions, -10.0f, 10.0f);
			cPosition = { bonePositions[0],bonePositions[1],bonePositions[2] };
		}
		else
		{
			auto& enemy = MESSENGER.CallEnemyInstance(0);
			cPosition = enemy->GetWorldTransform().position;
		}

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

			m_debugObjects.debugObject.AddInstanceData(cPosition, VECTOR3F(0.0f * 0.01745f, 180.0f * 0.01745f, 0.0f * 0.017454f),
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
					geomtry.position = cPosition;
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
					ImGui::SliderFloat("Radius", &radius, 1, 100);
					
					if (ImGui::Button("RadiusChange"))
					{
						geomtry.scale = { 1.0f,1.0f,1.0f };
						if (current == 1)
							radius = kSafeAreaRadius;
						geomtry.scale *= radius;
						m_collision[current].radius = radius;
					}
				}

				//Mesh & Bone SetNumber
				{
					if (ImGui::Button("Mesh 0"))
						m_collision[current].SetCurrentMesh(currentMesh, 0);
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
					std::vector<std::string> collisionType = { "Cube","SPHER","CAPSULE","CIRCLE","RECT","CYLINDER" };
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
	// Arrow
	//**************************************
	if (ImGui::CollapsingHeader("Arrow"))
	{
		m_statusParm.attackPoint = 10.0f;
		VECTOR3F angle = m_transformParm.angle;
		DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
		FLOAT4X4 rotationM = {};
		DirectX::XMStoreFloat4x4(&rotationM, R);
		VECTOR3F zAxis = { rotationM._31,rotationM._32,rotationM._33 };
		zAxis = NormalizeVec3(zAxis);

		FLOAT4X4 blendBone = m_blendAnimation.animationBlend._blendLocals[2].at(36);
		FLOAT4X4 modelAxisTransform = m_model->_resource->axisSystemTransform;
		FLOAT4X4 getBone = blendBone * modelAxisTransform * m_transformParm.world;
		VECTOR3F position = { getBone._41,getBone._42,getBone._43 };

		if (ImGui::Button("Create"))
			m_arrow->PrepareForArrow(position, m_transformParm.angle, zAxis);

		if (ImGui::CollapsingHeader("Update"))
			m_arrow->Update(m_elapsedTime);
		else
		{
			m_arrow->GetArrowParam()[0].first.position = position;
			m_arrow->GetArrowParam()[0].first.angle = m_transformParm.angle;
			m_arrow->GetArrowParam()[0].first.CreateWorld();
		}
		m_arrow->ImGui(device);

		if (ImGui::CollapsingHeader("Rotation"))
		{


			m_transformParm.angle.y += 1.0f * 0.01745f;
			m_transformParm.WorldUpdate();
		}

	}

	//**************************************
	// Function
	//**************************************
	if (ImGui::CollapsingHeader("Function"))
	{
		static bool activeFunction[6];
		if (ImGui::Button("Search"))
			activeFunction[0] = true;
		if (activeFunction[0])
		{
			if (FindAttackPoint())
				activeFunction[0] = false;

			if (m_debugObjects.controlPoint.GetInstance().empty())
			{
				auto primitive = m_debugObjects.GetSphere(device, "");
				m_debugObjects.controlPoint.AddGeometricPrimitive(std::move(primitive));

				for (int i = 0; i < 8; ++i)
				{
					VECTOR4F color = VECTOR4F(1.0f, 0.0f, 0.0f, 1.0f);
					if (m_controlPoint.at(i).first)
					{
						color.x = 0.0f;
						color.z = 1.0f;
					}
					m_debugObjects.controlPoint.AddInstanceData(m_controlPoint.at(i).second, VECTOR3F(0.0f * 0.01745f, 180.0f * 0.01745f, 0.0f * 0.017454f),
						VECTOR3F(2.0f, 2.0f, 2.0f), color);
				}
			}
			else
			{
				for (int i = 0; i < 8; ++i)
				{
					VECTOR4F color = VECTOR4F(1.0f, 0.0f, 0.0f, 1.0f);
					if (m_controlPoint.at(i).first)
					{
						color.x = 0.0f;
						color.z = 1.0f;
					}
					m_debugObjects.controlPoint.GetInstanceData(i).position = m_controlPoint.at(i).second;
					m_debugObjects.controlPoint.GetInstanceData(i).color = color;
					m_debugObjects.controlPoint.GetInstanceData(i).CreateWorld();
				}
			}
		}
		
		if (ImGui::Button("Run"))
			activeFunction[1] = true;

		if (activeFunction[1])
		{
			if ((MoveRun()))
				activeFunction[1] = false;
		}

		if (ImGui::Button("Set"))
			activeFunction[2] = true;

		if (activeFunction[2])
		{
			if ((SetArrow()))
				activeFunction[2] = false;
		}

		if (ImGui::Button("Shoot"))
			activeFunction[3] = true;

		if (activeFunction[3])
		{
			if ((Shoot()))
				activeFunction[3] = false;
		}

		if (ImGui::Button("Velocity"))
			activeFunction[4] = true;

		if (activeFunction[4])
		{
			if ((FindDirectionToAvoid()))
				activeFunction[4] = false;
		}

		if (ImGui::Button("Avoid"))
			activeFunction[5] = true;

		if (activeFunction[5])
		{
			if ((Avoid()))
				activeFunction[5] = false;
		}
	}

	//**************************************
	// Domain
	//**************************************
	if (ImGui::CollapsingHeader("HTN"))
	{
		ImGui::SetNextTreeNodeOpen(false, ImGuiSetCond_Once);
		if (ImGui::TreeNode("PrimitiveTask"))
		{
			static int primitiveTask = 0;
			ImGui::RadioButton("FindAttackPoint", &primitiveTask, 0); ImGui::SameLine();
			ImGui::RadioButton("Move", &primitiveTask, 1); 
			ImGui::RadioButton("SetArrow", &primitiveTask, 2); ImGui::SameLine();
			ImGui::RadioButton("ShootArrow", &primitiveTask, 3);
			ImGui::RadioButton("Avoiding", &primitiveTask, 4); ImGui::SameLine();
			ImGui::RadioButton("FindDirectionAvoid", &primitiveTask, 5);
			PrimitiveTaskType type = static_cast<PrimitiveTaskType>(primitiveTask);
	
			int count = static_cast<int>(m_domain.GetPrimitiveTasks().size());
			ImGui::InputInt("PrimitiveTasks Size", &count);

			if (ImGui::Button("SetPrimitiveTask"))
			{
				m_domain.SetPrimitiveTask(type);
				m_domainConverter.AddPrimitivetaskType(type);
			}

			if (ImGui::Button("Set Operator"))
			{
				switch (type)
				{
				case PrimitiveTaskType::FindAttackPoint:
					m_domain.GetPrimitiveTask(type)->SetOperator(&Archer::FindAttackPoint);
					break;
				case PrimitiveTaskType::Move:
					m_domain.GetPrimitiveTask(type)->SetOperator(&Archer::MoveRun);
					break;
				case PrimitiveTaskType::SetArrow:
					m_domain.GetPrimitiveTask(type)->SetOperator(&Archer::SetArrow);
					break;
				case PrimitiveTaskType::ShootArrow:
					m_domain.GetPrimitiveTask(type)->SetOperator(&Archer::Shoot);
					break;
				case PrimitiveTaskType::Avoiding:
					m_domain.GetPrimitiveTask(type)->SetOperator(&Archer::Avoid);
					break;
				case PrimitiveTaskType::FindDirectionAvoid:
					m_domain.GetPrimitiveTask(type)->SetOperator(&Archer::FindDirectionToAvoid);
					break;
				}
			}

			ImGui::TreePop();
		}

		ImGui::SetNextTreeNodeOpen(false, ImGuiSetCond_Once);
		if (ImGui::TreeNode("CompoundTask"))
		{
			static int compoundTask = 0;
			ImGui::RadioButton("Attack", &compoundTask, 0); ImGui::SameLine();
			ImGui::RadioButton("PrepareAttack", &compoundTask, 1);
			ImGui::RadioButton("Avoid", &compoundTask, 2);
			CompoundTaskType type = static_cast<CompoundTaskType>(compoundTask);

			int count = static_cast<int>(m_domain.GetCompoundTasks().size());
			ImGui::InputInt("CompoundTasks Size", &count);

			if (ImGui::Button("SetCompoundTask"))
			{
				m_domain.SetCompoundTask(type);
				m_domainConverter.AddCompoundTaskType(type);
			}
			ImGui::SameLine();

			if (ImGui::Button("Complete Task"))
				m_domain.CompleteCompoundTask(type);

			if (ImGui::Button("Select RootTask"))
			{
				auto task = m_domain.GetCompoundTask(type);
				m_planRunner.SetRootTask(task);
			}

			ImGui::TreePop();
		}

		ImGui::SetNextTreeNodeOpen(false, ImGuiSetCond_Once);
		if (ImGui::TreeNode("Method"))
		{
			static int method = 0;
			ImGui::RadioButton("AtkMethod", &method, 0); ImGui::SameLine();
			ImGui::RadioButton("FindAPMethod", &method, 1);
			ImGui::RadioButton("PrepareAtkMethod", &method, 2); ImGui::SameLine();
			ImGui::RadioButton("AvoidMethod", &method, 3);
			ImGui::RadioButton("FindDAMethod", &method, 4);

			int count = static_cast<int>(m_domain.GetMethods().size());
			ImGui::InputInt("Methods Size", &count);

			if (ImGui::Button("SetMethod"))
			{
				MethodType type = static_cast<MethodType>(method);
				m_domain.SetMethod(type);
				m_domainConverter.AddMethodType(type);
			}

			ImGui::TreePop();
		}

		ImGui::SetNextTreeNodeOpen(false, ImGuiSetCond_Once);
		if (ImGui::TreeNode("Precondition"))
		{
			static int precondition = 0;
			ImGui::RadioButton("AtkPrecondition", &precondition, 0); ImGui::SameLine();
			ImGui::RadioButton("PrepareAtkPrecondition", &precondition, 1);
			ImGui::RadioButton("TruePrecondition", &precondition, 2); ImGui::SameLine();
			ImGui::RadioButton("AvoidPrecondition", &precondition, 3);
			ImGui::RadioButton("FindDAPrecondition", &precondition, 4);

			int count = static_cast<int>(m_domain.GetPreconditions().size());
			ImGui::InputInt("Preconditions Size", &count);

			if (ImGui::Button("SetPrecondition"))
			{
				PreconditionType type = static_cast<PreconditionType>(precondition);
				m_domain.SetPrecondition(type);
				m_domainConverter.AddPreconditionType(type);
			}

			ImGui::TreePop();
		}

		ImGui::SetNextTreeNodeOpen(false, ImGuiSetCond_Once);
		if (ImGui::TreeNode("Planner"))
		{
			static bool isExecute = false;
			if (ImGui::Button("Planning"))
				m_currentPlanList = m_planRunner.UpdatePlan(m_worldState);
			if (ImGui::Button("Execute"))
				isExecute = !isExecute;
			if (isExecute)
			{
				size_t count = m_currentPlanList.size();
				static size_t currentTask = 0;
				ImGui::Text("CurrentTask %d", static_cast<int>(currentTask));
				for (currentTask; currentTask < count;)
				{
					if (m_currentPlanList.at(currentTask)->ExecuteOperator(this))
						++currentTask;
					else
						break;

				}
				if (currentTask == count)
				{
					isExecute = false;
					currentTask = 0;
				}
			}

				ImGui::TreePop();
		}

	}
#endif
	ImGui::End();

#endif
}
