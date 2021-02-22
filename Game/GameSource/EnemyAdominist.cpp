#include "EnemyAdominist.h"
#include "Enemy.h"

void EnemyAdominist::Init()
{
	int select = static_cast<int>(EnemyType::Boss);
	m_enemys[select] = std::make_unique<Enemy>();
	m_enemys[select]->Init();
}

void EnemyAdominist::Update(float& elapsedTime)
{
	int end = static_cast<int>(EnemyType::End);
	for (int i = 0; i < end; ++i)
	{
		m_enemys[i]->Update(elapsedTime);
	}
}

void EnemyAdominist::Render(ID3D11DeviceContext* immediateContext)
{
	int end = static_cast<int>(EnemyType::End);
	for (int i = 0; i < end; ++i)
	{
		m_enemys[i]->Render(immediateContext);
	}
}

void EnemyAdominist::Release()
{
	int end = static_cast<int>(EnemyType::End);
	for (int i = 0; i < end; ++i)
	{
		m_enemys[i]->Release();
	}
}

void EnemyAdominist::ImGui(ID3D11Device* device)
{
	int select = static_cast<int>(EnemyType::Boss);
	m_enemys[select]->ImGui(device);
}
