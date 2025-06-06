#include "ArcherCharacter.h"
#include "MessengTo.h"
#include "Arrow.h"
#include "Collision.h"
#include ".\LibrarySource\ModelData.h"
#include ".\LibrarySource\VectorCombo.h"

#include "..\External_libraries\imgui\imgui.h"
#include "..\External_libraries\imgui\imgui_impl_dx11.h"
#include "..\External_libraries\imgui\imgui_impl_win32.h"
#include "..\External_libraries\imgui\imgui_internal.h"
CEREAL_CLASS_VERSION(Archer, 8);

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

	m_elapsedTime = 0.0f;

	m_transformParm.position = { 30.0f,0.0f,-20.0f };
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

	m_currentPlanList = m_agentAI.Init(this);
	m_statusParm.life = 100.0f;
	m_stepParm.maxSpeed = m_stepParm.speed;
	m_controlPoint.resize(8);
	m_canRun = true;
	m_isSetArrow = false;
	auto& wepon = RunningMarket().FindProductConer(0);
	std::unique_ptr<Arrow> arrow{ wepon.GiveProduct<Arrow>() };
	m_arrow = std::move(arrow);
	m_statusParm.attackPoint = 10.0f;
	m_writeTimer = 0.0f;
	m_recoverTimer = 0.0f;
	m_recoverTarget = {};
	m_selectCharacter = 0;
	m_playerCreditLv = 1.0f;
	m_writeMaxTimer = 1.0f;
	m_recoverMaxTimer = 6.0f;
	m_goalTimer = 0.0f;
	m_goalMaxTimer = 2.5f;
	m_currentTask = 0;
	m_shootCount = 0;
	m_selectPoint = 0;
	m_isAdjust = false;
	m_onEffect = false;
	m_canRecover = true;
	m_damageParm.maxSpeed = m_damageParm.speed;
	m_statusParm.maxLife = m_statusParm.life;
//	m_selectMovePoint.resize(4);
}

void Archer::Update(float& elapsedTime)
{
	m_elapsedTime = elapsedTime;
//	m_active = false;
	if (m_active)
	{
		if (m_hasShoot)
		{
			m_arrow->Update(m_elapsedTime);
			if (!m_arrow->GetArrowParam().at(0).second.isFlying)
				m_hasShoot = false;
		}

		if (!m_currentPlanList.empty() && !m_statusParm.isDamage)
		{

			int taskCount = static_cast<int>(m_currentPlanList.size());
			if (m_currentPlanList[m_currentTask]->ExecuteOperator(this))
			{
				++m_currentTask;
			}
			if (m_currentTask >= taskCount)
			{
				m_currentPlanList = m_agentAI.OperationFlowStart(this);
				m_currentTask = 0;
			}
		}
		ActiveWriteTimer();
		ActiveRecoverTimer();
		m_attackArrow.UpdateTime(elapsedTime);
	}
	KnockBack();

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
	if (m_hasShoot)
		m_arrow->Render(immediateContext);

	VECTOR4F scroll{ 0.0f, 0.0f, 0.0f, 0.0f };
	m_debugObjects.debugObject.Render(immediateContext, scroll, true);

	if(m_isAdjust)
		m_debugObjects.controlPoint.Render(Framework::GetInstance().GetContext(), scroll, false);

}

void Archer::Release()
{
	m_blendAnimation.animationBlend.ReleaseAllSampler();
	m_agentAI.Release();
	if (m_model)
	{
		if (m_model.unique())
		{
			m_model.reset();
		}
	}
}

