#include "MetaAI.h"
#include "Enemy.h"
#include "SaberCharacter.h"
#include "ArcherCharacter.h"
#include "FighterCharacter.h"

bool MetaAI::Init()
{
	m_enemyID = 0;
	m_playerID = 0;
	AddEnemy(m_enemyID,std::make_shared<Enemy>());

	//AddPlayer(m_playerID, std::make_shared<Saber>());
	//AddPlayer(m_playerID, std::make_shared<Archer>());
	AddPlayer(m_playerID, std::make_shared<Fighter>());

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
			if (m_players[i]->GetStatus().isAttack && !m_players[i]->GetStatus().isDamage)
			{
				for (auto& collision : m_players[i]->GetCollsion())
				{
					switch (collision.collisionType)
					{
					case CharacterParameter::Collision::CUBE:
						break;
					case CharacterParameter::Collision::SPHER:
						break;
					case CharacterParameter::Collision::CAPSULE:
							break;
					}
				}
			}
		}
	}
}

void MetaAI::RenderOfPlayer(ID3D11DeviceContext* immediateContext, uint16_t characterNomber)
{
	auto playerCount = m_players.size();

	for (int i = 0; i < playerCount; ++i)
	{
		if (m_players[i] != nullptr)
			m_players[i]->Render(immediateContext);
	}
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
		}
		break;
	}
}


void MetaAI::Release()
{
	m_enemyID = m_playerID = -1;
	ReleaseCharacter();
}

void MetaAI::ReleaseCharacter()
{
	m_enemys.clear();
	m_players.clear();
}


