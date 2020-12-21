#include "EnemyWalkTask.h"
#include "MessengTo.h"
#include "Enemy.h"
#include "Collision.h"

void EnemyWalkTask::Run(Enemy* enemy)
{
#if 1
	auto& animation = enemy->GetBlendAnimation();
	switch (m_moveState)
	{
	case Action::START:
		m_taskState = TASK_STATE::RUN;
		m_animNo = Enemy::Animation::STEP;
		m_walkTime = 0.0f;
		m_blendValue = 0.1f;
		m_hasFinishedBlend = false;
		++enemy->GetJudgeElement().moveCount;
		animation.animationBlend.SetAnimationSpeed(1.1f);
		animation.animationBlend.AddSampler(m_animNo, enemy->GetModel());
		animation.animationBlend.ResetAnimationFrame();
		JudgeVectorDirection(enemy);
		++m_moveState;
	break;
	case Action::STEP:
		if (!m_hasFinishedBlend)
			m_hasFinishedBlend = JudgeBlendRatio(animation);
		else
		{
			if (StepMove(enemy, m_hasFinishedBlend))
			{
				m_hasFinishedBlend = false;
				bool isNear = IsNearTarget(enemy);
				m_animNo = isNear ? Enemy::Animation::IDLE : Enemy::Animation::RUN;
				m_moveState = isNear ? Action::END : Action::WALK;
				m_blendValue = isNear ? 0.07f : 0.1f;
				float animationSpeed = isNear ? 1.0f : 1.1f;
				animation.animationBlend.AddSampler(m_animNo, enemy->GetModel());
				animation.animationBlend.SetAnimationSpeed(animationSpeed);
				animation.animationBlend.ResetAnimationFrame();
				++enemy->GetJudgeElement().moveCount;
			}
		}
	break;
	case Action::WALK:
		if (Walk(enemy))
		{
			m_animNo = Enemy::Animation::IDLE;
			animation.animationBlend.AddSampler(m_animNo, enemy->GetModel());
			animation.animationBlend.ResetAnimationFrame();
			animation.animationBlend.SetAnimationSpeed(1.0f);
			m_blendValue = 0.07f;
			++m_moveState;
		}
		break;
	case Action::END:
		if (JudgeBlendRatio(animation))
		{
			m_moveState = 0;
			m_taskState = TASK_STATE::END;

			auto& players = MESSENGER.CallPlayersInstance();
			for (auto& player : players)
			{
				player->GetJudgeElement().attackHitCount = 0;
			}
		}
	}

#endif
}

bool EnemyWalkTask::JudgeBlendRatio(CharacterParameter::BlendAnimation& animation, const bool isLoop)
{
	animation.animationBlend._blendRatio += m_blendValue;
	if (animation.animationBlend._blendRatio >= animation.blendRatioMax)//magicNumber
	{
		animation.animationBlend._blendRatio = 0.0f;
		size_t samplerSize = animation.animationBlend.GetSampler().size();
		for (size_t i = 0; i < samplerSize; ++i)
		{
			animation.animationBlend.ReleaseSampler(0);
		}
		if (!isLoop)
			animation.animationBlend.FalseAnimationLoop(0);
		return true;
	}

	return false;
}