void Archer::WriteBlackboard(CharacterAI* target)
{
	m_agentAI.SavePerception(this, target);
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
	if (m_blendAnimation.animationBlend.SearchSampler(Animation::HITBIG)) return;

	//***********************
	// Reactions that match
	// the requirements.
	//***********************
	if (m_statusParm.life > 0)
	{
		if (m_damageParm.hasBigDamaged)
		{
			m_blendAnimation.animationBlend.AddSampler(Animation::HITBIG, m_model);
		}
		else
		{
			m_blendAnimation.animationBlend.AddSampler(Animation::HIT, m_model);
		}
	}
	else
		m_blendAnimation.animationBlend.AddSampler(Animation::DEATH, m_model);

	//*******************
	// Flag related
	//*******************
	{
		//if (m_adjustAnimation)
		//	m_adjustAnimation = !m_adjustAnimation;

		m_statusParm.isDamage = true;

	}

	//***************************
	// Correct angle 
	// in direction of movement
	//***************************
	{
		float dot = atan2f(m_damageParm.vector.x, m_damageParm.vector.z);
		m_transformParm.angle.y = dot;
		m_transformParm.WorldUpdate();
	}

	m_blendAnimation.animationBlend.SetAnimationSpeed(1.3f);
	KnockBack();
}

bool Archer::KnockBack()
{
	//******************************
	// If it is false, 
	// you can do Move,Step,Attack,
	// but if it is true, 
	// you can do KnockBack
	//******************************
	if (!m_statusParm.isDamage)
		return false;

	//**************************************
	// Set the blend ratio to 1.
	// When we get to 1, we stop blending.
	//**************************************
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
	}
	else
	{
		if (m_statusParm.life > 0)
		{
			uint32_t animationEnd = m_damageParm.hasBigDamaged ? 135 : 60;
			uint32_t  currentAnimationFrame = m_blendAnimation.animationBlend.GetAnimationTime(0);

			if (currentAnimationFrame == animationEnd)
			{
				m_statusParm.isDamage = false;
				m_animationType = Animation::IDLE;
				m_blendAnimation.animationBlend._blendRatio = 0.f;
				m_damageParm.speed = m_damageParm.maxSpeed;
				m_blendAnimation.animationBlend.ResetAnimationFrame();
				m_blendAnimation.animationBlend.SetAnimationSpeed(1.0f);
				m_currentPlanList = m_agentAI.OperationFlowStart(this);
				m_selectMovePoint.clear();
				m_currentTask = 0;
				m_state = 0;
				m_selectPoint = 0;
				m_stepParm.speed = m_stepParm.maxSpeed;
				m_hasRotated = false;
				m_hasBlendAnim = false;
				m_canRun = true;
				return true;

			}
		}
		else
		{
			m_blendAnimation.animationBlend._blendRatio = 0.0f;
			m_blendAnimation.animationBlend.ResetAnimationFrame();
			m_blendAnimation.animationBlend.SetAnimationSpeed(1.0f);
		}
	}

	//*******************
	// Move backwards
	//*******************
	{
		VECTOR3F deceleration = m_damageParm.hasBigDamaged ? m_damageParm.deceleration : m_damageParm.deceleration * 3.0f;
		m_damageParm.speed -= deceleration;
		if (m_damageParm.speed.x <= 0.0f)
			m_damageParm.speed = { 0.0f,0.0f,0.0f };

		VECTOR3F angle = m_transformParm.angle;
		DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
		FLOAT4X4 rotationM = {};
		DirectX::XMStoreFloat4x4(&rotationM, R);
		VECTOR3F front = { rotationM._31,rotationM._32,rotationM._33 };
		VECTOR3F velocity = {};
		velocity.x = front.x * (m_damageParm.speed.x) * -1.0f;
		velocity.y = 0.0f;
		velocity.z = front.z * (m_damageParm.speed.z) * -1.0f;

		m_transformParm.position += velocity * m_elapsedTime;
		m_transformParm.WorldUpdate();
	}

	return true;

}

