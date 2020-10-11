#include "EnemyFarAttack0Task.h"
#include "MessengTo.h"
#include "Enemy.h"

void EnemyFarAttack0Task::Run(Enemy* enemy)
{
	auto& animation = enemy->GetBlendAnimation();
# if 0
	switch (m_moveState)
	{
	case 0:
	{
		m_taskState = TASK_STATE::START;
		animation.animationBlend.AddSampler(4, enemy->GetModel());
		animation.animationBlend.ResetAnimationFrame();
		++m_moveState;
	}
	break;
	case 1:
	{
		m_taskState = TASK_STATE::RUN;
		if (JudgeBlendRatio(animation))
		{
			++m_moveState;
			animation.animationBlend.ResetAnimationSampler(0);
		}
	}
	break;
	case 2:
	{
		//プレイヤーの方向に走る
		//距離が近づくと変更
		int targetID = enemy->GetJudgeElement().targetID;
		std::shared_ptr<CharacterAI> player = MESSENGER.CallPlayerInstance(targetID);
		auto& playerTransform = player->GetWorldTransform();
		auto& enemyTransform = enemy->GetWorldTransform();
		VECTOR3F distance = playerTransform.position - enemyTransform.position;
		float dist = ToDistVec3(distance);
		VECTOR3F velocity = NormalizeVec3(distance);

		VECTOR3F enemyAngle = VECTOR3F(sinf(enemyTransform.angle.y), 0.0f, cosf(enemyTransform.angle.y));
		enemyAngle = NormalizeVec3(enemyAngle);

		float dot = DotVec3(enemyAngle, velocity);
		float rot = 1.0f - dot;

		float limit = enemy->GetMove().turnSpeed;

		if (rot > limit)
		{
			rot = limit;
		}

		VECTOR3F cross = CrossVec3(enemyAngle, velocity);
		if (cross.y > 0.0f)
		{
			enemyTransform.angle.y += rot;
		}
		else
		{
			enemyTransform.angle.y -= rot;
		}

		enemy->GetMove().velocity = velocity * enemy->GetMove().accle;
		enemyTransform.position += enemy->GetMove().velocity * enemy->GetElapsedTime();
		enemyTransform.WorldUpdate();

		if (42.0f >= dist)//magicNumber
		{
			++m_moveState;
			animation.animationBlend.AddSampler(22, enemy->GetModel());
			animation.animationBlend.ResetAnimationFrame();
		}

	}
	break;
	case 3:
	{
		if (JudgeBlendRatio(animation))
		{
			++m_moveState;
		}
		float dist = ToDistVec3(enemy->GetMove().velocity);
		if (dist >= 10.0f)
		{
			int targetID = enemy->GetJudgeElement().targetID;
			std::shared_ptr<CharacterAI> player = MESSENGER.CallPlayerInstance(targetID);
			auto& playerTransform = player->GetWorldTransform();
			auto& enemyTransform = enemy->GetWorldTransform();
			VECTOR3F distance = playerTransform.position - enemyTransform.position;
			VECTOR3F velocity = NormalizeVec3(distance);
			enemy->GetMove().velocity -= velocity * (enemy->GetMove().accle *2.0);
			float dist = ToDistVec3(enemy->GetMove().velocity);
			if (dist >= 0.0f)
			{
				enemyTransform.position += enemy->GetMove().velocity * enemy->GetElapsedTime();
				enemyTransform.WorldUpdate();
			}
		}
	}
	break;
	case 4:
	{
		if (JudgeAnimationRatio(enemy, 8, 0))
			++m_moveState;
		float dist = ToDistVec3(enemy->GetMove().velocity);
		if (dist >= 10.0f)
		{
			int targetID = enemy->GetJudgeElement().targetID;
			std::shared_ptr<CharacterAI> player = MESSENGER.CallPlayerInstance(targetID);
			auto& playerTransform = player->GetWorldTransform();
			auto& enemyTransform = enemy->GetWorldTransform();
			VECTOR3F distance = playerTransform.position - enemyTransform.position;
			VECTOR3F velocity = NormalizeVec3(distance);
			enemy->GetMove().velocity -= velocity * (enemy->GetMove().accle * 2.0);
			enemyTransform.position += enemy->GetMove().velocity * enemy->GetElapsedTime();
			enemyTransform.WorldUpdate();

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
# else 

	switch (m_moveState)
	{
	case 0:
	{
		m_taskState = TASK_STATE::START;
		animation.animationBlend.AddSampler(5, enemy->GetModel());
		animation.animationBlend.ResetAnimationFrame();
		++m_moveState;
	}
	break;
	case 1:
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

		m_taskState = TASK_STATE::RUN;
		Chase(enemy);



	}
	break;
	case 2:
	{

	}
	break;
	case 3:
	{
		m_taskState = TASK_STATE::RUN;
		if (JudgeBlendRatio(animation))
		{
			++m_moveState;
			animation.animationBlend.ResetAnimationSampler(0);
		}
	}
	break;

	case 4:
	{
		//プレイヤーの方向に走る
		//距離が近づくと変更
		int targetID = enemy->GetJudgeElement().targetID;
		std::shared_ptr<CharacterAI> player = MESSENGER.CallPlayerInstance(targetID);
		auto& playerTransform = player->GetWorldTransform();
		auto& enemyTransform = enemy->GetWorldTransform();
		VECTOR3F distance = playerTransform.position - enemyTransform.position;
		float dist = ToDistVec3(distance);
		VECTOR3F velocity = NormalizeVec3(distance);

		VECTOR3F enemyAngle = VECTOR3F(sinf(enemyTransform.angle.y), 0.0f, cosf(enemyTransform.angle.y));
		enemyAngle = NormalizeVec3(enemyAngle);

		float dot = DotVec3(enemyAngle, velocity);
		float rot = 1.0f - dot;

		float limit = enemy->GetMove().turnSpeed;

		if (rot > limit)
		{
			rot = limit;
		}

		VECTOR3F cross = CrossVec3(enemyAngle, velocity);
		if (cross.y > 0.0f)
		{
			enemyTransform.angle.y += rot;
		}
		else
		{
			enemyTransform.angle.y -= rot;
		}

		enemy->GetMove().velocity = velocity * enemy->GetMove().accle;
		enemyTransform.position += enemy->GetMove().velocity * enemy->GetElapsedTime();
		enemyTransform.WorldUpdate();

		if (42.0f >= dist)//magicNumber
		{
			++m_moveState;
			animation.animationBlend.AddSampler(22, enemy->GetModel());
			animation.animationBlend.ResetAnimationFrame();
		}

	}
	break;
	case 5:
	{
		if (JudgeBlendRatio(animation))
		{
			++m_moveState;
		}
		float dist = ToDistVec3(enemy->GetMove().velocity);
		if (dist >= 10.0f)
		{
			int targetID = enemy->GetJudgeElement().targetID;
			std::shared_ptr<CharacterAI> player = MESSENGER.CallPlayerInstance(targetID);
			auto& playerTransform = player->GetWorldTransform();
			auto& enemyTransform = enemy->GetWorldTransform();
			VECTOR3F distance = playerTransform.position - enemyTransform.position;
			VECTOR3F velocity = NormalizeVec3(distance);
			enemy->GetMove().velocity -= velocity * (enemy->GetMove().accle * 2.0);
			float dist = ToDistVec3(enemy->GetMove().velocity);
			if (dist >= 0.0f)
			{
				enemyTransform.position += enemy->GetMove().velocity * enemy->GetElapsedTime();
				enemyTransform.WorldUpdate();
			}
		}
	}
	break;
	case 6:
	{
		if (JudgeAnimationRatio(enemy, 8, 0))
			++m_moveState;
		float dist = ToDistVec3(enemy->GetMove().velocity);
		if (dist >= 10.0f)
		{
			int targetID = enemy->GetJudgeElement().targetID;
			std::shared_ptr<CharacterAI> player = MESSENGER.CallPlayerInstance(targetID);
			auto& playerTransform = player->GetWorldTransform();
			auto& enemyTransform = enemy->GetWorldTransform();
			VECTOR3F distance = playerTransform.position - enemyTransform.position;
			VECTOR3F velocity = NormalizeVec3(distance);
			enemy->GetMove().velocity -= velocity * (enemy->GetMove().accle * 2.0);
			enemyTransform.position += enemy->GetMove().velocity * enemy->GetElapsedTime();
			enemyTransform.WorldUpdate();

		}
	}
	break;
	case 7:
	{
		if (JudgeBlendRatio(animation))
		{
			animation.animationBlend.ResetAnimationSampler(0);
			m_moveState = 0;
			m_taskState = TASK_STATE::END;
		}

	}
	break;
	};

#endif

}

void EnemyFarAttack0Task::Chase(Enemy* enemy)
{
	auto player = MESSENGER.CallPlayerInstance(m_targetID);
	auto playerTransform = player->GetWorldTransform();
	auto& enemyTransform = enemy->GetWorldTransform();

	VECTOR3F distance = playerTransform.position - enemyTransform.position;

	VECTOR3F nDistance = NormalizeVec3(distance);
	VECTOR3F enemyFront = VECTOR3F(sinf(enemyTransform.angle.y), 0.0f, cosf(enemyTransform.angle.y));
	enemyFront = NormalizeVec3(enemyFront);

	float dot = DotVec3(enemyFront, nDistance);
	float rot = 1.0f - dot;
	float limit = enemy->GetMove().turnSpeed;
	if (rot > limit)
	{
		rot = limit;
	}

	VECTOR3F cross = CrossVec3(enemyFront, nDistance);

	if (cross.y > 0.0f)
		enemyTransform.angle.y += rot;
	else
		enemyTransform.angle.y -= rot;

	float speed = ToDistVec3(distance);
	if (speed <= 20.0f)
		speed = 0.0f;

	VECTOR3F velocity = nDistance * speed;

	enemy->GetMove().accle = velocity - enemy->GetMove().velocity;
	enemy->GetMove().velocity += enemy->GetMove().accle;
	enemyTransform.position += enemy->GetMove().velocity * enemy->GetElapsedTime();

	enemyTransform.WorldUpdate();
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
