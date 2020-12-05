#include "MetaAI.h"
#include "Enemy.h"
#include "SaberCharacter.h"
#include "ArcherCharacter.h"
#include "FighterCharacter.h"
#include "Collision.h"

bool MetaAI::Init()
{
	m_enemyID = 0;
	m_playerID = 0;
	AddEnemy(m_enemyID,std::make_shared<Enemy>());

	//AddPlayer(m_playerID, std::make_shared<Saber>());
	//Archer
	//AddPlayer(m_playerID, std::make_shared<Archer>());
	AddPlayer(m_playerID, std::make_shared<Fighter>());

	//Archer
	//m_scope = std::make_unique<Scope>();
	//m_scope->Init();
	return true;
}

void MetaAI::UpdateOfEnemys(float& elapsedTime)
{
	auto enemyCount = m_enemys.size();

	for (int i = 0; i < enemyCount; ++i)
	{
		if (m_enemys[i] != nullptr)
			m_enemys[i]->Update(elapsedTime);
	}
}

void MetaAI::RenderOfEnemy(ID3D11DeviceContext* immediateContext, uint16_t characterNomber)
{
	auto enemyCount = m_enemys.size();

	for (int i = 0; i < enemyCount; ++i)
	{
		if (m_enemys[i] != nullptr)
			m_enemys[i]->Render(immediateContext);
	}
}

void MetaAI::ImGuiOfEnemy(ID3D11Device* device, uint16_t characterNomber)
{
	if(m_enemys[characterNomber] != nullptr)
		m_enemys[characterNomber]->ImGui(device);
}

void MetaAI::AddEnemy(uint16_t& enemyID, std::shared_ptr<CharacterAI> enemy)
{
	enemy->SetID(enemyID);
	++enemyID;
	enemy->Init();
	m_enemys.emplace_back(enemy);
}

void MetaAI::DeterminationOfEnemy(int id, MessengType type)
{
	switch (type)
	{
	case MessengType::ASK_ATTACK:
	{
		int count = 0;
		for (auto& enemy : m_enemys)
		{
			if (enemy->GetMessageFlg().askAttack)
				++count;
		}

		if (count == 0)
			m_enemys[id]->GetMessageFlg().askAttack = true;
	}
	break;
	case MessengType::ASK_PROTECTION:
		break;
	case MessengType::CALL_FRIEND:
		break;
	case MessengType::CALL_HELP:
		break;
	case MessengType::TELL_DEAD:
		m_isFinish[1] = true;
		break;
	default:
		break;
	}
}


void MetaAI::UpdateOfPlayers(float& elapsedTime)
{
	auto playerCount = m_players.size();

	for (int i = 0; i < playerCount; ++i)
	{
		if (m_players[i] != nullptr)
		{
			m_players[i]->Update(elapsedTime);
		
			CollisionPlayerAndEnemy(m_players[i]);
		}
	}

	//Archer
	//if (m_scope->GetAimMode())
	//{
	//	VECTOR3F target = m_enemys[0]->GetWorldTransform().position;
	//	m_scope->Update(target);
	//}
}

void MetaAI::RenderOfPlayer(ID3D11DeviceContext* immediateContext, uint16_t characterNomber)
{
	if (m_players[characterNomber] != nullptr)
		m_players[characterNomber]->Render(immediateContext);
}

void MetaAI::ImGuiOfPlayer(ID3D11Device* device, uint16_t characterNomber)
{
	if (m_players[characterNomber] != nullptr)
		m_players[characterNomber]->ImGui(device);
}

void MetaAI::AddPlayer(uint16_t& playerID, std::shared_ptr<CharacterAI> player)
{
	player->SetID(playerID);
	++playerID;
	player->Init();
	m_players.emplace_back(player);
}