bool Archer::JudgeBlendRatio(const bool isLoop)
{
	m_blendAnimation.animationBlend._blendRatio += m_blendAnimation.idleBlendRtio;
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

void Archer::ActiveWriteTimer()
{
	m_writeTimer += m_elapsedTime;
	if (m_writeTimer >= m_writeMaxTimer)
	{
		CharacterAI* target = MESSENGER.CallEnemyInstance(EnemyType::Boss);
		WriteBlackboard(target);
		target = MESSENGER.CallPlayerInstance(PlayerType::Fighter);
		WriteBlackboard(target);
		m_writeTimer = 0.0f;
	}
}

void Archer::ActiveRecoverTimer()
{
	if (!m_canRecover)
	{
		m_recoverTimer += m_elapsedTime;
		if (m_recoverTimer > m_recoverMaxTimer)
			m_canRecover = true;
	}
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
	CharacterAI* enemy = MESSENGER.CallEnemyInstance(EnemyType::Boss);
	VECTOR3F ePosition = enemy->GetWorldTransform().position;
	VECTOR3F angle = enemy->GetWorldTransform().angle;
	VECTOR3F front = VECTOR3F(sinf(angle.y), 0.0f, cosf(angle.y));
	front = NormalizeVec3(front);

	Collision::Sphere stage, point;

	stage.position = {};
	stage.radius = 81.0f;
	stage.scale = 1.0f;
	point.radius = m_collision[0].scale;
	point.scale = 1.0f;
	Collision collision;
	int selectNo = 0;
	float radius = 0.0f;
	float minDistance = FLT_MAX;
	VECTOR3F atkPos = {};
	bool isSelectAtkPoint = false;
	
	//敵から半径45離れた地点に8個の攻撃地点を配置
	//そして選択
	for (int i = 0; i < 8; ++i)
	{
		m_controlPoint[i].second.x = ePosition.x + cosf(radius * 0.01745f) * (kSafeAreaRadius * 0.7f);
		m_controlPoint[i].second.z = ePosition.z + sinf(radius * 0.01745f) * (kSafeAreaRadius * 0.7f);
		radius += 45.0f;

		VECTOR3F normalizeDist = NormalizeVec3(m_controlPoint[i].second - ePosition);
		float dot = DotVec3(front, normalizeDist);
		if (dot >= 0.f)
			m_controlPoint[i].first = true;
		else
			m_controlPoint[i].first = false;


		point.position = m_controlPoint[i].second;

		if (!collision.JudgeSphereAndSphere(point, stage))
			m_controlPoint[i].first = true;

		float distance = ToDistVec3(m_controlPoint[i].second - m_transformParm.position);
		if (!m_controlPoint[i].first && distance <= minDistance)
		{
			minDistance = distance;
			atkPos = m_controlPoint[i].second;
			isSelectAtkPoint = true;
			selectNo = i;
		}
	}

#endif
	
	//選択されなかったときは3番を選ぶ
	if (!isSelectAtkPoint)
	{
		int num  = rand() % 8;
		atkPos = m_controlPoint[num].second;
		selectNo = num;
	}

	//選んだポイントが近いのなら走らなくていい
	float dist = ToDistVec3(atkPos - m_transformParm.position);
	if (dist > 10.0f)
		m_selectMovePoint.emplace_back(atkPos);
	else
		m_canRun = false;

	//同じ場所で4回打ったら
	//次の場所を決める
	//if (m_shootCount == 4)
	//{
	//	m_canRun = true;
	//	m_shootCount = 0;
	//	int count = static_cast<int>(m_controlPoint.size());
	//	//ルートを決める
	//	for (int i = 1; i <= 3; ++i)
	//	{
	//		int num = i + selectNo;
	//		if (num >= count)
	//		{
	//			num -= count;
	//		}
	//		VECTOR3F position = m_controlPoint[num].second;
	//		point.position = position;

	//		if (collision.JudgeSphereAndSphere(point, stage))
	//		{
	//			m_selectMovePoint.emplace_back(position);
	//			selectNo = num;
	//		}
	//	}

	//	if (m_selectMovePoint.size() != 3)
	//	{
	//		m_selectMovePoint.clear();
	//		for (int i = 1; i <= 3; ++i)
	//		{
	//			int num = selectNo - i;
	//			if (num < 0)
	//			{
	//				num += count;
	//			}
	//			VECTOR3F position = m_controlPoint[num].second;
	//			point.position = position;

	//			if (collision.JudgeSphereAndSphere(point, stage))
	//			{
	//				m_selectMovePoint.emplace_back(position);
	//				selectNo = num;
	//			}
	//		}
	//	}

	//	if(m_selectMovePoint.empty())
	//		m_selectMovePoint.emplace_back(atkPos);
	//}

	m_hasRotated = false;
//	m_moveParm.velocity = NormalizeVec3(m_selectMovePoint.back() - m_transformParm.position);
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
			if (selectNo == i)
			{
				color = VECTOR4F(0.0f, 1.0f, 0.0f, 1.0f);
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
			if (selectNo == i)
			{
				color = VECTOR4F(0.0f, 1.0f, 0.0f, 1.0f);
			}
			m_debugObjects.controlPoint.GetInstanceData(i).position = m_controlPoint.at(i).second;
			m_debugObjects.controlPoint.GetInstanceData(i).color = color;
			m_debugObjects.controlPoint.GetInstanceData(i).CreateWorld();
		}
	}

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

	if (m_blendAnimation.animationBlend.SearchSampler(Animation::RUN))
	{
		if (JudgeBlendRatio())
			++m_state;
	}
	else
	{
		if (!m_blendAnimation.animationBlend.SearchSampler(Animation::HIT) &&
			!m_blendAnimation.animationBlend.SearchSampler(Animation::HITBIG) &&
			!m_blendAnimation.animationBlend.SearchSampler(Animation::DIVE))
			m_blendAnimation.animationBlend.ResetAnimationSampler(0);
		m_blendAnimation.animationBlend.AddSampler(Animation::RUN, m_model);
	}

	CharacterAI* enemy = MESSENGER.CallEnemyInstance(EnemyType::Boss);
	float distance = ToDistVec3(enemy->GetWorldTransform().position - m_transformParm.position);
	int count = static_cast<int>(m_selectMovePoint.size());
	if (distance < kSafeAreaRadius * 0.5f || count == 0)
	{
		m_currentPlanList = m_agentAI.OperationFlowStart(this);
		m_hasBlendAnim = false;
		m_selectPoint = 0;
		m_goalTimer = 0.0f;
		m_currentTask = 0;
		m_selectMovePoint.clear();
		return false;
	}

	VECTOR3F goal = m_selectMovePoint.at(m_selectPoint);
	m_hasRotated = Rotate(goal, 0.15f);
	VECTOR3F front = VECTOR3F(sinf(m_transformParm.angle.y), 0.0f, cosf(m_transformParm.angle.y));
	VECTOR3F vector = front * m_moveParm.accle;
	m_transformParm.position += vector * m_elapsedTime;
	m_transformParm.WorldUpdate();
	
	m_goalTimer += m_elapsedTime;
	distance = ToDistVec3(goal - m_transformParm.position);
	if (distance <= 2.0f)
	{
		m_state = 0;
		m_hasRotated = false;
		++m_selectPoint;
		m_goalTimer = 0.0f;
		if (m_selectPoint >= count)
		{
			m_blendAnimation.animationBlend.AddSampler(Animation::AIM, m_model);
			m_hasBlendAnim = false;
			m_selectPoint = 0;
			m_selectMovePoint.clear();
			return true;
		}
	}

	if (m_goalTimer > m_goalMaxTimer)
	{
		m_currentPlanList = m_agentAI.OperationFlowStart(this);
		m_hasBlendAnim = false;
		m_selectPoint = 0;
		m_goalTimer = 0.0f;
		m_currentTask = 0;
		m_selectMovePoint.clear();
		return false;
	}

	return false;
}

