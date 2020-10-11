#include "EnemyChaseTask.h"
#include "MessengTo.h"
#include "Enemy.h"

void EnemyChaseTask::Run(Enemy* enemy)
{
	auto& animation = enemy->GetBlendAnimation();
	switch (m_moveState)
	{
	case 0:
	{
		auto& players = MESSENGER.CallPlayersInstance();
		int targetID = enemy->GetJudgeElement().targetID;
		uint32_t targetAttackHitCount = players.at(targetID)->GetJudgeElement().attackHitCount;

		int playerCount = static_cast<int>(players.size());
		for (int i = 0; i < playerCount; ++i)
		{
			if (i != targetID)
			{
				uint32_t attackHitCount = players.at(i)->GetJudgeElement().attackHitCount;
				if (targetAttackHitCount < attackHitCount)
					m_targetID = i;
			}
		}

		enemy->GetJudgeElement().targetID = m_targetID;
		m_taskState = TASK_STATE::START;
		m_isLockOn = false;
		++m_moveState;

	}
	break;
	case 1:
	{
		std::shared_ptr<CharacterAI> player = MESSENGER.CallPlayerInstance(m_targetID);

		VECTOR3F playerPosition = player->GetWorldTransform().position;
		VECTOR3F enemyPosition = enemy->GetWorldTransform().position;

		float direction = ToDistVec3(playerPosition - enemyPosition);
		VECTOR3F normalizeDist = NormalizeVec3(playerPosition - enemyPosition);

		VECTOR3F angle = enemy->GetWorldTransform().angle;
		VECTOR3F front = VECTOR3F(sinf(angle.y), 0.0f, cosf(angle.y));
		front = NormalizeVec3(front);

		float dot = DotVec3(front, normalizeDist);

		float cosTheta = acosf(dot);

		float frontValue = enemy->GetStandardValue().viewFrontValue;
		if (cosTheta <= frontValue)
		{
			m_moveState = 0;
			m_isLockOn = false;
			m_taskState = TASK_STATE::END;
		}
		else
		{
			VECTOR3F cross = CrossVec3(front, normalizeDist);
			if (cross.y > 0.0f)
			{
				m_moveState = 2;
				animation.animationBlend.AddSampler(1, enemy->GetModel());
				animation.animationBlend.ResetAnimationFrame();
			}
			else
			{
				m_moveState = 3;
				animation.animationBlend.AddSampler(2, enemy->GetModel());
				animation.animationBlend.ResetAnimationFrame();
			}
		}
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

		auto player = MESSENGER.CallPlayersInstance();
		int targetID = enemy->GetJudgeElement().targetID;

		VECTOR3F playerPosition = player.at(targetID)->GetWorldTransform().position;
		VECTOR3F enemyPosition = enemy->GetWorldTransform().position;

		float direction = ToDistVec3(playerPosition - enemyPosition);
		VECTOR3F normalizeDist = NormalizeVec3(playerPosition - enemyPosition);

		VECTOR3F angle = enemy->GetWorldTransform().angle;
		VECTOR3F front = VECTOR3F(sinf(angle.y), 0.0f, cosf(angle.y));
		front = NormalizeVec3(front);

		float dot = DotVec3(front, normalizeDist);
		float rot = 1.0f - dot;
		float limit = enemy->GetMove().turnSpeed;
		if (rot > limit)
		{
			rot = limit;
		}
		float cosTheta = acosf(dot);
		float frontValue = enemy->GetStandardValue().viewFrontValue;
		if (cosTheta <= frontValue)
			m_isLockOn = true;
		
		uint32_t currentAnimationTime = enemy->GetBlendAnimation().animationBlend.GetAnimationTime(0);

		if (m_moveAnimationTime > currentAnimationTime)
		{
			auto& enemyTransform = enemy->GetWorldTransform();
			enemyTransform.angle.y += rot;
			enemyTransform.WorldUpdate();
		}

		if (m_isLockOn && currentAnimationTime == 0)
		{
			if (animation.animationBlend.GetSampler().size() == 2)
			{
				animation.animationBlend.ResetAnimationSampler(0);
				animation.animationBlend.ReleaseSampler(0);
			}

			animation.animationBlend.AddSampler(0, enemy->GetModel());
			animation.animationBlend.ResetAnimationFrame();
			m_moveState = 4;
		}
	}
	break;
	case 3:
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

		auto player = MESSENGER.CallPlayersInstance();
		int targetID = enemy->GetJudgeElement().targetID;

		VECTOR3F playerPosition = player.at(targetID)->GetWorldTransform().position;
		VECTOR3F enemyPosition = enemy->GetWorldTransform().position;

		float direction = ToDistVec3(playerPosition - enemyPosition);
		VECTOR3F normalizeDist = NormalizeVec3(playerPosition - enemyPosition);

		VECTOR3F angle = enemy->GetWorldTransform().angle;
		VECTOR3F front = VECTOR3F(sinf(angle.y), 0.0f, cosf(angle.y));
		front = NormalizeVec3(front);
		float dot = DotVec3(front, normalizeDist);
		float rot = 1.0f - dot;
		float limit = enemy->GetMove().turnSpeed;
		if (rot > limit)
		{
			rot = limit;
		}
		float cosTheta = acosf(dot);
		float frontValue = enemy->GetStandardValue().viewFrontValue;
		if (cosTheta <= frontValue)
			m_isLockOn = true;
		uint32_t currentAnimationTime = enemy->GetBlendAnimation().animationBlend.GetAnimationTime(0);

		if (m_moveAnimationTime > currentAnimationTime)
		{
			auto& enemyTransform = enemy->GetWorldTransform();
			enemyTransform.angle.y -= rot;
			enemyTransform.WorldUpdate();
		}

		if (m_isLockOn && currentAnimationTime == 0)
		{
			if (animation.animationBlend.GetSampler().size() == 2)
			{
				animation.animationBlend.ResetAnimationSampler(0);
				animation.animationBlend.ReleaseSampler(0);
			}

			animation.animationBlend.AddSampler(0, enemy->GetModel());
			animation.animationBlend.ResetAnimationFrame();
			++m_moveState;
		}
	}
	break;
	case 4:
	{
		if (JudgeBlendRatio(animation))
		{
			animation.animationBlend.ResetAnimationSampler(0);
			m_moveState = 0;
			m_isLockOn = false;
			m_taskState = TASK_STATE::END;
		}
	}
	break;
	}


}

bool EnemyChaseTask::JudgeBlendRatio(CharacterParameter::BlendAnimation& animation)
{
	animation.animationBlend._blendRatio += 0.045f;//magicNumber
	if (animation.animationBlend._blendRatio >= animation.blendRatioMax)//magicNumber
	{
		animation.animationBlend._blendRatio = 0.0f;
		animation.animationBlend.ResetAnimationSampler(0);
		animation.animationBlend.ReleaseSampler(0);
		return true;
	}

	return false;
}

uint32_t EnemyChaseTask::JudgePriority(const int id)
{
	return m_priority;
}