#include "FighterState.h"
void MetaAI::DeterminationOfPlayer(int id, MessengType type)
{
	switch (type)
	{
	case MessengType::CALL_FRIEND:
		break;
	case MessengType::CALL_HELP:
		if (m_players[id]->GetChangeComand().isPlay)
		{
			m_players[id]->SetState(MovekState::GetInstance());
		}

		break;
	case MessengType::CHANGE_PLAYER:
		if (m_players[id]->GetChangeComand().isPlay)
		{
			m_players[id]->GetChangeComand().isPlay = false;
			int nextPlay = static_cast<int>(m_players[id]->GetChangeComand().changeType);

			assert(id != nextPlay);

			m_players[nextPlay]->GetChangeComand().isPlay = true;
		
			VECTOR3F playerPosition = m_players[nextPlay]->GetWorldTransform().position;
			VECTOR3F enemyPosition = m_enemys[0]->GetWorldTransform().position;

			VECTOR3F distance = playerPosition - enemyPosition;
			distance = NormalizeVec3(distance);

			FLOAT4X4 modelAxisTransform = m_players[nextPlay]->GetModel()->_resource->axisSystemTransform;
			FLOAT4X4 world = modelAxisTransform * m_players[nextPlay]->GetWorldTransform().world;
			VECTOR3F rightAxis = { world._11,world._12,world._13 };
			rightAxis = NormalizeVec3(rightAxis);
			VECTOR3F rigthValue = rightAxis * m_players[nextPlay]->GetCamera().rightValue;

			Source::CameraControlle::CameraManager().GetInstance()->SetRigth(rigthValue);
			Source::CameraControlle::CameraManager().GetInstance()->SetDistance(distance);
			Source::CameraControlle::CameraManager().GetInstance()->SetObject(playerPosition);
			Source::CameraControlle::CameraManager().GetInstance()->SetLength(m_players[nextPlay]->GetCamera().lenght);
			Source::CameraControlle::CameraManager().GetInstance()->SetHeightAboveGround(m_players[nextPlay]->GetCamera().heightAboveGround);
			Source::CameraControlle::CameraManager().GetInstance()->SetNextEye();
			Source::CameraControlle::CameraManager().GetInstance()->SetValue(0.f);
			Source::CameraControlle::CameraManager().GetInstance()->SetCameraMode(Source::CameraControlle::CameraManager().CameraMode::CHANGE_OBJECT);
		
		}
		break;
	case MessengType::SHIFT_AIM_MODE:
		m_scope->SetAimMode();
		break;
	case MessengType::TELL_DEAD:
		m_isFinish[0] = true;
		break;
	}
}

void MetaAI::RenderOfScope(ID3D11DeviceContext* immediateContext)
{
	if (m_scope->GetAimMode())
	{
		m_scope->Render(immediateContext);
	}
}

void MetaAI::ImGuiOfScope(ID3D11Device* device)
{
	m_scope->ImGui(device);
}

bool MetaAI::CollisionPlayerAttack(int id, CharacterParameter::Collision& collision)
{
	if (m_players[id]->GetStatus().isAttack && m_players[id]->GetStatus().isDamage)
		return false;

	switch (collision.collisionType)
	{
	case CharacterParameter::Collision::SPHER:
	{
		Collision::Sphere mySelf, target;
		mySelf.position = collision.position[0];
		mySelf.radius = collision.radius;
		mySelf.scale = collision.scale;
		auto& enemy = m_enemys[0]->GetCollision()[0];
		target.position = enemy.position[0];
		target.radius = enemy.radius;
		target.scale = enemy.scale;
		Collision collision;
		if (collision.JudgeSphereAndSphere(mySelf, target))
		{
			Source::CameraControlle::CameraManager::GetInstance()->SetVibration(0.5f,0.5f);
			++m_players[id]->GetJudgeElement().attackHitCount;
			++m_enemys[0]->GetJudgeElement().damageCount;
			m_enemys[0]->GetStatus().life -= m_players[id]->GetStatus().attackPoint;

			MESSENGER.MessageToLifeUpdate(m_enemys[0]->GetStatus().life,m_enemys[0]->GetStatus().maxLife ,
				UIActLabel::LIFE_E, 0);

			if (m_enemys[0]->GetStatus().life <= 0)
			{
				m_enemys[0]->GetStatus().isExit = false;
			}
			return true;
		}
	}
	break;
	case CharacterParameter::Collision::CAPSULE:
	{
		Collision::Capsule mySelf;
		mySelf.startPos = collision.position[0];
		mySelf.endPos = collision.position[1];
		mySelf.radius = collision.radius;
		mySelf.scale = collision.scale;
		Collision::Sphere target;
		auto& enemy = m_enemys[0]->GetCollision()[0];
		target.position = enemy.position[0];
		target.radius = enemy.radius;
		target.scale = enemy.scale;
		Collision collision;
		if (collision.JudgeCapsuleAndSphere(mySelf, target))
		{
			++m_players[id]->GetJudgeElement().attackHitCount;
			++m_enemys[0]->GetJudgeElement().damageCount;
			m_enemys[0]->GetStatus().life -= m_players[id]->GetStatus().attackPoint;
			if (m_enemys[0]->GetStatus().life <= 0)
			{
				m_enemys[0]->GetStatus().isExit = false;
			}
			return true;
		}

	}
	break;
	}

	return false;
}

