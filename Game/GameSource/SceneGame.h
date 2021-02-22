#pragma once
#include <vector>
#include <memory>
#include "Scene.h"
#include "MetaAI.h"
#include "Stage.h"
#include "UIAdominist.h"
#include "SceneEffect.h"
#include ".\LibrarySource\Camera.h"
#include ".\LibrarySource\Vector.h"
#include ".\LibrarySource\Constants.h"
#include ".\LibrarySource\ConstantBuffer.h"

#ifdef _DEBUG
#include "..\External_libraries\imgui\imgui.h"
#include "..\External_libraries\imgui\imgui_impl_dx11.h"
#include "..\External_libraries\imgui\imgui_impl_win32.h"
#include "..\External_libraries\imgui\imgui_internal.h"
#endif


class Game : public Scene
{
public:
	Game() = default;
	virtual ~Game() = default;

	bool Initialize(ID3D11Device* device) override;
	void Update(float& elapsedTime)override;
	void Render(ID3D11DeviceContext* immediateContext, float elapsedTime)override;
	void ImGui()override;
	void Uninitialize()override;

	VECTOR3F DistancePlayerToEnemy();
	VECTOR3F CameraRightValue();
private:
	enum GameEvent
	{
		START,
		FIGHT,
		WIN,
		LOSE
	};
private:
	std::unique_ptr<Source::ConstantBuffer::ConstantBuffer<Source::Constants::SceneConstants>> m_sceneConstantBuffer;

	VECTOR3F scale = { 1.0f, 1.0f, 1.0f };
	VECTOR3F angle = { 0.0f, 180.0f * 0.01745f, 0.0f };
	VECTOR3F translate = { 0.0f, 0.0f, .0f };
	VECTOR4F direction = { sinf(angle.y * 0.01745f), 0.0f, cosf(angle.y * 0.01745f),1.0f };
	float focalLength = 5.0f;
	float offsetY[3] = { 9.816f,5.479f,9.816f};
	SceneEffect m_sceneEffect;
	std::unique_ptr<Stage> m_stage;
	std::shared_ptr<MetaAI> m_metaAI;
	std::shared_ptr<UIAdominist> m_uiAdominist;

	GameEvent m_eventState = START;
	float m_vignetteTimer = 0.0f;
	bool m_isAct = 0;
};