bool Archer::SetArrow()
{	
	CharacterAI* enemy = MESSENGER.CallEnemyInstance(EnemyType::Boss);
	VECTOR3F ePosition = enemy->GetWorldTransform().position;
	if (!m_hasBlendAnim)
		m_hasBlendAnim = JudgeBlendRatio();

	if (!m_hasRotated)
		m_hasRotated = Rotate(ePosition,0.3f,true);

//	if(m_blendAnimation.animationBlend.GetSampler().size() == 1)
	{
		const int selectMesh = 2;
		const int selectBone = 36;
		FLOAT4X4 blendBone = m_blendAnimation.animationBlend._blendLocals[selectMesh].at(selectBone);
		FLOAT4X4 modelAxisTransform = m_model->_resource->axisSystemTransform;
		FLOAT4X4 getBone = blendBone * modelAxisTransform * m_transformParm.world;
		VECTOR3F position = { getBone._41,getBone._42,getBone._43 };
		VECTOR3F angle = m_transformParm.angle;
		DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
		FLOAT4X4 rotationM = {};
		DirectX::XMStoreFloat4x4(&rotationM, R);
		VECTOR3F zAxis = { rotationM._31,rotationM._32,rotationM._33 };
		zAxis = NormalizeVec3(zAxis);
		if (!m_isSetArrow)
		{
		if(m_attackArrow.m_arrowType == ArrowType::NORMAL)
			m_statusParm.attackPoint = m_attackArrow.m_attackPoint[0];
		else
		{
			m_statusParm.attackPoint = m_attackArrow.m_attackPoint[1];
			m_attackArrow.m_useArrowTimer = 0.0f;
		}
			m_arrow->PrepareForArrow(position, m_transformParm.angle, zAxis);
			m_isSetArrow = true;
		}
		m_arrow->GetArrowParam()[0].first.position = position;
		m_arrow->GetArrowParam()[0].first.angle = m_transformParm.angle;
		m_arrow->GetArrowParam()[0].second.velocity = zAxis;
		m_arrow->GetArrowParam()[0].first.CreateWorld();
	}

	if (m_hasRotated && m_hasBlendAnim)
	{
		m_isSetArrow = false;
		return true;
	}
	return false;
}

