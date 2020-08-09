#pragma once
#include <vector>
#include <memory>
#include "Scene.h"
#include "PlayerAdminist.h"
#include "Stage.h"
#include ".\LibrarySource\Vector.h"
#include ".\LibrarySource\Constants.h"
#include ".\LibrarySource\ConstantBuffer.h"
#include ".\LibrarySource\Camera.h"
#include ".\LibrarySource\EntityComponentSystem.h"

#ifdef _DEBUG
#include "..\External_libraries\imgui\imgui.h"
#include "..\External_libraries\imgui\imgui_impl_dx11.h"
#include "..\External_libraries\imgui\imgui_impl_win32.h"
#include "..\External_libraries\imgui\imgui_internal.h"
#endif


class Game : public Scene
{

	std::unique_ptr<Source::ConstantBuffer::ConstantBuffer<Source::Constants::SceneConstants>> m_sceneConstantBuffer;

	VECTOR3F scale = { 1.0f, 1.0f, 1.0f };
	VECTOR3F angle = { 0.0f, 180.0f * 0.01745f, 0.0f };
	VECTOR3F translate = { 0.0f, 0.0f, .0f };
	VECTOR4F direction = { sinf(angle.y * 0.01745f), 0.0f, cosf(angle.y * 0.01745f),1.0f };
	float focalLength = 5.0f;

	PlayerAdminist m_playerAdminist;
	Stage m_stage;
public:
	Game() = default;
	virtual ~Game() = default;

	bool Initialize(ID3D11Device* device) override;
	void Update(float& elapsedTime)override;
	void Render(ID3D11DeviceContext* immediateContext, float elapsedTime)override;
	void ImGui()override;
	void Uninitialize()override;
};