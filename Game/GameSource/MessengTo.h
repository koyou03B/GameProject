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

	bool AttackingMessage(const int id,CharacterParameter::Collision& player)
	{
		if (m_metaAI->CollisionPlayerAttack(id, player))
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