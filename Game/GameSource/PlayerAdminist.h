#pragma once
#include <map>
#include "PlayCharacters.h"
#include ".\LibrarySource\Vector.h"

class PlayerAdminist
{
public:
	PlayerAdminist() = default;
	~PlayerAdminist() = default;

	bool Init();

	void Update(float& elapsedTime);

	void Render(ID3D11DeviceContext* immediateContext,uint16_t characterNomber);

	void ImGui(ID3D11Device* device,uint16_t characterNomber);

	void ReleaseCharacter();

    void AddPlayer(uint16_t characterNomber, std::shared_ptr<PlayCharacter> character);

	inline std::map<uint16_t, std::shared_ptr<PlayCharacter>>& GetPlayers() { return m_players; }

private:
	std::map<uint16_t,std::shared_ptr<PlayCharacter>> m_players;
};