#include "MetaAI.h"
#include "Enemy.h"
#include "Collision.h"

bool MetaAI::Init(ID3D11Device* device)
{
	m_playerAdominist.Init();
	m_enemyAdominist.Init();
	m_effectManager.Init();

	m_sceneEffect.ChoiceSceneEffect(device, SceneEffectType::VIGNETTE);

	return true;
}

void MetaAI::Update(float& elapsedTime)
{
	m_enemyAdominist.Update(elapsedTime);
	m_playerAdominist.Update(elapsedTime);
	CollisionObject();	
	m_effectManager.Update(elapsedTime);
	m_sceneEffect.UpdateVignette(1.5f, 7.0f, 0.5f, elapsedTime);

}

void MetaAI::Render(ID3D11DeviceContext* immediateContext)
{
	m_enemyAdominist.Render(immediateContext);
	m_playerAdominist.Render(immediateContext);

}

void MetaAI::RenderOfEffect(ID3D11DeviceContext* immediateContext)
{
	FLOAT4X4 projection = Source::CameraControlle::CameraManager().GetInstance()->GetProjection();
	FLOAT4X4 view = Source::CameraControlle::CameraManager().GetInstance()->GetView();
	m_effectManager.Render(immediateContext, projection, view);
}

void MetaAI::ActivateEffect(ID3D11DeviceContext* immediateContext, const SceneEffectType type)
{
	m_sceneEffect.ActivateEffect(immediateContext, type);
}

void MetaAI::DeactivateEffect(ID3D11DeviceContext* immediateContext, const SceneEffectType type)
{
	m_sceneEffect.DeactivateEffect(immediateContext, type);
}

void MetaAI::ImGuiOfPlayer(ID3D11Device* device)
{
	m_playerAdominist.ImGui(device);
}

void MetaAI::ImGuiOfEnemy(ID3D11Device* device)
{
	m_enemyAdominist.ImGui(device);
}

void MetaAI::ImGuiOfEffect()
{
	m_effectManager.ImGui();
}

void MetaAI::ImGuiOfShader()
{
	m_sceneEffect.ImGui();
}

bool MetaAI::CollisionPlayerAttack(PlayerType type, CharacterParameter::Collision& collision)
{
	CharacterAI* player = m_playerAdominist.GetSelectPlayer(type);
	bool isDamage = player->GetStatus().isDamage;
	if (isDamage)
		return false;

	CharacterAI* enemy = m_enemyAdominist.GetSelectEnemy(EnemyType::Boss);
	if (m_collisionjudge.JudgeAttack(enemy->GetCollision().at(0), collision))
	{
		uint32_t& atkHitCount = player->GetJudgeElement().attackHitCount;
		uint32_t& damageCount = enemy->GetJudgeElement().damageCount;
		float& attackPoint    = player->GetStatus().attackPoint;
		float& enemyLife      = enemy->GetStatus().life;
		float enemyMaxLife    = enemy->GetStatus().maxLife;
		++damageCount;
		++atkHitCount;
		enemyLife -= attackPoint;

		if (type != PlayerType::Archer)
		{
			Source::CameraControlle::CameraManager::GetInstance()->SetVibration(0.5f, 0.5f);
			VECTOR3F effectPos = collision.position[0];
			VECTOR3F judgeNormal = NormalizeVec3(effectPos - player->GetWorldTransform().position);
			float offseZ = player->GetWorldTransform().position.z + judgeNormal.z * 5.0f;
			effectPos.z = offseZ;
			m_effectManager.SelectEffect(EffectType::FighterAttack, effectPos,1);
		}
		else
		{
			VECTOR3F effectPos = collision.position[0];
			float offsetY = effectPos.y+0.5f;
			VECTOR3F judgeNormal = NormalizeVec3(enemy->GetWorldTransform().position - effectPos);
			effectPos = effectPos + judgeNormal * 2.2f;
			effectPos.y = offsetY;
			m_effectManager.SelectEffect(EffectType::ArrowAttack, effectPos, 1);
			atkHitCount += 1;
		}
		MESSENGER.MessageToLifeUpdate(enemyLife, enemyMaxLife, UIActLabel::LIFE_E, 0);

		if (enemyLife <= 0)
		{
			bool& isExit = enemy->GetStatus().isExit;
			isExit = true;
			m_isWinner = true;
		}

		return true;
	}

	return false;
}