bool Archer::Shoot()
{
	if (!m_hasShoot && !m_blendAnimation.animationBlend.SearchSampler(Animation::SHOOT))
	{
		m_blendAnimation.animationBlend.ResetAnimationSampler(0);
		m_blendAnimation.animationBlend.AddSampler(Animation::SHOOT, m_model);
		m_hasBlendAnim = false;
		m_hasShoot = true;
	}

	if (!m_hasBlendAnim)
		m_hasBlendAnim = JudgeBlendRatio(false);
	else
	{
		m_state = 0;
		m_hasBlendAnim = false;
		++m_shootCount;
		return true;
	}

	return false;
}

bool Archer::FindDirectionToAvoid()
{
	CharacterAI* enemy = MESSENGER.CallEnemyInstance(EnemyType::Boss);

	std::vector<int> entryPointID;
	Collision::Sphere stage,point,target;
	stage.position = {};
	stage.radius = 81.0f;
	stage.scale = 1.0f;

	point.radius = 1.0f;
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
		m_controlPoint[i].second.x = m_transformParm.position.x + cosf(radius * 0.01745f) * (kSafeAreaRadius * 0.7f);
		m_controlPoint[i].second.z = m_transformParm.position.z + sinf(radius * 0.01745f) * (kSafeAreaRadius * 0.7f);
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
				VECTOR3F(2.0f, 2.0f, 2.0f), color);
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
	if (m_blendAnimation.animationBlend.SearchSampler(Animation::DIVE))
		m_blendAnimation.animationBlend.ResetAnimationSampler(0);
	m_blendAnimation.animationBlend.SetAnimationSpeed(1.6f);
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

	if (m_blendAnimation.animationBlend.SearchSampler(Animation::DIVE))
	{
		m_hasBlendAnim = JudgeBlendRatio(false);
	}
	else
	{
		if (!m_blendAnimation.animationBlend.SearchSampler(Animation::HIT) &&
			!m_blendAnimation.animationBlend.SearchSampler(Animation::HITBIG))
			m_blendAnimation.animationBlend.ResetAnimationSampler(0);
		m_blendAnimation.animationBlend.AddSampler(Animation::DIVE, m_model);
	}

	if (m_hasBlendAnim)
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
	if (!m_blendAnimation.animationBlend.SearchSampler(Animation::HEAL))
	{
		if (!m_blendAnimation.animationBlend.SearchSampler(Animation::HIT) &&
			!m_blendAnimation.animationBlend.SearchSampler(Animation::HITBIG) &&
			!m_blendAnimation.animationBlend.SearchSampler(Animation::DIVE))
			m_blendAnimation.animationBlend.ResetAnimationSampler(0);
		m_blendAnimation.animationBlend.AddSampler(Animation::HEAL, m_model);
		m_canRecover = false;
		m_recoverTimer = 0.0f;

		float targetHp, targetMaxHp = 0.0f;
		CharacterAI* target = nullptr;
		if (m_recoverParm.isPlayer)
		{
			target = MESSENGER.CallPlayerInstance(PlayerType::Fighter);
			targetHp = target->GetStatus().life;
			targetMaxHp = target->GetStatus().maxLife;
			m_selectCharacter = 0;
			m_recoverTarget = target->GetWorldTransform().position;
		}
		else
		{
			targetHp = m_statusParm.life;
			targetMaxHp = m_statusParm.maxLife;
			m_selectCharacter = 1;
			m_recoverTarget = m_transformParm.position;
		}

		float hpRatio = targetHp / targetMaxHp;
		float healValue = 0.0f;
		if (m_recoverParm.standardLv > hpRatio)
		{
			healValue = m_recoverParm.healValue[1];
			m_recoverMaxTimer = m_recoverParm.recoverMaxTime[1];
		}
		else
		{
			healValue = m_recoverParm.healValue[0];
			m_recoverMaxTimer = m_recoverParm.recoverMaxTime[0];
		}

		MESSENGER.MessageToLifeUpdate(targetHp + healValue, targetMaxHp,
			UIActLabel::LIFE_P, m_selectCharacter);

		if (m_recoverParm.isPlayer)
		{
			float& hp = target->GetStatus().life;
			hp += healValue;
			if (hp > targetMaxHp)
				hp = targetMaxHp;
		}
		else
		{
			m_statusParm.life += healValue;
			if (m_statusParm.life > targetMaxHp)
				m_statusParm.life = targetMaxHp;
		}

	}
	else
	{
		bool hasAnimation = JudgeBlendRatio(false);
		if (hasAnimation)
		{
			uint32_t  currentAnimationFrame = m_blendAnimation.animationBlend.GetAnimationTime(0);
			if (!m_onEffect)
			{
				MESSENGER.MessageToRecoverEffect(m_recoverTarget, 1, m_selectCharacter);
				m_onEffect = true;
			}

			if (currentAnimationFrame == 110)
			{
				m_onEffect = false;
				return true;
			}
		}
	}

	return false;
}

