#pragma once
#include <d3d11.h>
#include "SceneEffect.h"
#include "PlayerAdominist.h"
#include "EnemyAdominist.h"
#include "CollisionJudge.h"
#include "MessengType.h"
#include ".\LibrarySource\Vector.h"

class MetaAI
{
public:
	MetaAI() = default;
	~MetaAI() = default;

	bool Init(ID3D11Device* device);
	void Update(float& elapsedTime);
	void Render(ID3D11DeviceContext* immediateContext);
	void ActivateEffect(ID3D11DeviceContext* immediateContext, const SceneEffectType type);
	void DeactivateEffect(ID3D11DeviceContext* immediateContext, const SceneEffectType type);
	void Release();

	void ImGuiOfPlayer(ID3D11Device* device);
	void ImGuiOfEnemy(ID3D11Device* device);
	void ImGuiOfShader();

	bool CollisionPlayerAttack(PlayerType type, CharacterParameter::Collision& collision);
	bool CollisionEnemyAttack(EnemyType id, CharacterParameter::Collision& collision);
	void CollisionObject();

	PlayerAdominist& GetPlayerAdominist() { return m_playerAdominist; }
	EnemyAdominist& GetEnemyAdominist()	  { return m_enemyAdominist; }

	bool GetIsWinner() { return m_isWinner; }
	bool GetIsLoaser() { return m_isLoaser; }

private:
	bool m_isWinner;
	bool m_isLoaser;
	SceneEffect m_sceneEffect;

	PlayerAdominist m_playerAdominist;
	EnemyAdominist  m_enemyAdominist;
	CollisionJudge  m_collisionjudge;
};