bool EnemyWalkTask::Walk(Enemy* enemy)
{
	auto& animation = enemy->GetBlendAnimation();
	int targetID = enemy->GetJudgeElement().targetID;
	auto& player = MESSENGER.CallPlayerInstance(targetID);

	if (!m_hasFinishedBlend)
		m_hasFinishedBlend = JudgeBlendRatio(animation,true);

#if 0
	auto playerTransform = player->GetWorldTransform();
	auto& enemyTransform = enemy->GetWorldTransform();

	VECTOR3F enemyPosition = enemyTransform.position;
	VECTOR3F playerPosition = playerTransform.position;
	float direction = ToDistVec3(playerPosition - enemyPosition);
	float speed = direction / kFIveSecond;
	FLOAT4X4 axisTransform = enemy->GetModel()->_resource->axisSystemTransform;
	DirectX::XMMATRIX mEnemy = DirectX::XMLoadFloat4x4(&(axisTransform * enemyTransform.world));
	DirectX::XMVECTOR qEnemy = DirectX::XMQuaternionRotationMatrix(mEnemy);

	DirectX::XMVECTOR vForward = mEnemy.r[2];
	DirectX::XMVECTOR vUp = mEnemy.r[1];
	DirectX::XMVECTOR vRight = mEnemy.r[0];

	DirectX::XMVECTOR vQuaternion = DirectX::XMQuaternionRotationAxis(vUp, kTurnValue);

	DirectX::XMVECTOR rotQ = DirectX::XMVectorMultiply(vQuaternion, qEnemy);

	DirectX::XMMATRIX rot = DirectX::XMMatrixRotationQuaternion(rotQ);
	FLOAT4X4 matrix;
	DirectX::XMStoreFloat4x4(&matrix, rot);

	VECTOR3F right = { matrix._11 ,matrix._12 ,matrix._13 };
	VECTOR3F front = { matrix._31 ,matrix._32 ,matrix._33 };

	right = NormalizeVec3(right);
	front = NormalizeVec3(front);

	static float rotALL = 0;
	rotALL += kTurnValue;
	

	float diameter = ToDistVec3(playerPosition - enemyPosition) * 2.0f;//’¼Œa
	static float range = diameter;
	float axisXVelocityX = /*diameter * 3.14f * */range * 3.14f * (kTurnValue / (static_cast<float>(2 * M_PI))) * cosf(rotALL) * (right.x * -1.0f);
	float axisXVelocityZ = /*diameter * 3.14f * */range * 3.14f * (kTurnValue / (static_cast<float>(2 * M_PI))) * cosf(rotALL) * (right.z * -1.0f);
	float axisZVelocityX = /*diameter * 3.14f * */range * 3.14f * (kTurnValue / (static_cast<float>(2 * M_PI))) * sinf(rotALL) * (front.x * -1.0f);
	float axisZVelocityZ = /*diameter * 3.14f * */range * 3.14f *(kTurnValue / (static_cast<float>(2 * M_PI))) * sinf(rotALL) * (front.z * -1.0f);

//	enemyTransform.position.x += (axisXVelocityX + axisZVelocityX);
	enemyTransform.position.x -= (axisXVelocityX + axisXVelocityZ);
	enemyTransform.position.z += (axisZVelocityX + axisZVelocityZ);

	VECTOR3F normalizeDist = NormalizeVec3(playerPosition - enemyPosition);
	float dot = DotVec3(front, normalizeDist);
	float fRot = 1.0f - dot;
	float limit = enemy->GetMove().turnSpeed;
	if (fRot > limit)
	{
		fRot = limit;
	}

	enemyTransform.angle.y += fRot;
	enemyTransform.WorldUpdate();

	if (m_walkTime >= kWalkTimer)
		return true;
	//else
	//	m_walkTime += enemy->GetElapsedTime();
#elif 0
	auto playerTransform = player->GetWorldTransform();
	auto& enemyTransform = enemy->GetWorldTransform();

	VECTOR3F enemyPosition = enemyTransform.position;
	VECTOR3F playerPosition = playerTransform.position;

	VECTOR3F distance = playerPosition - enemyPosition;
	DirectX::XMVECTOR vDistnace = DirectX::XMLoadFloat3(&distance);
	static float rotALL = 0;
	rotALL += kTurnValue;

	DirectX::XMVECTOR vEnemy = DirectX::XMLoadFloat3(&enemyTransform.position);
	DirectX::XMMATRIX lmPlayer = DirectX::XMMatrixInverse(0, DirectX::XMLoadFloat4x4(&playerTransform.world));
	vEnemy = DirectX::XMVector3TransformCoord(vEnemy, lmPlayer);
	vDistnace = DirectX::XMVector3TransformCoord(vDistnace, lmPlayer);
	DirectX::XMStoreFloat3(&enemyTransform.position, vEnemy);
	DirectX::XMStoreFloat3(&distance, vEnemy);
	float radius = ToDistVec3(distance);
	enemyTransform.position.x = sinf(rotALL * 0.01745f) * radius;
	enemyTransform.position.z = cosf(rotALL * 0.01745f) * radius;
	vEnemy = DirectX::XMLoadFloat3(&enemyTransform.position);

	vEnemy = DirectX::XMVector3TransformCoord(vEnemy, DirectX::XMLoadFloat4x4(&playerTransform.world));
	DirectX::XMStoreFloat3(&enemyTransform.position, vEnemy);

	distance = playerPosition - enemyPosition;
	float dot = atan2f(distance.x, distance.z);
	enemyTransform.angle.y = dot;
	enemyTransform.WorldUpdate();

#elif 1
	//^‚Á‚·‚®‚Å‚¢‚¢‚æ
	if (m_walkTime > kWalkTimer)
	{
		m_hasFinishedBlend = false;
		return true;
	}
	else
	{
		m_walkTime += enemy->GetElapsedTime();
		auto playerTransform = player->GetWorldTransform();
		auto& enemyTransform = enemy->GetWorldTransform();

		VECTOR3F enemyPosition = enemyTransform.position;
		VECTOR3F playerPosition = playerTransform.position;
		VECTOR3F nDistance = NormalizeVec3(playerPosition - enemyPosition);
		VECTOR3F velocity = nDistance * m_accel[1];
		enemy->GetWorldTransform().position += velocity * enemy->GetElapsedTime();

		VECTOR3F angle = enemy->GetWorldTransform().angle;
		VECTOR3F front = VECTOR3F(sinf(angle.y), 0.0f, cosf(angle.y));
		front = NormalizeVec3(front);

		float dot = DotVec3(front, nDistance);
		if (!std::isfinite(dot))
			dot = 0.0f;
		float rot = 1.0f - dot;
		float limit = enemy->GetMove().turnSpeed;
		if (rot > limit)
		{
			rot = limit;
		}
		VECTOR3F cross = CrossVec3(front, nDistance);
		if (cross.y > 0.0f)
		{
			enemyTransform.angle.y += rot;
		}
		else
		{
			enemyTransform.angle.y -= rot;
		}
		enemyTransform.WorldUpdate();

		enemy->GetWorldTransform().WorldUpdate();

		float distance = ToDistVec3(playerPosition - enemyPosition);

		if (distance < 25.0f)
		{
			m_hasFinishedBlend = false;
			return true;
		}

	}
#endif 
	return false;
}

