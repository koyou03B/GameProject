#pragma once
#include"Scene.h"

#ifdef _DEBUG
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "imgui_internal.h"
#endif


class Clear : public Scene
{
public:
	Clear() = default;
	virtual ~Clear() = default;

	bool Initialize(ID3D11Device* device) override;
	void Update(float& elapsedTime) override;
	void Render(ID3D11DeviceContext* immediateContext, float elapsedTime) override;
	void ImGui() override;
	void Uninitialize() override;
};