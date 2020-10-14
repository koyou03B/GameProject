#include "EnemyFarAttack0Task.h"
#include "MessengTo.h"
#include "Enemy.h"

void EnemyFarAttack0Task::Run(Enemy* enemy)
{
	auto& animation = enemy->GetBlendAnimation();

	switch (m_moveState)
	{
	case 0:
	{
		m_taskState = TASK_STATE::START;
		animation.animationBlend.AddSampler(16, enemy->GetModel());
		animation.animationBlend.ResetAnimationFrame();
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
		m_targetSpeed = targetDist / 180.0f;
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
			animation.animationBlend.AddSampler(13, enemy->GetModel());
			++m_moveState;
		}
		else
		{
			auto& enemyTransform = enemy->GetWorldTransform();

			if (m_chaseTimer <= 180.0f)
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
			++m_chaseTimer;
		}

		//if(JudgeBlendRatio(animation))
		//	++m_moveState;
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
		if (JudgeAnimationRatio(enemy, 5, 1))
		{
			//animation.animationBlend.AddSampler(1, enemy->GetModel());
			++m_moveState;
		}
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


bool EnemyFarAttack0Task::JudgeBlendRatio(CharacterParameter::BlendAnimation& animation)
{
	m_taskState = TASK_STATE::RUN;
	if (animation.animationBlend.GetSampler().size() == 1) return false;

	animation.animationBlend._blendRatio += 0.045f;//magicNumber
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

bool EnemyFarAttack0Task::JudgeAnimationRatio(Enemy* enemy, const int attackNo, const int nextAnimNo)
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

uint32_t EnemyFarAttack0Task::JudgePriority(const int id)
{
	std::shared_ptr<CharacterAI> enemy = MESSENGER.CallEnemyInstance(id);
	uint32_t attackCount = enemy->GetJudgeElement().attackCount;
	uint32_t attackCountVaule = enemy->GetStandardValue().attackCountValue;
	if (attackCount <= attackCountVaule)
		return m_priority;

	return minPriority;
}
