#include "PlayerAdominist.h"
#include "FighterCharacter.h"
#include "ArcherCharacter.h"

void PlayerAdominist::Init()
{
	int select = static_cast<int>(PlayerType::Fighter);
	m_players[select] = std::make_unique<Fighter>();
	select = static_cast<int>(PlayerType::Archer);
	m_players[select] = std::make_unique<Archer>();
}

void PlayerAdominist::Update(float& elapsedTime)
{
	int end = static_cast<int>(PlayerType::End);
	for (int i = 0; i < end; ++i)
	{
		m_players[i]->Update(elapsedTime);
	}
}

void PlayerAdominist::Render(ID3D11DeviceContext* immediateContext)
{
	int end = static_cast<int>(PlayerType::End);
	for (int i = 0; i < end; ++i)
	{
		m_players[i]->Render(immediateContext);
	}
}

void PlayerAdominist::Release()
{
	int end = static_cast<int>(PlayerType::End);
	for (int i = 0; i < end; ++i)
	{
		m_players[i]->Release();
	}
}

void PlayerAdominist::ImGui(ID3D11Device* device)
{
	int no = static_cast<int>(PlayerType::Fighter);
	m_players[no]->ImGui(device);
	no = static_cast<int>(PlayerType::Archer);
	m_players[no]->ImGui(device);

}
