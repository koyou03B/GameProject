#pragma once
#include <vector>
#include <memory>
#include "Scene.h"
#include ".\LibrarySource\Vector.h"
#include ".\LibrarySource\Input.h"
#include ".\LibrarySource\SpriteBatch.h"
#include ".\LibrarySource\ConstantBuffer.h"
#include ".\LibrarySource\Constants.h"
#include ".\LibrarySource\FrameBuffer.h"
#include ".\LibrarySource\Rasterizer.h"
#include ".\LibrarySource\ScreenFilter.h"
#include ".\LibrarySource\SkinnedMesh.h"
#include ".\LibrarySource\StaticMesh.h"
#include ".\LibrarySource\Fog.h"
#ifdef _DEBUG
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "imgui_internal.h"
#endif

#include "TitleObjectAdominist.h"
class Title : public Scene
{
public:
	Title() = default;
	virtual ~Title() = default;

	bool Initialize(ID3D11Device* device)override;
	void Update(float& elapsedTime)override;
	void Render(ID3D11DeviceContext* immediateContext, float elapsedTime)override;
	void ImGui()override;
	void Uninitialize()override;

private:
	std::shared_ptr<Source::Sprite::SpriteBatch> m_sprite;
	std::unique_ptr<Source::FrameBuffer::FrameBuffer> m_frameBuffer[2];
	std::unique_ptr<Source::ScreenFilter::ScreenFilter> m_screenFilter;

	std::unique_ptr<TitleObjctAdominist> m_tObjectAdominist;
	std::unique_ptr<Source::ConstantBuffer::ConstantBuffer<Source::Constants::SceneConstants>> m_sceneConstantBuffer;
	std::unique_ptr<Source::Fog::Fog> m_fog;
	Source::Input::Input* m_input;

	int m_eventState = 0;
	bool isEvent = false;
};