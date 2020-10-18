#include "EnemyFarAttack1Task.h"
#include "MessengTo.h"
#include "Enemy.h"

void EnemyFarAttack1Task::Run(Enemy* enemy)
{
	auto& animation = enemy->GetBlendAnimation();

	switch (m_moveState)
	{
	case 0:
	{
		m_taskState = TASK_STATE::START;
		animation.animationBlend.AddSampler(16, enemy->GetModel());
		animation.animationBlend.ResetAnimationFrame();
		enemy->GetStatus().isAttack = false;
		auto player = MESSENGER.CallPlayerInstance(m_targetID);
		m_targetPosition = player->GetWorldTransform().position;
		m_targetNormal = {};
		m_targetSpeed = {};
		m_chaseTimer = 0;
		++m_moveState;
		m_isNear = false;
		m_isSampler = false;
	}
	break;
	case 1:
	{
		m_taskState = TASK_STATE::RUN;

		if (JudgeBlendRatio(animation))
		{
			m_isSampler = true;
		}

		if (m_isSampler)
		{
			uint32_t currentAnimationTime = enemy->GetBlendAnimation().animationBlend.GetAnimationTime(0);
			if (currentAnimationTime >= 150)
			{
				enemy->GetBlendAnimation().animationBlend.AddSampler(2, enemy->GetModel());
				++m_moveState;
				m_isSampler = false;
			}
		}

		auto& enemyTransform = enemy->GetWorldTransform();

		auto& player = MESSENGER.CallPlayerInstance(m_targetID);
		VECTOR3F targetPosition = player->GetWorldTransform().position;
		VECTOR3F targetDistance = targetPosition - enemyTransform.position;
		m_targetNormal = NormalizeVec3(targetDistance);
		float targetDist = ToDistVec3(targetDistance);
		m_targetSpeed = targetDist / 240.0f;
		VECTOR3F angle = enemy->GetWorldTransform().angle;
		VECTOR3F front = VECTOR3F(sinf(angle.y), 0.0f, cosf(angle.y));
		front = NormalizeVec3(front);

		float dot = DotVec3(front, m_targetNormal);
		float rot = 1.0f - dot;
		float limit = enemy->GetMove().turnSpeed;
		if (rot > limit)
		{
			rot = limit;
		}

		VECTOR3F cross = CrossVec3(front, m_targetNormal);
		if (cross.y > 0.0f)
		{
			enemyTransform.angle.y += rot;
		}
		else
		{
			enemyTransform.angle.y -= rot;
		}

		enemyTransform.WorldUpdate();

	}
	break;
	case 2:
	{
		if (animation.animationBlend.GetSampler().size() == 2)
		{
			animation.animationBlend._blendRatio += 0.045f;//magicNumber
			if (animation.animationBlend._blendRatio >= animation.blendRatioMax)//magicNumber
			{
				animation.animationBlend._blendRatio = 0.0f;
				animation.animationBlend.ResetAnimationSampler(0);
				animation.animationBlend.ReleaseSampler(0);
			}
		}

		if (m_isNear)
		{
			animation.animationBlend.AddSampler(11, enemy->GetModel());
			++m_moveState;
		}
		else
		{
			auto& enemyTransform = enemy->GetWorldTransform();

			if (m_chaseTimer < 210.0f)
			{

				enemy->GetMove().velocity = m_targetNormal * m_targetSpeed;
				enemyTransform.position += enemy->GetMove().velocity;

				enemyTransform.WorldUpdate();

			}
			else
			{
				m_isNear = true;
				m_chaseTimer = 0;
			}

			if (m_chaseTimer < 180.0f)
			{
				auto& runAttack = enemy->GetCollision().at(4);
				int runAttackMesh = runAttack.GetCurrentMesh(0);
				int runAttackBone = runAttack.GetCurrentBone(0);
				FLOAT4X4 getAttackBone = animation.animationBlend._blendLocals[runAttackMesh].at(runAttackBone);
				FLOAT4X4 modelAxisTransform = enemy->GetModel()->_resource->axisSystemTransform;
				FLOAT4X4 worldTransform = enemy->GetWorldTransform().world;
				FLOAT4X4 AttackTransform = getAttackBone * modelAxisTransform * worldTransform;

				runAttack.position[0] = { AttackTransform._41,AttackTransform._42,AttackTransform._43 };
				enemy->GetStatus().attackPoint = enemy->GetAttack(8).attackPoint;

				MESSENGER.EnemyAttackingMessage(enemy->GetID(), runAttack);
			}
			++m_chaseTimer;
		}


	}
	break;
	case 3:
	{
		if (JudgeBlendRatio(animation))
		{
			++m_moveState;
		}

	}
	break;
	case 4:
	{
		if (JudgeAnimationRatio(enemy, 3, 1))
		{
			//		animation.animationBlend.AddSampler(1, enemy->GetModel());
			++m_moveState;
		}
		auto& rightPunch = enemy->GetCollision().at(1);
		int rightPunchMesh = rightPunch.GetCurrentMesh(0);
		int rightPunchBone = rightPunch.GetCurrentBone(0);
		FLOAT4X4 getAttackBone = animation.animationBlend._blendLocals[rightPunchMesh].at(rightPunchBone);
		FLOAT4X4 modelAxisTransform = enemy->GetModel()->_resource->axisSystemTransform;
		FLOAT4X4 worldTransform = enemy->GetWorldTransform().world;
		FLOAT4X4 AttackTransform = getAttackBone * modelAxisTransform * worldTransform;

		rightPunch.position[0] = { AttackTransform._41,AttackTransform._42,AttackTransform._43 };
		enemy->GetStatus().attackPoint = enemy->GetAttack(5).attackPoint;

		MESSENGER.EnemyAttackingMessage(enemy->GetID(), rightPunch);
	}
	break;
	case 5:
	{
		if (JudgeBlendRatio(animation))
		{
			animation.animationBlend.ResetAnimationSampler(0);
			m_moveState = 0;
			m_taskState = TASK_STATE::END;
		}
	}
	break;
	}


}


