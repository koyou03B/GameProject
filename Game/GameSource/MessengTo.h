#pragma once
#include "MetaAI.h"
#include "UIAdominist.h"
#include "MessengType.h"

class MessengTo
{
public:
	MessengTo() = default;
	~MessengTo() = default;

	CharacterAI* CallEnemyInstance(const EnemyType type)
	{
		return m_metaAI->GetEnemyAdominist().GetSelectEnemy(type);
	}

	CharacterAI* CallPlayerInstance(const PlayerType type)
	{
		return m_metaAI->GetPlayerAdominist().GetSelectPlayer(type);
	}

	bool AttackingMessage(const PlayerType type,CharacterParameter::Collision& player)
	{
		if (m_metaAI->CollisionPlayerAttack(type, player))
			return true;

		return false;
	}

	bool AttackingMessage(const EnemyType type, CharacterParameter::Collision& enemy)
	{
		if (m_metaAI->CollisionEnemyAttack(type, enemy))
			return true;

		return false;
	}

	void MessageToLifeUpdate(const float currentHP, const float maxHP,
		const UIActLabel label, const uint32_t id)
	{
		m_uiAdominist->LifeUpdate(label, (maxHP - currentHP) / maxHP, id);
	}

	void MessageToRecoverEffect(const VECTOR3F position, const int count,const int targetID)
	{
		m_metaAI->ActiveRecoverEffect(position, count, targetID);
	}

	inline void SetMetaAI(const std::shared_ptr<MetaAI>& metaAI)
	{
		m_metaAI = metaAI;
	}

	inline void SetUIAdominist(const std::shared_ptr<UIAdominist>& uiAdominist)
	{
		m_uiAdominist = uiAdominist;
	}

	inline static MessengTo& GetInstance()
	{
		static MessengTo messegeTo;
		return messegeTo;
	};

private:
	std::shared_ptr<MetaAI> m_metaAI;
	std::shared_ptr<UIAdominist> m_uiAdominist;
};


#define MESSENGER	 MessengTo::GetInstance()