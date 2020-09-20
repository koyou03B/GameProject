#pragma once
#include <vector>
#include "Scope.h"
#include "CharacterAI.h"
#include "MessengType.h"
#include ".\LibrarySource\Vector.h"

class MetaAI
{
public:
	MetaAI() = default;
	~MetaAI() = default;

	bool Init();

	//******************************************************************************************
	// Enemy
	//******************************************************************************************
	void UpdateOfEnemys(float& elapsedTime);

	void RenderOfEnemy(ID3D11DeviceContext* immediateContext, uint16_t characterNomber);

	void ImGuiOfEnemy(ID3D11Device* device, uint16_t characterNomber);

	void AddEnemy(uint16_t& enemyID, std::shared_ptr<CharacterAI> enemy);

	void DeterminationOfEnemy(int id, MessengType type);
	//******************************************************************************************

	//******************************************************************************************
	// Player
	//******************************************************************************************
	void UpdateOfPlayers(float& elapsedTime);

	void RenderOfPlayer(ID3D11DeviceContext* immediateContext, uint16_t characterNomber);

	void ImGuiOfPlayer(ID3D11Device* device, uint16_t characterNomber);

	void AddPlayer(uint16_t& playerID, std::shared_ptr<CharacterAI> player);

	void DeterminationOfPlayer(int id, MessengType type);
	//******************************************************************************************
	
	//************
	// Scope
	//************
	void RenderOfScope(ID3D11DeviceContext* immediateContext);
	void ImGuiOfScope(ID3D11Device* device);
	//*****************
	// Collision
	//*****************
	bool CollisionPlayerAttack(int id, CharacterParameter::Collision& collision);
	void CollisionPlayerAndEnemy(std::shared_ptr<CharacterAI>& player);

	//****************
	// Release
	//****************
	void Release();
	void ReleaseCharacter();

	//******************
	// Gettet
	//******************
	inline std::vector<std::shared_ptr<CharacterAI>>& GetEnemys() { return m_enemys; }
	inline std::vector<std::shared_ptr<CharacterAI>>& GetPlayers() { return m_players; }
	inline CharacterAI* GetPlayCharacter()
	{
		int i = 0;
		for (i;i < m_playerID; ++i)
		{
			if (m_players[i]->GetChangeComand().isPlay)
				break;
		}
		return &(*m_players[i]);
	};
	inline std::unique_ptr<Scope>& GetScope() { return m_scope; }
private:
	uint16_t m_enemyID = -1;
	uint16_t m_playerID = -1;

	std::vector<std::shared_ptr<CharacterAI>> m_enemys;
	std::vector<std::shared_ptr<CharacterAI>> m_players;

	std::unique_ptr<Scope> m_scope;
};