bool EnemyFarAttack1Task::JudgeBlendRatio(CharacterParameter::BlendAnimation& animation)
{
	m_taskState = TASK_STATE::RUN;
	if (animation.animationBlend.GetSampler().size() == 1) return false;

	animation.animationBlend._blendRatio += 0.025f;//magicNumber
	if (animation.animationBlend._blendRatio >= animation.blendRatioMax)//magicNumber
	{
		animation.animationBlend._blendRatio = 0.0f;
		animation.animationBlend.ResetAnimationSampler(0);
		animation.animationBlend.ReleaseSampler(0);
		animation.animationBlend.FalseAnimationLoop(0);
		return true;
	}

	return false;
}

bool EnemyFarAttack1Task::JudgeAnimationRatio(Enemy* enemy, const int attackNo, const int nextAnimNo)
{
	uint32_t currentAnimationTime = enemy->GetBlendAnimation().animationBlend.GetAnimationTime(0);
	uint32_t attackFrameCount = enemy->GetAttack(attackNo).frameCount;
	if (currentAnimationTime >= attackFrameCount)
	{
		enemy->GetBlendAnimation().animationBlend.AddSampler(nextAnimNo, enemy->GetModel());

		//enemy->GetBlendAnimation().animationBlend.ResetAnimationFrame();
		return true;
	}

	return false;
}

uint32_t EnemyFarAttack1Task::JudgePriority(const int id)
{
	std::shared_ptr<CharacterAI> enemy = MESSENGER.CallEnemyInstance(id);
	uint32_t attackCount = enemy->GetJudgeElement().attackCount;
	uint32_t attackCountValue = enemy->GetStandardValue().attackCountValue;

	if (attackCount > attackCountValue)
		return m_priority;

	return minPriority;
}
