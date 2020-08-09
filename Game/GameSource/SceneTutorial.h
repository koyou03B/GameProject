#pragma once
#include<memory>
#include"Scene.h"

class Tutorial : public Scene
{
public:
	Tutorial() = default;
	virtual ~Tutorial() = default;

	bool Initialize(ID3D11Device* device)override;
	void Update(float& elapsedTime)override;
	void Render(ID3D11DeviceContext* immediateContext, float elapsedTime)override;
	void ImGui()override;
	void Uninitialize()override;
};