bool MetaAI::CollisionEnemyAttack(int id, CharacterParameter::Collision& collision)
{
	switch (collision.collisionType)
	{
	case CharacterParameter::Collision::SPHER:
	{
		Collision::Sphere mySelf, target;
		mySelf.position = collision.position[0];
		mySelf.radius = collision.radius;
		mySelf.scale = collision.scale;

		for (auto& player : m_players)
		{
			if (player->GetStatus().isDamage) continue;
			int targetID = m_enemys[id]->GetJudgeElement().targetID;
			auto& playerColl = player->GetCollision().at(0);
			target.position = playerColl.position[0];
			target.radius = playerColl.radius;
			target.scale = playerColl.scale;

			Collision collision;
			if (collision.JudgeSphereAndSphere(mySelf, target))
			{
				++m_enemys[id]->GetJudgeElement().attackHitCount;
				float attackPoint = m_enemys[id]->GetStatus().attackPoint;
				float hitComparison = player->GetDamage().hitComparison;
				if (hitComparison < attackPoint)
					player->GetDamage().hasBigDamaged = true;

				player->GetStatus().life -= m_enemys[id]->GetStatus().attackPoint;
			//	m_enemys[id]->GetStatus().isAttack = true;
				MESSENGER.MessageToLifeUpdate(player->GetStatus().life, player->GetStatus().maxLife,
					UIActLabel::LIFE_P, player->GetID());
				VECTOR3F vector = m_enemys[id]->GetWorldTransform().position - player->GetWorldTransform().position;

				player->GetDamage().vector = vector;

				//if (m_scope->GetAimMode()) m_scope->GetAimMode() = false;
				if (player->GetStatus().life <= 0)
				{
					player->GetStatus().isExit = false;
				}
				else
					player->Impact();
				return true;
			}
		}


	}
	break;
	case CharacterParameter::Collision::CAPSULE:
	{
		Collision::Capsule mySelf;
		mySelf.startPos = collision.position[0];
		mySelf.endPos = collision.position[1];
		mySelf.radius = collision.radius;
		mySelf.scale = collision.scale;
		Collision::Sphere target;

		for (auto& player : m_players)
		{
			if (player->GetStatus().isDamage) continue;
			int targetID = m_enemys[id]->GetJudgeElement().targetID;
			auto& playerColl = player->GetCollision().at(0);
			target.position = playerColl.position[0];
			target.radius = playerColl.radius;
			target.scale = playerColl.scale;

			Collision collision;
			if (collision.JudgeCapsuleAndSphere(mySelf, target))
			{
				++m_enemys[id]->GetJudgeElement().attackHitCount;
				player->GetStatus().life -= m_enemys[id]->GetStatus().attackPoint;
				MESSENGER.MessageToLifeUpdate(player->GetStatus().life, player->GetStatus().maxLife,
					UIActLabel::LIFE_P, player->GetID());
				//m_enemys[id]->GetStatus().isAttack = true;
				//if (m_scope->GetAimMode()) m_scope->GetAimMode() = false;
				if (m_players[targetID]->GetStatus().life <= 0)
				{
					m_players[targetID]->GetStatus().isExit = false;
				}
				else
					player->Impact();
				return true;
			}
		}
	}
	break;
	}

	return false;
}

void MetaAI::CollisionPlayerAndEnemy(std::shared_ptr<CharacterAI>& player)
{
	Collision::Circle mySelf, target;
	if (player->GetCollision().empty()) return;
	mySelf.position = { player->GetCollision()[0].position[0].x,player->GetCollision()[0].position[0].z };
	mySelf.radius = player->GetCollision()[0].radius;
	mySelf.scale =  player->GetCollision()[0].scale;

	auto& enemy = m_enemys[0]->GetCollision()[0];
	target.position = { enemy.position[0].x,enemy.position[0].z };
	target.radius = enemy.radius;
	target.scale = enemy.scale;
	Collision collision;
	if (collision.JudgeCircleAndCircle(mySelf, target))
	{
		VECTOR2F distance;
		distance.x = mySelf.position.x - enemy.position[0].x;
		distance.y = mySelf.position.y - enemy.position[0].z;

		float length = sqrtf(distance.x * distance.x + distance.y * distance.y);
		float sub = (mySelf.radius + target.radius) - length;
		distance.x /= length;
		distance.y /= length;
		distance.x *= sub;
		distance.y *= sub;
		player->GetWorldTransform().position.x += distance.x;
		player->GetWorldTransform().position.z += distance.y;

		player->GetWorldTransform().WorldUpdate();
	}

}


void MetaAI::Release()
{
	m_enemyID = m_playerID = -1;
	ReleaseCharacter();
}

void MetaAI::ReleaseCharacter()
{

	for (auto& enemy : m_enemys)
	{
		enemy->Release();
	}

	for (auto& player : m_players)
	{
		player->Release();
	}


	m_enemys.clear();
	m_players.clear();
}