bool EnemyWalkTask::IsTurnChase(Enemy* enemy)
{
	//auto& animation = enemy->GetBlendAnimation();
	//static bool blendFinish = false;
	//if (!blendFinish)
	//	blendFinish = JudgeBlendRatio(animation, true);
	//VECTOR3F enemyPosition = enemy->GetWorldTransform().position;
	//float direction = ToDistVec3(m_targetPosition - enemyPosition);
	//VECTOR3F normalizeDist = NormalizeVec3(m_targetPosition - enemyPosition);
	//VECTOR3F angle = enemy->GetWorldTransform().angle;
	//VECTOR3F front = VECTOR3F(sinf(angle.y), 0.0f, cosf(angle.y));
	//front = NormalizeVec3(front);
	//float dot = DotVec3(front, normalizeDist);
	//float rot = 1.0f - dot;
	//float limit = enemy->GetMove().turnSpeed;
	//if (rot > limit)
	//	rot = limit;
	//uint32_t currentAnimationTime = enemy->GetBlendAnimation().animationBlend.GetAnimationTime(0);
	//float cosTheta = acosf(dot);
	//float frontValue = enemy->GetStandardValue().viewFrontValue;
	//if (cosTheta <= frontValue && currentAnimationTime == 0)
	//{
	//	blendFinish = false;
	//	return true;
	//}
	//if (kTurnChanseTimer > currentAnimationTime)
	//{
	//	auto& enemyTransform = enemy->GetWorldTransform();
	//	if (m_animNo == Enemy::Animation::LEFT_TURN)
	//		rot *= -1;
	//	enemyTransform.angle.y += rot;
	//	enemyTransform.WorldUpdate();
	//}

	return false;
}

int EnemyWalkTask::JudgeTurnChace(Enemy* enemy)
{
	int targetID = enemy->GetJudgeElement().targetID;
	auto& player = MESSENGER.CallPlayerInstance(targetID);

	VECTOR3F playerPosition = player->GetWorldTransform().position;
	VECTOR3F enemyPosition = enemy->GetWorldTransform().position;

	float direction = ToDistVec3(playerPosition - enemyPosition);
	VECTOR3F normalizeDist = NormalizeVec3(playerPosition - enemyPosition);

	VECTOR3F angle = enemy->GetWorldTransform().angle;
	VECTOR3F front = VECTOR3F(sinf(angle.y), 0.0f, cosf(angle.y));
	front = NormalizeVec3(front);

	VECTOR3F cross = CrossVec3(front, normalizeDist);
	if (cross.y > 0.0f)
		return Enemy::Animation::RIGHT_TURN;
	else
		return Enemy::Animation::LEFT_TURN;

	return 0;
}