bool MetaAI::CollisionEnemyAttack(EnemyType type, CharacterParameter::Collision& collision)
{
	int maxSize = static_cast<int>(PlayerType::End);
	for (int i = 0; i < maxSize; ++i)
	{
		PlayerType playerType = static_cast<PlayerType>(i);
		CharacterAI* player   = m_playerAdominist.GetSelectPlayer(playerType);
		bool isDamage         = player->GetStatus().isDamage;
		bool invincibleMode   = player->GetJudgeElement().invincibleMode;

		if (isDamage || invincibleMode) continue;
		if (m_collisionjudge.JudgeAttack(player->GetCollision().at(0), collision))
		{
			CharacterAI* enemy    = m_enemyAdominist.GetSelectEnemy(type);
			uint32_t& atkhitCount = enemy->GetJudgeElement().attackHitCount;
			float& attackPoint    = enemy->GetStatus().attackPoint;
			float& playerLife     = player->GetStatus().life;
			float playerMaxLife = player->GetStatus().maxLife;
			float hitComparison   = player->GetDamage().hitComparison;

			++atkhitCount;
			playerLife -= attackPoint;
			MESSENGER.MessageToLifeUpdate(playerLife, playerMaxLife, UIActLabel::LIFE_P, i);
			if (hitComparison < attackPoint)
			{
				bool& hasBigDamaged = player->GetDamage().hasBigDamaged;
				hasBigDamaged = true;
			}

			VECTOR3F enemyPosition  = enemy->GetWorldTransform().position;
			VECTOR3F playerPosition = player->GetWorldTransform().position;
			VECTOR3F vector = enemyPosition - playerPosition;
			VECTOR3F& damageVector = player->GetDamage().vector;
			damageVector = vector;

			if (playerLife <= 0)
			{
				bool& isExit = player->GetStatus().isExit;
				isExit = false;
				m_isLoaser = true;
			}
			player->Impact();

			if (playerType == PlayerType::Fighter)
			{
				m_sceneEffect.StartVignette();
				m_playerAdominist.WriteBlackboard(player);
			}


			return true;
		}
	}
	return false;
}

void MetaAI::CollisionObject()
{
	int count = static_cast<int>(PlayerType::End);
	count += static_cast<int>(EnemyType::End);
	for (int i = 0; i < count; ++i)
	{
		PlayerType type = static_cast<PlayerType>(i);
		CharacterAI* player = nullptr;
		CharacterAI* target = nullptr;
		if (type == PlayerType::Fighter)
		{
			player = m_playerAdominist.GetSelectPlayer(type);
			target = m_enemyAdominist.GetSelectEnemy(EnemyType::Boss);
		}
		else if (type == PlayerType::Archer)
		{
			player = m_playerAdominist.GetSelectPlayer(type);
			target = m_enemyAdominist.GetSelectEnemy(EnemyType::Boss);
		}
		else
		{
			player = m_playerAdominist.GetSelectPlayer(PlayerType::Fighter);
			target = m_playerAdominist.GetSelectPlayer(PlayerType::Archer);
		}

		if(m_collisionjudge.JudgeDistnace(player, target))
		{
			VECTOR3F pBodyPos = player->GetWorldTransform().position;
			float pRadius = player->GetCollision()[0].radius;

			VECTOR3F tBodyPos = target->GetWorldTransform().position;
			float tRadius = target->GetCollision()[0].radius;

			VECTOR2F distance;
			distance.x = pBodyPos.x - tBodyPos.x;
			distance.y = pBodyPos.z - tBodyPos.z;

			float length = ToDistVec2(distance);
			float sub = (pRadius + tRadius) - length;
			distance.x /= length;
			distance.y /= length;
			distance.x *= sub;
			distance.y *= sub;
			player->GetWorldTransform().position.x += distance.x;
			player->GetWorldTransform().position.z += distance.y;
			player->GetWorldTransform().WorldUpdate();
		}
	}
}

void MetaAI::Release()
{
	m_playerAdominist.Release();
	m_enemyAdominist.Release();
}

void MetaAI::ActiveRecoverEffect(const VECTOR3F postion, const int count, const int targetID)
{
	m_effectManager.SelectEffect(EffectType::Heal, postion, count, targetID);
}

void MetaAI::ActivateEnemy()
{
	m_playerAdominist.Active();
	m_enemyAdominist.Active();
}


