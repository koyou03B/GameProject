#pragma once
#include "MetaAI.h"
#include "MessengType.h"

class MessengTo
{
public:
	MessengTo() = default;
	~MessengTo() = default;

	void MessageFromEnemy(const int id, MessengType type)
	{
		m_metaAI->DeterminationOfEnemy(id,type);
	}

	void MessageFromPlayer(const int id, MessengType type)
	{
		m_metaAI->DeterminationOfPlayer(id, type);
	}

	VECTOR3F CallScopePosition()
	{
		return m_metaAI->GetScope()->GetWorldScreenPosition();
	}

	std::vector<std::shared_ptr<CharacterAI>>& CallEnemysInstance()
	{
		return m_metaAI->GetEnemys();
	}

	std::shared_ptr<CharacterAI>& CallEnemyInstance(const int id)
	{
		return m_metaAI->GetEnemys().at(id);
	}

	std::vector<std::shared_ptr<CharacterAI>>& CallPlayersInstance()
	{
		return m_metaAI->GetPlayers();
	}

	std::shared_ptr<CharacterAI>& CallPlayerInstance(const int id)
	{
		return m_metaAI->GetPlayers().at(id);
	}

	bool AttackingMessage(const int id,CharacterParameter::Collision& player)
	{
		if (m_metaAI->CollisionPlayerAttack(id, player))
			return true;

		return false;
	}

	bool EnemyAttackingMessage(const int id, CharacterParameter::Collision& enemy)
	{
		if (m_metaAI->CollisionEnemyAttack(id, enemy))
			return true;

		return false;
	}

	inline void SetMetaAI(const std::shared_ptr<MetaAI>& metaAI)
	{
		m_metaAI = metaAI;
	}

	inline static MessengTo& GetInstance()
	{
		static MessengTo messegeTo;
		return messegeTo;
	};

private:
	std::shared_ptr<MetaAI> m_metaAI;
};


#define MESSENGER	 MessengTo::GetInstance()