#pragma once
#include <memory>
#include <vector>
#include <d3d11.h>
#include "CharacterAI.h"

enum class PlayerType
{
	Fighter,
	Archer,
	End
};

class PlayerAdominist
{
public :
	PlayerAdominist() = default;
	~PlayerAdominist() = default;

	void Init();
	void Update(float& elapsedTime);
	void Render(ID3D11DeviceContext* immediateContext);
	void Release();
	void Active();
	void WriteBlackboard(CharacterAI* target);
	void ImGui(ID3D11Device* device);

	//中身いじれる用
	CharacterAI* GetSelectPlayer(const PlayerType type)
	{
		const int select = static_cast<int>(type);
		return m_players[select].get();
	}

	CharacterAI* GetPlayPlayer()
	{
		int select = static_cast<int>(PlayerType::Fighter);
		return m_players[select].get();
	}

	//中身いじれない用
	const CharacterAI* GetReadPlayer(const PlayerType type)
	{
		const int select = static_cast<int>(type);
		return m_players[select].get();
	}

private:
	std::unique_ptr<CharacterAI> m_players[static_cast<int>(PlayerType::End)];
};