bool Archer::Revival()
{
	return false;
}

void Archer::ImGui(ID3D11Device* device)
{
#if _DEBUG //_DEBUG

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
			CharacterAI* enemy = MESSENGER.CallEnemyInstance(EnemyType::Boss);
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
		static bool activeFunction[7];
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
		if (ImGui::Button("Recover"))
			activeFunction[6] = true;

		if (activeFunction[6])
		{
			if ((Heal()))
				activeFunction[6] = false;
		}
	}

	//**************************************
	// Domain
	//**************************************
	if (ImGui::CollapsingHeader("Gamemaker"))
	{
		m_agentAI.ImGui(this);
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

	if (ImGui::CollapsingHeader("Recover"))
	{
		ImGui::SliderFloat2("HealValue", m_recoverParm.healValue, 0.0f, m_statusParm.maxLife);
		ImGui::SliderFloat2("RecoverMaxTime", m_recoverParm.recoverMaxTime, 0.0f, 10.0f);
		ImGui::SliderFloat("StandardLv", &m_recoverParm.standardLv, 0.0f, 1.0f);
		if (ImGui::Button("Player"))
			m_recoverParm.isPlayer = true;
		ImGui::SameLine();
		if (ImGui::Button("Archer"))
			m_recoverParm.isPlayer = false;

	}

	if (ImGui::CollapsingHeader("Life"))
	{
		static float damageOffset = 0.0f;
		ImGui::SliderFloat("DamageValue", &damageOffset, 0, m_statusParm.maxLife);

		if (ImGui::Button("Update"))
		{
			m_statusParm.life -= damageOffset;
			MESSENGER.MessageToLifeUpdate(m_statusParm.life, m_statusParm.maxLife,
				UIActLabel::LIFE_P, 1);
		}
	}

	if (ImGui::CollapsingHeader("AttackArrow"))
	{
		ImGui::SliderFloat2("ArrowAttackPoint", m_attackArrow.m_attackPoint, 0.0f, 25.0f);
		ImGui::SliderFloat("UseArrowMaxTimer", &m_attackArrow.m_useArrowMaxTimer, 0.0f, 10.0f);
	}

	if (ImGui::CollapsingHeader("Active View"))
	{
		ImGui::BulletText(u8"現在のやることリスト");
		for (auto& task : m_currentPlanList)
		{
			ImGui::BulletText("%s", task->GetTaskName().data());
		}
		int currentTask = m_currentTask >= m_currentPlanList.size() ? m_currentTask - 1: m_currentTask;
		ImGui::BulletText(u8"現在やっているタスク->%s", m_currentPlanList[currentTask]->GetTaskName().data());

		for (auto& point : m_selectMovePoint)
		{
			float position[] = { point.x,point.y,point.z };
			ImGui::InputFloat3("Point", position);
		}
		ImGui::InputFloat("GoalTImer", &m_goalTimer);
		ImGui::BulletText(u8"目的地->%d", m_selectPoint);
	}


	ImGui::Checkbox("Active", &m_active);
	ImGui::Text("attackHitCount->%d", m_judgeElementPram.attackHitCount);
	ImGui::Text("m_canRecover->%d", m_canRecover);
	ImGui::SliderFloat("BlendRatio", &m_blendAnimation.animationBlend._blendRatio, 0.0f, 1.0f);
#endif
	ImGui::End();


#else
ReleaseAdjust();
#endif
}

