#include "EnemyNearAttack0Task.h"
#include "MessengTo.h"
#include "Enemy.h"
#include ".\LibrarySource\Vector.h"

//magicNumber
void EnemyNearAttack0Task::Run(Enemy* enemy)
{
	auto& animation = enemy->GetBlendAnimation();
	switch (m_moveState)
	{
	case 0:
	{
		m_taskState = TASK_STATE::START;
		animation.animationBlend.AddSampler(8,enemy->GetModel());
		animation.animationBlend.ResetAnimationFrame();
		++m_moveState;
	}
	break;
	case 1:
	{
		m_taskState = TASK_STATE::RUN;
		if(JudgeBlendRatio(animation))
			++m_moveState;
	}
	break;
	case 2:
	{
		if (JudgeAnimationRatio(enemy, 0, 9))
			++m_moveState;
	}
	break;
	case 3:
	{
		if (JudgeBlendRatio(animation))
			++m_moveState;
	}
	break;
	case 4:
	{
		if (JudgeAnimationRatio(enemy, 1, 1))
			++m_moveState;
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

bool EnemyNearAttack0Task::JudgeBlendRatio(CharacterParameter::BlendAnimation& animation)
{
	m_taskState = TASK_STATE::RUN;
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

bool EnemyNearAttack0Task::JudgeAnimationRatio(Enemy* enemy, const int attackNo, const int nextAnimNo)
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

uint32_t EnemyNearAttack0Task::JudgePriority(const int id)
{
	auto player = MESSENGER.CallPlayersInstance();
	std::shared_ptr<CharacterAI> enemy = MESSENGER.CallEnemyInstance(id);
	int targetID = enemy->GetJudgeElement().targetID;

	VECTOR3F playerPosition = player.at(targetID)->GetWorldTransform().position;
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
		uint32_t attackHitCount = static_cast<uint32_t>(enemy->GetJudgeElement().attackHitCount);
		uint32_t attackHitCountValue = static_cast<uint32_t>(enemy->GetStandardValue().attackHitCountValue);

		if (attackHitCount < attackHitCountValue)
			return m_priority;
	}

	return minPriority;
}
