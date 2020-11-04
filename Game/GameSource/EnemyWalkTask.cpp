#include "EnemyWalkTask.h"
#include "MessengTo.h"
#include "Enemy.h"

void EnemyWalkTask::Run(Enemy* enemy)
{
#if 0
	auto& animation = enemy->GetBlendAnimation();
	switch (m_moveState)
	{
	case Action::START:
		m_taskState = TASK_STATE::RUN;
		if (animation.animationBlend.GetSampler().size() != 2)
		{
			//m_animNo = JudgeTurnChace(enemy);
			m_animNo = static_cast<int>(Enemy::Animation::RUN);
			animation.animationBlend.AddSampler(m_animNo, enemy->GetModel());
			animation.animationBlend.ResetAnimationFrame();
		}
		else
		{
			if (JudgeBlendRatio(animation))
			{
				++m_moveState;
				++enemy->GetJudgeElement().moveCount;
				m_animNo = Enemy::Animation::IDLE;
			}
		}
		m_walkTime = 0.0f;
		break;
	case Action::WALK:
		if (IsTurnChase(enemy))
		{
			m_animNo = Enemy::Animation::IDLE;
			animation.animationBlend.AddSampler(m_animNo, enemy->GetModel());
			animation.animationBlend.ResetAnimationFrame();
			++m_moveState;
		}
		break;
	case Action::END:
		if (JudgeBlendRatio(animation))
		{
			animation.animationBlend.ResetAnimationSampler(0);
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

bool EnemyWalkTask::JudgeBlendRatio(CharacterParameter::BlendAnimation& animation)
{
	animation.animationBlend._blendRatio += kBlendValue;//magicNumber
	if (animation.animationBlend._blendRatio >= animation.blendRatioMax)//magicNumber
	{
		animation.animationBlend._blendRatio = 0.0f;
		animation.animationBlend.ResetAnimationSampler(0);
		animation.animationBlend.ReleaseSampler(0);
		return true;
	}

	return false;
}

bool EnemyWalkTask::IsTurnChase(Enemy* enemy)
{
	auto& animation = enemy->GetBlendAnimation();
	int targetID = enemy->GetJudgeElement().targetID;
	auto& player = MESSENGER.CallPlayerInstance(targetID);
	if (animation.animationBlend.GetSampler().size() == 2)
	{
		animation.animationBlend._blendRatio += kBlendValue;//magicNumber
		if (animation.animationBlend._blendRatio >= animation.blendRatioMax)//magicNumber
		{
			animation.animationBlend._blendRatio = 0.0f;
			animation.animationBlend.ResetAnimationSampler(0);
			animation.animationBlend.ReleaseSampler(0);
		}
	}

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
		return true;
	else
	{
		m_walkTime += enemy->GetElapsedTime();
		auto playerTransform = player->GetWorldTransform();
		auto& enemyTransform = enemy->GetWorldTransform();

		VECTOR3F enemyPosition = enemyTransform.position;
		VECTOR3F playerPosition = playerTransform.position;
		VECTOR3F nDistance = NormalizeVec3(playerPosition - enemyPosition);
		VECTOR3F velocity = nDistance * m_accel;
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

		if (distance < m_maxDirection)
			return true;
	}
#endif 
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
		return Enemy::Animation::RightTurn;
	else
		return Enemy::Animation::LeftTurn;

	return 0;
}

uint32_t EnemyWalkTask::JudgePriority(const int id, const VECTOR3F playerPos) 
{
	return m_priority;
}