void EnemyWalkTask::JudgeVectorDirection(Enemy* enemy)
{
	m_nVecToTarget = {};
	VECTOR3F enemyPosition = enemy->GetWorldTransform().position;
	VECTOR3F enemyAngle = enemy->GetWorldTransform().angle;
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(enemyAngle.x, enemyAngle.y, enemyAngle.z);
	FLOAT4X4 rotationM = {};
	DirectX::XMStoreFloat4x4(&rotationM, R);
	VECTOR3F enemyFront = { rotationM._31,rotationM._32,rotationM._33 };


	auto& player = MESSENGER.CallPlayerInstance(enemy->GetJudgeElement().targetID);
	FLOAT4X4 targetWorld = player->GetWorldTransform().world;
	VECTOR3F targetAngle = player->GetWorldTransform().angle;
	R = DirectX::XMMatrixRotationRollPitchYaw(targetAngle.x, targetAngle.y, targetAngle.z);
	rotationM = {};
	DirectX::XMStoreFloat4x4(&rotationM, R);
	VECTOR3F targetFront = { rotationM._31,rotationM._32,rotationM._33 };
	VECTOR3F targetPosition = { targetWorld._41,targetWorld._42,targetWorld._43 };
	VECTOR3F maxAccel = player->GetMove().accle;

	VECTOR3F targetVeclocity = targetFront * (maxAccel * 46.0f);
	VECTOR3F v = targetVeclocity * enemy->GetElapsedTime();
	targetPosition += targetVeclocity /** enemy->GetElapsedTime()*/;

	Collision::Circle stageRange;
	stageRange.radius = 81.0f;
	stageRange.position = {};
	stageRange.scale = 1.0f;
	Collision coll;

//	if (!coll.JudgeCircleAndpoint(stageRange, VECTOR2F(targetPosition.x, targetPosition.z)))
	m_nVecToTarget = NormalizeVec3(targetPosition - enemyPosition);
	float dot = DotVec3(enemyFront, m_nVecToTarget);
	if (-0.8f > dot && dot > -1.0f)
	{
		targetPosition = { targetWorld._41,targetWorld._42,targetWorld._43 };
		m_nVecToTarget = NormalizeVec3(targetPosition - enemyPosition);

	}

}

bool EnemyWalkTask::StepMove(Enemy* enemy, bool isBlendFinish)
{
	uint32_t currentAnimationTime = enemy->GetBlendAnimation().animationBlend.GetAnimationTime(0);

	auto& enemyTransform = enemy->GetWorldTransform();
	enemy->GetMove().velocity = m_nVecToTarget * m_accel[0];
	enemyTransform.position += enemy->GetMove().velocity * enemy->GetElapsedTime();

	if (currentAnimationTime < 40)
	{
		VECTOR3F angle = enemy->GetWorldTransform().angle;
		VECTOR3F front = VECTOR3F(sinf(angle.y), 0.0f, cosf(angle.y));
		front = NormalizeVec3(front);

		float dot = DotVec3(front, m_nVecToTarget);
		float rot = 1.0f - dot;
		float limit = enemy->GetMove().turnSpeed+0.04f;
		if (rot > limit)
			rot = limit;

		VECTOR3F cross = CrossVec3(front, m_nVecToTarget);
		if (cross.y > 0.0f)
		{
			enemyTransform.angle.y += rot;
		}
		else
		{
			enemyTransform.angle.y -= rot;
		}

	}

	if (isBlendFinish)
	{
		if (currentAnimationTime >= 84)
			return true;

		if (currentAnimationTime >= 60)
		{
			auto& player = MESSENGER.CallPlayerInstance(enemy->GetJudgeElement().targetID);
			VECTOR3F targetPosition = player->GetWorldTransform().position;
			VECTOR3F targetDistance = targetPosition - enemyTransform.position;
			VECTOR3F targetNormal = NormalizeVec3(targetDistance);
			float targetDist = ToDistVec3(targetDistance);
			VECTOR3F angle = enemy->GetWorldTransform().angle;
			VECTOR3F front = VECTOR3F(sinf(angle.y), 0.0f, cosf(angle.y));
			front = NormalizeVec3(front);

			float dot = DotVec3(front, targetNormal);
			float rot = 1.0f - dot;
			float limit = enemy->GetMove().turnSpeed + 0.04f;
			if (rot > limit)
				rot = limit;

			VECTOR3F cross = CrossVec3(front, targetNormal);
			if (cross.y > 0.0f)
			{
				enemyTransform.angle.y += rot;
			}
			else
			{
				enemyTransform.angle.y -= rot;
			}
		}

	}
	enemyTransform.WorldUpdate();

	return false;
}

bool EnemyWalkTask::IsNearTarget(Enemy* enemy)
{
	int targetID = enemy->GetJudgeElement().targetID;
	auto& player = MESSENGER.CallPlayerInstance(targetID);

	auto playerTransform = player->GetWorldTransform();
	auto& enemyTransform = enemy->GetWorldTransform();

	VECTOR3F enemyPosition = enemyTransform.position;
	VECTOR3F playerPosition = playerTransform.position;
	float distance = ToDistVec3(playerPosition - enemyPosition);

	if (distance < 24.0f)
		return true;

	return false;
}

uint32_t EnemyWalkTask::JudgePriority(const int id, const VECTOR3F playerPos) 
{
	return m_priority;
}
