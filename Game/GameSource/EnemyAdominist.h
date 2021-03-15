#pragma once
#include <memory>
#include <d3d11.h>
#include "CharacterAI.h"

enum class EnemyType
{
	Boss,
	End
};

class EnemyAdominist
{
public:
	EnemyAdominist() = default;
	~EnemyAdominist() = default;

	void Init();
	void Update(float& elapsedTime);
	void Render(ID3D11DeviceContext* immediateContext);
	void Release();
	void Active();
	void ImGui(ID3D11Device* device);

	//中身いじれる用
	CharacterAI* GetSelectEnemy(const EnemyType type)
	{
		const int select = static_cast<int>(type);
		return m_enemys[select].get();
	}

	//中身いじれない用
	const CharacterAI* GetReadEnemy(const EnemyType type)
	{
		const int select = static_cast<int>(type);
		return m_enemys[select].get();
	}

private:
	std::unique_ptr<CharacterAI> m_enemys[static_cast<int>(EnemyType::End)];
};