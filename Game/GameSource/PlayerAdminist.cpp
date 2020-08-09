#include "PlayerAdminist.h"
#include "SaberCharacter.h"
#include "ArcherCharacter.h"
#include "FighterCharacter.h"

bool PlayerAdminist::Init()
{
	AddPlayer(0, std::make_shared<Saber>());
	AddPlayer(1, std::make_shared<Archer>());
	AddPlayer(2, std::make_shared<Fighter>());

	m_players.at(0)->Init();
	m_players.at(1)->Init();
	m_players.at(2)->Init();

	return false;
}

void PlayerAdminist::Update(float& elapsedTime)
{
	auto playerCount = m_players.size();

	for (int i = 0; i < playerCount; ++i)
	{
		if(m_players[i] != nullptr)
			m_players[i]->Update(elapsedTime);
	}
}

void PlayerAdminist::Render(ID3D11DeviceContext* immediateContext,uint16_t characterNomber)
{
	auto character = m_players.find(characterNomber);
	if (character != m_players.end())
	{
		if (character->second != nullptr)
			character->second->Render(immediateContext);
	}

}

void PlayerAdminist::ImGui(ID3D11Device* device,uint16_t characterNomber)
{
	auto character = m_players.find(characterNomber);
	if (character != m_players.end())
	{
		if (character->second != nullptr)
			character->second->ImGui(device);
	}
}

void PlayerAdminist::ReleaseCharacter()
{
	auto playerCount = m_players.size();

	for (int i = 0; i < playerCount; ++i)
	{
		m_players.erase(i);
	}
}

void PlayerAdminist::AddPlayer(uint16_t characterNomber,std::shared_ptr<PlayCharacter> character)
{
	m_players.insert(std::make_pair(characterNomber, character));
}