void Archer::ReleaseAdjust()
{
	float width = 1600.0f;
	float height = 100.0f;
	ImGui::SetNextWindowPos(ImVec2(width, height));
	ImGui::Begin("ArcherCharacter");

	ImGui::BulletText(u8"選択されたタスク群");
	{
		ImGui::BeginChild("Test", ImVec2(200, 100), true);
	for (auto& task : m_currentPlanList)
	{
		ImGui::Text("%s", task->GetTaskName().data());
	}
	ImGui::EndChild();
	}
	ImGui::BulletText(u8"現在のタスク");
	{
		std::string taskName = m_currentPlanList[m_currentTask]->GetTaskName();
		ImGui::Text("%s", taskName.data());
	}
	ImGui::BulletText(u8"次のタスク");
	{
		int nextTask = m_currentTask + 1;
		int maxtaskList = m_currentPlanList.size();
		std::string taskName;
		if (nextTask >= maxtaskList)
			taskName = "これで終わり";
		else
			 taskName = m_currentPlanList[nextTask]->GetTaskName();
		ImGui::Text("%s", taskName.data());
	}

	ImGui::Checkbox(u8"球体可視化", &m_isAdjust);
	ImGui::BulletText(u8"攻撃地点->球体");
	{
		ImGui::BeginChild("攻撃種類", ImVec2(200, 80), true);
		ImGui::Text(u8"青->攻撃可能ポイント");
		ImGui::Text(u8"赤->攻撃不可ポイント");
		ImGui::Text(u8"緑->選択した攻撃ポイント");
		ImGui::EndChild();
	}

	ImGui::BulletText(u8"逃走方向->球体");
	{
		ImGui::BeginChild("逃走種類", ImVec2(200, 80), true);
		ImGui::Text(u8"青->逃走可能ポイント");
		ImGui::Text(u8"赤->逃走不可ポイント");
		ImGui::Text(u8"緑->選択した逃走ポイント");
		ImGui::EndChild();
	}
	ImGui::End();
}

void AttackArrow::UpdateTime(float& elapsedTime)
{
	if (m_useArrowTimer > m_useArrowMaxTimer)
		m_arrowType = ArrowType::STRONG;
	else
	{
		m_useArrowTimer += elapsedTime;
		m_arrowType = ArrowType::NORMAL;
	}
}
